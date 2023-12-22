#include "irelia.h"
#include "sdk.hpp"
#include "spell.h"
#include "utils.h"

namespace irelia
{
    /* Spells */
    script::spell* q;
    script::spell* w;
    script::spell* e;
    script::spell* r;

    /* Variables & data */
    math::vector3 blade_position = {};
    std::string max_passive_buff_name = "ireliapassivestacksmax";
    std::string mark_passive_buff_name = "ireliamark";
    std::string stacks_passive_buff_name = "ireliapassivestacks";
    
    /* Items */
    bool has_blade_of_the_ruined_king;
    bool has_wits_end;
    bool has_nashors_tooth;
    bool has_sheen;
    bool has_divine_sunderer;
    bool has_trinity_force;
    bool has_essence_reaver;
    bool has_lich_bane;
    bool has_iceborn_gauntlet;

    /* Sheen items tracker */
    float last_sheen_t;
    std::unordered_map<std::string, bool> sheen_buffs = {
        {"sheen", true},
        {"3078trinityforce", true},
        {"6632buff", true},
        {"3508buff", true},
        {"lichbane", true},
        {"6662buff", true}
    };
    
    /* Events */
    void __fastcall game_update();
    void __fastcall present();
    void __fastcall wndproc( uint32_t msg, uint32_t wparam, uint32_t lparam );
    void __fastcall create_object( game_object* object );
    void __fastcall delete_object( game_object* object );
    void __fastcall create_missile( game_object* missile );
    void __fastcall basic_attack( game_object* object, game_object* target, spell_cast* cast );
    void __fastcall stop_cast( game_object* object, spell_cast* cast, bool was_attack_processed );
    void __fastcall process_cast( game_object* object, spell_cast* cast );
    void __fastcall buff_gain( game_object* object, buff_instance* buff );
    void __fastcall buff_loss( game_object* object, buff_instance* buff );
    void __fastcall draw_world();
    void __fastcall neutral_minion_kill( game_object* object, game_object* minion, int camp_side_team_id );
    void __fastcall new_path( game_object* object, bool is_dash, float dash_speed );
    void __fastcall execute_cast( game_object* object, spell_cast* cast );
    bool __fastcall issue_order( game_object* object, game_object_order order_type, math::vector3 position, game_object* target, bool move_pet );
    bool __fastcall cast_spell( game_object* object, int spell_slot, math::vector3 position, game_object* target );
    void __fastcall animation( game_object* object, uint32_t animation_hash );
    void __fastcall cast_heal( game_object* object, game_object* target, float amount );

    void register_spells()
    {
        q = new script::spell(0, 600.f);
        w = new script::spell(1, 0.f);
        e = new script::spell(2, 775.f);
        r = new script::spell(3, 0.f);

        e->set_skillshot(pred_sdk::spell_type::linear, pred_sdk::targetting_type::center_to_edge, 0.4f, 70.f, 2000.f, { pred_sdk::collision_type::yasuo_wall });
    }
    
    void create_config()
    {
        const auto& player = g_sdk->object_manager->get_local_player();
        const auto config = g_sdk->menu_manager->add_category("shulepin_aio_irelia","ShulepinAIO - " + player->get_char_name());
        {
            
        }
    }
    
    void load_events()
    {
        g_sdk->event_manager->register_callback( event_manager::event::game_update, reinterpret_cast< void* >( game_update ) );
        g_sdk->event_manager->register_callback( event_manager::event::present, reinterpret_cast< void* >( present ) );
        g_sdk->event_manager->register_callback( event_manager::event::wndproc, reinterpret_cast< void* >( wndproc ) );
        g_sdk->event_manager->register_callback( event_manager::event::create_object, reinterpret_cast< void* >( create_object ) );
        g_sdk->event_manager->register_callback( event_manager::event::delete_object, reinterpret_cast< void* >( delete_object ) );
        g_sdk->event_manager->register_callback( event_manager::event::create_missile, reinterpret_cast< void* >( create_missile ) );
        g_sdk->event_manager->register_callback( event_manager::event::basic_attack, reinterpret_cast< void* >( basic_attack ) );
        g_sdk->event_manager->register_callback( event_manager::event::stop_cast, reinterpret_cast< void* >( stop_cast ) );
        g_sdk->event_manager->register_callback( event_manager::event::process_cast, reinterpret_cast< void* >( process_cast ) );
        g_sdk->event_manager->register_callback( event_manager::event::buff_gain, reinterpret_cast< void* >( buff_gain ) );
        g_sdk->event_manager->register_callback( event_manager::event::buff_loss, reinterpret_cast< void* >( buff_loss ) );
        g_sdk->event_manager->register_callback( event_manager::event::draw_world, reinterpret_cast< void* >( draw_world ) );
        g_sdk->event_manager->register_callback( event_manager::event::neutral_minion_kill, reinterpret_cast< void* >( neutral_minion_kill ) );
        g_sdk->event_manager->register_callback( event_manager::event::new_path, reinterpret_cast< void* >( new_path ) );
        g_sdk->event_manager->register_callback( event_manager::event::execute_cast, reinterpret_cast< void* >( execute_cast ) );
        g_sdk->event_manager->register_callback( event_manager::event::issue_order, reinterpret_cast< void* >( issue_order ) );
        g_sdk->event_manager->register_callback( event_manager::event::cast_spell, reinterpret_cast< void* >( cast_spell ) );
        g_sdk->event_manager->register_callback( event_manager::event::animation, reinterpret_cast< void* >( animation ) );
        g_sdk->event_manager->register_callback( event_manager::event::cast_heal, reinterpret_cast< void* >( cast_heal ) );
    }
    
    void unload_events()
    {
        g_sdk->event_manager->unregister_callback( event_manager::event::game_update, reinterpret_cast< void* >( game_update ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::present, reinterpret_cast< void* >( present ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::wndproc, reinterpret_cast< void* >( wndproc ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::create_object, reinterpret_cast< void* >( create_object ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::delete_object, reinterpret_cast< void* >( delete_object ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::create_missile, reinterpret_cast< void* >( create_missile ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::basic_attack, reinterpret_cast< void* >( basic_attack ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::stop_cast, reinterpret_cast< void* >( stop_cast ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::process_cast, reinterpret_cast< void* >( process_cast ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::buff_gain, reinterpret_cast< void* >( buff_gain ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::buff_loss, reinterpret_cast< void* >( buff_loss ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::draw_world, reinterpret_cast< void* >( draw_world ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::neutral_minion_kill, reinterpret_cast< void* >( neutral_minion_kill ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::new_path, reinterpret_cast< void* >( new_path ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::execute_cast, reinterpret_cast< void* >( execute_cast ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::issue_order, reinterpret_cast< void* >( issue_order ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::cast_spell, reinterpret_cast< void* >( cast_spell ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::animation, reinterpret_cast< void* >( animation ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::cast_heal, reinterpret_cast< void* >( cast_heal ) );
    }
    
    void load()
    {
        register_spells();
        create_config();
        load_events();
    }

    void unload()
    {
        unload_events();
    }

    /* Common functions */

    float calculate_siphon_damage(const int level)
    {
        constexpr float base_damage = 40.f;
        constexpr float initial_increase = 7.f;
        constexpr float damage_increase_per_level = 7.f;

        if (level < 10) 
        {
            return base_damage;
        }

        const float additional_damage = initial_increase + (static_cast<float>(level) - 10.f) * damage_increase_per_level;

        return base_damage + additional_damage;
    }

    float calculate_wits_end_damage(const int level)
	{
        float base_damage = 15.0f;
        
        if (level >= 9)
        {
            base_damage += (std::min(level, 14) - 8) * 10.0f;
        }
        if (level >= 15)
        {
            base_damage += (1.25f * (level - 15)) + 1.25f;
        }
        
        return base_damage;
    }
    
    float calculate_q_damage(game_object* target)
    {
        const auto& time = g_sdk->clock_facade->get_game_time();
        const auto& player = g_sdk->object_manager->get_local_player();
        const auto player_level = player->get_level(); 
        const auto spell_level = q->get_level(); 
        const auto total_ad = player->get_attack_damage();
        const auto total_ap = player->get_ability_power();
        const auto base_ad = player->get_base_attack_damage();
        const auto bonus_ad = total_ad - base_ad;
        const auto target_hp = utils::get_real_health(target, "ALL");

        float physical_damage = 0.0f;
        float magical_damage = 0.0f;

        constexpr float base_damage[] = { 5.f, 25.f, 45.f, 65.f, 85.f };
        constexpr float ad_scaling_factor = 0.6f;

        if (target->is_minion())
        {
            physical_damage = base_damage[spell_level - 1] + (43.f + 12.f * static_cast<float>(player_level)) + (total_ad * ad_scaling_factor);
        }
        else
        {
            physical_damage = base_damage[spell_level - 1] + (total_ad * ad_scaling_factor);
        }

        std::string irelia_passive_buff = "ireliapassivestacksmax";
        if (player->get_buff_by_name(irelia_passive_buff))
        {
            const float irelia_passive_damage = (7.0f + 3.0f * static_cast<float>(player_level)) + (bonus_ad * 0.2f);
            magical_damage += irelia_passive_damage;
        }
        
        if (has_blade_of_the_ruined_king)
        {
            float botrk_damage = 0.12f * target->get_hp();
            botrk_damage = (target->is_minion() && botrk_damage > 60.f) ? 60.f : botrk_damage;
            physical_damage += botrk_damage;

            if (target->is_hero())
            {
                std::string buff_name = "item3153botrkstacks";
                const auto& buff = target->get_buff_by_name(buff_name);
                if (buff && buff->get_count() == 2)
                {
                    magical_damage += calculate_siphon_damage(player_level);
                }
            }
        }

        if (has_wits_end)
        {
            magical_damage += calculate_wits_end_damage(player_level);
        }

        if (has_nashors_tooth)
        {
            magical_damage += 15.0f + (0.2f * total_ap);
        }

        if (has_sheen)
        {
            std::string buff_name = "sheen";
            const auto& buff = player->get_buff_by_name(buff_name);
            if (buff || time >= last_sheen_t + 1.5f)
            {
                physical_damage += base_ad;
            }
        }

        if (has_divine_sunderer)
        {
            std::string buff_name = "6632buff";
            const auto& buff = player->get_buff_by_name(buff_name);
            if (buff || time >= last_sheen_t + 1.5f)
            {
                float divine_sunderer_damage = 1.6f * base_ad + (0.04f * target->get_max_hp());

                if (target->is_monster())
                {
                    divine_sunderer_damage = std::max(1.5f * base_ad, std::min(divine_sunderer_damage, 2.5f * base_ad));
                }
                physical_damage += divine_sunderer_damage;
            }
        }

        if (has_trinity_force)
        {
            std::string buff_name = "3078trinityforce";
            const auto& buff = player->get_buff_by_name(buff_name);
            if (buff || time >= last_sheen_t + 1.5f)
            {
                physical_damage += base_ad * 2;
            }
        }

        if (has_essence_reaver)
        {
            std::string buff_name = "3508buff";
            const auto& buff = player->get_buff_by_name(buff_name);
            if (buff || time >= last_sheen_t + 1.5f)
            {
                physical_damage += (base_ad * 1.3f) + (bonus_ad * 0.2f);
            }
        }

        if (has_lich_bane)
        {
            std::string buff_name = "lichbane";
            const auto& buff = player->get_buff_by_name(buff_name);
            if (buff || time >= last_sheen_t + 1.5f)
            {
                magical_damage += (base_ad * 0.75f) + (total_ap * 0.5f);
            }
        }

        if (has_iceborn_gauntlet)
        {
            std::string buff_name = "6662buff";
            const auto& buff = player->get_buff_by_name(buff_name);
            if (buff || time >= last_sheen_t + 1.5f)
            {
                physical_damage += base_ad;
            }
        }
        
        const auto calculated_physical_damage = sdk::damage->calc_damage(dmg_sdk::damage_type::physical, player, target, physical_damage);
        const auto calculated_magical_damage = sdk::damage->calc_damage(dmg_sdk::damage_type::magical, player, target, magical_damage);

        return calculated_physical_damage + calculated_magical_damage;
    }

    void update_items()
    {
        // TODO: Update items only when player around the shop
        
        const auto& player = g_sdk->object_manager->get_local_player();

        int blade_of_the_ruined_king_slot = -1;
        has_blade_of_the_ruined_king = player->has_item(3153, &blade_of_the_ruined_king_slot);

        int wits_end_slot = -1;
        has_wits_end = player->has_item(3091, &wits_end_slot);

        int nashors_tooth_slot = -1;
        has_nashors_tooth = player->has_item(3115, &nashors_tooth_slot);

        int sheen_slot = -1;
        has_sheen = player->has_item(3057, &sheen_slot);

        int divine_sunderer_slot = -1;
        has_divine_sunderer = player->has_item(6632, &divine_sunderer_slot);

        int trinity_force_slot = -1;
        has_trinity_force = player->has_item(3078, &trinity_force_slot);

        int essence_reaver_slot = -1;
        has_essence_reaver = player->has_item(3508, &essence_reaver_slot);

        int lich_bane_slot = -1;
        has_lich_bane = player->has_item(3100, &lich_bane_slot);

        int iceborn_gauntlet_slot = -1;
        has_iceborn_gauntlet = player->has_item(6662, &iceborn_gauntlet_slot);
    }

    bool can_kill_with_q(game_object* target)
    {
        const auto q_damage = calculate_q_damage(target);
        const auto target_hp = utils::get_real_health(target, "ALL");
        
        if (target->is_lane_minion())
        {
            // TODO: Add baron buff
            
            return q_damage > target_hp;
        }

        return q_damage > target_hp;
    }
    
    /* Logic */
    
    game_object* get_best_q_target()
    {
        const auto& player = g_sdk->object_manager->get_local_player();
        const auto& cursor_position = g_sdk->hud_manager->get_cursor_position();
        const auto stack_buff = player->get_buff_by_name(stacks_passive_buff_name);
        
        game_object* min_dist_obj = nullptr;
        game_object* min_dist_minion = nullptr;
        float original_dist = player->get_position().distance(cursor_position);
        float min_dist = original_dist;

        for (auto&& minion : g_sdk->object_manager->get_minions())
        {
            if (!minion ||
                !utils::is_enemy(minion) ||
                !utils::is_valid_target(minion, q->get_range()))
            {
                continue;
            }

            const float minion_dist = cursor_position.distance(minion->get_position());
            
            if (can_kill_with_q(minion) || minion->get_buff_by_name(mark_passive_buff_name))
            {
                if (minion_dist < min_dist) {
                    min_dist = minion_dist;
                    min_dist_minion = minion;
                    min_dist_obj = minion;
                }
            }
        }

        int q_count = 0;
        float hero_dist = FLT_MAX;
        
        for (auto&& hero : g_sdk->object_manager->get_heroes())
        {
            if (!hero ||
                !utils::is_enemy(hero) ||
                !utils::is_valid_target(hero, 2500.f))
            {
                continue;
            }

            const float dist_to_hero = hero->get_position().distance(player->get_position());
            const float dist_from_cursor_to_hero = cursor_position.distance(hero->get_position());
            
            if (dist_to_hero >= q->get_range() && dist_to_hero < hero_dist)
            {
                hero_dist = dist_to_hero;
            }

            if (hero->get_buff_by_name(mark_passive_buff_name) || can_kill_with_q(hero))
            {
                q_count++;
            }

            if ((hero->get_buff_by_name(mark_passive_buff_name) && (player->get_buff_by_name(max_passive_buff_name) || !min_dist_minion)) &&
                (dist_to_hero > utils::get_real_attack_range(hero, player) ||
                utils::get_real_health(hero, "AD") <= 2 * calculate_q_damage(hero) + sdk::damage->get_aa_damage(player, hero) ||
                (player->get_hp() / player->get_max_hp() <= 0.3 && hero->get_hp() > player->get_hp()) ||
                dist_from_cursor_to_hero > hero_dist ||
                r->is_ready() ||
                e->is_ready() ||
                q_count > 1 ||
                (stack_buff && stack_buff->get_count() == 4)) ||
                can_kill_with_q(hero))
            {
                if (dist_from_cursor_to_hero < min_dist)
                {
                    min_dist = dist_from_cursor_to_hero;
                    min_dist_obj = hero;
                }
            }
        }
        
        return min_dist_obj;
    }

    void combo_logic()
    {
        const auto& player = g_sdk->object_manager->get_local_player();
        
        if (!sdk::orbwalker->combo() ||
            sdk::orbwalker->would_cancel_attack())
        {
            return;
        }

        if (q->is_ready())
        {
            const auto best_q_target = get_best_q_target();
            if (best_q_target)
            {
                q->cast_spell(best_q_target, 0.1f);
            }
        }

        if (e->is_ready())
        {
            const auto& target = utils::get_target(e->get_range());
            if (target)
            {
                if (blade_position != math::vector3())
                {
                    
                }
                else
                {
                    
                }
            }
        }
    }
    
    /* Events */
    
    void __fastcall game_update()
    {
        update_items();
        combo_logic();
    }

    void __fastcall present()
    {
        for (auto&& hero : g_sdk->object_manager->get_heroes())
        {
            if (!hero ||
                !utils::is_enemy(hero) || 
                !utils::is_valid_target(hero, 1000.f))
            {
                continue;
            }

            const auto q_damage = calculate_q_damage(hero);
            g_sdk->renderer->add_text(std::to_string(q_damage), 20.f, g_sdk->renderer->world_to_screen(hero->get_position()), 1, 0xFFFFFFFF);
        }
        
        for (auto&& minion : g_sdk->object_manager->get_minions())
        {
            if (!minion ||
                !utils::is_enemy(minion) || 
                !utils::is_valid_target(minion, 1000.f))
            {
                continue;
            }

            const auto q_damage = calculate_q_damage(minion);
            g_sdk->renderer->add_text(std::to_string(q_damage), 20.f, g_sdk->renderer->world_to_screen(minion->get_position()), 1, 0xFFFFFFFF);
        }

        if (blade_position != math::vector3())
        {
            g_sdk->renderer->add_circle_3d(blade_position, 100.f, 2.f, 0xFFFFFFFF);
        }
    }

    void __fastcall wndproc( uint32_t msg, uint32_t wparam, uint32_t lparam )
    {
        //
    }

    void __fastcall create_object( game_object* object )
    {
        //
    }

    void __fastcall delete_object( game_object* object )
    {
        //
    }

    void __fastcall create_missile( game_object* missile )
    {
        //
    }

    void __fastcall basic_attack( game_object* object, game_object* target, spell_cast* cast )
    {
        //
    }

    void __fastcall stop_cast( game_object* object, spell_cast* cast, bool was_attack_processed )
    {
        //
    }

    void __fastcall process_cast( game_object* object, spell_cast* cast )
    {
        if (utils::is_me(object))
        {
            const auto name = cast->get_spell_data()->get_static_data()->get_name();

            if (name == std::string("IreliaE"))
            {
                blade_position = cast->get_end_pos();
            }

            if (name == std::string("IreliaE2"))
            {
                blade_position = {};
            }
        }
    }

    void __fastcall buff_gain( game_object* object, buff_instance* buff )
    {
        //
    }

    void __fastcall buff_loss( game_object* object, buff_instance* buff )
    {
        if (has_sheen ||
            has_divine_sunderer ||
            has_trinity_force ||
            has_essence_reaver ||
            has_lich_bane ||
            has_iceborn_gauntlet)
        {
            if (sheen_buffs.contains(buff->get_name()))
            {
                last_sheen_t = g_sdk->clock_facade->get_game_time();
            }
        }
    }

    void __fastcall draw_world()
    {
        //
    }

    void __fastcall neutral_minion_kill( game_object* object, game_object* minion, int camp_side_team_id )
    {
        //
    }

    void __fastcall new_path( game_object* object, bool is_dash, float dash_speed )
    {
        //
    }

    void __fastcall execute_cast( game_object* object, spell_cast* cast )
    {
        //
    }

    bool __fastcall issue_order( game_object* object, game_object_order order_type, math::vector3 position, game_object* target, bool move_pet )
    {
        return true;
    }

    bool __fastcall cast_spell( game_object* object, int spell_slot, math::vector3 position, game_object* target )
    {
        return true;
    }

    void __fastcall animation( game_object* object, uint32_t animation_hash )
    {
        //
    }

    void __fastcall cast_heal( game_object* object, game_object* target, float amount )
    {
        //
    }
}
