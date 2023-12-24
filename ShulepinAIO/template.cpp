#include "template.h"

#include "hashes.h"
#include "polygon.h"
#include "sdk.hpp"
#include "spell.h"
#include "utils.h"

namespace template_champion
{
    /* Spells */
    script::spell* q;
    script::spell* w;
    script::spell* e;
    script::spell* r;

    script::spell* q1;
    script::spell* q2;
    script::spell* q3;

    script::spell* w3;

    script::spell* e1;
    script::spell* e3;

    /* Variables & data */
    constexpr uint32_t q_mood_hash = 171220955;
	constexpr uint32_t w_mood_hash = 323565974; 
	constexpr uint32_t e_mood_hash = 2991143953;

    int current_mood = -1;

    enum hwei_spell
	{
        None,
        HweiQQ,
        HweiQW,
        HweiQE,
        HweiWQ,
        HweiWW,
        HweiEQ,
        HweiEW,
        HweiEE,
        HweiR
    };

    hwei_spell last_casted_spell{};
    float last_casted_spell_t{};
    
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
        q = new script::spell(0, 0.f);
        w = new script::spell(1, 0.f);
        e = new script::spell(2, 0.f);
        r = new script::spell(3, 0.f);

        q1 = new script::spell(0, 800.f);
        q1->set_skillshot(pred_sdk::spell_type::linear, pred_sdk::targetting_type::edge_to_edge, 0.25f, 70.f, 2000.f, { pred_sdk::collision_type::unit, pred_sdk::collision_type::yasuo_wall, pred_sdk::collision_type::braum_wall });

        q2 = new script::spell(1, 1900.f);
        q2->set_skillshot(pred_sdk::spell_type::circular, pred_sdk::targetting_type::center_to_edge, 0.5f, 200.f, FLT_MAX, {});
        q2->set_proc_delay(1.f);

        q3 = new script::spell(2, 900.f); // proc_delay 0.7
        q3->set_skillshot(pred_sdk::spell_type::linear, pred_sdk::targetting_type::center_to_edge, 0.35f, 140.f, 1000.f, {});

        w3 = new script::spell(2, 0.f);

        e1 = new script::spell(0, 1000.f);
        e1->set_skillshot(pred_sdk::spell_type::linear, pred_sdk::targetting_type::center_to_edge, 0.25f, 70.f, 1300.f, { pred_sdk::collision_type::unit, pred_sdk::collision_type::yasuo_wall, pred_sdk::collision_type::braum_wall });

        e3 = new script::spell(3, 800.f);
        e3->set_skillshot(pred_sdk::spell_type::linear, pred_sdk::targetting_type::center_to_edge, 0.35f, 300.f, FLT_MAX, {});
        e3->set_proc_delay(0.627f);
    }
    
    void create_config()
    {
        const auto& player = g_sdk->object_manager->get_local_player();
        const auto config = g_sdk->menu_manager->add_category("shulepin_aio_template_champion","ShulepinAIO - Template");
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

    /* Common */

    hwei_spell string_to_spell(const std::string& name)
	{
        if (name == "HweiQQ") return HweiQQ;
        if (name == "HweiQW") return HweiQW;
        if (name == "HweiQE") return HweiQE;
        if (name == "HweiWQ") return HweiWQ;
        if (name == "HweiWW") return HweiWW;
        if (name == "HweiEQ") return HweiEQ;
        if (name == "HweiEW") return HweiEW;
        if (name == "HweiEE") return HweiEE;
        if (name == "HweiR") return HweiR;
    }

    void set_last_casted_spell(const std::string& name)
	{
        const auto& time = g_sdk->clock_facade->get_game_time();

        last_casted_spell = string_to_spell(name);
        last_casted_spell_t = time;
    }

    hwei_spell get_last_casted_spell(const float t = 5.f)
    {
        const auto& time = g_sdk->clock_facade->get_game_time();

        if (last_casted_spell_t + t > time)
        {
            return last_casted_spell;
        }

        return None;
    }

    int get_current_mood()
    {
        return q->get_toggle_state() > 0 ? current_mood : -1;
    }

    bool can_cast_mood(const float t = 0.15f)
    {
        return q->is_mood_order_passed(t) && w->is_mood_order_passed(t) && e->is_mood_order_passed(t);
    }

    bool cast_qq(const math::vector3 cast_position)
    {
	    if (!q->is_ready() ||
            !can_cast_mood())
	    {
            return false;
	    }

        if (q->cast_mood())
        {
            return q->cast_spell(cast_position);
        }

        return false;
    }

    bool cast_qw(const math::vector3 cast_position)
    {
        if (!q->is_ready() ||
            !can_cast_mood())
        {
            return false;
        }

        if (q->cast_mood())
        {
            return w->cast_spell(cast_position);
        }

        return false;
    }

    bool cast_qe(const math::vector3 cast_position)
    {
        if (!q->is_ready() ||
            !can_cast_mood())
        {
            return false;
        }

        if (q->cast_mood())
        {
            return e->cast_spell(cast_position);
        }

        return false;
    }

    bool cast_we(const float t = 0.15f)
    {
        if (!w->is_ready() ||
            !can_cast_mood(t))
        {
            return false;
        }

        if (w->cast_mood())
        {
            return e->cast_spell();
        }

        return false;
    }

    bool cast_eq(const math::vector3 cast_position)
    {
        if (!e->is_ready() ||
            !can_cast_mood())
        {
            return false;
        }

        if (e->cast_mood())
        {
            return q->cast_spell(cast_position);
        }

        return false;
    }

    bool cast_ee(const math::vector3 cast_position)
    {
        if (!e->is_ready() ||
            !can_cast_mood())
        {
            return false;
        }

        if (e->cast_mood())
        {
            return e->cast_spell(cast_position);
        }

        return false;
    }

    /* Logic */

    void combo_logic()
    {
        const auto& player = g_sdk->object_manager->get_local_player();
        const auto& time = g_sdk->clock_facade->get_game_time();
        const auto& cursor = g_sdk->hud_manager->get_cursor_position();
        const auto& last_spell = get_last_casted_spell();

        const auto target = sdk::target_selector->get_hero_target([](game_object* hero)
            {
                return utils::is_valid_target(hero, q2->get_range());
            });

        if (!target)
        {
	        return;
        }

        const auto q1_pred = q1->get_prediction(target, 0);
        const auto q2_pred = q2->get_prediction(target, 0);
        const auto q3_pred = q3->get_prediction(target, 0);

        const auto e1_pred = e1->get_prediction(target, 0);
        const auto e3_pred = e3->get_prediction(target, 0);

        bool should_cast_q1 = false;
        {
	        if (!q1_pred.is_valid || !q1->is_in_range(q1_pred.cast_position))
	        {
                should_cast_q1 = false;
	        }
            else
            {
                should_cast_q1 = true;
            }
        }

        bool should_cast_q2 = false;
        {
            if (!q2_pred.is_valid || !q1->is_in_range(q2_pred.cast_position))
            {
                should_cast_q2 = false;
            }
            else
            {
                if (utils::get_percent_health(target) <= 50.f)
                {
                    if (utils::is_on_cc(target, q2->get_proc_delay(), 0.15f))
                    {
                        should_cast_q2 = true;
                    }
                }
            }
        }

        bool should_cast_q3 = false;
        {
            if (!q3_pred.is_valid || !q3->is_in_range(q3_pred.cast_position))
            {
                should_cast_q3 = false;
            }
            else
            {
                if (e->is_ready() && e3_pred.is_valid && e3->is_in_range(e3_pred.cast_position))
                {
                    should_cast_q3 = true;
                }
            }
        }

        bool should_cast_e1 = false;
        {
            if (!e1_pred.is_valid || !e3->is_in_range(e1_pred.cast_position))
            {
                should_cast_e1 = false;
            }
            else
            {
                if (q->is_ready(1.f))
                {
                    should_cast_e1 = true;
                }
            }
        }

        bool should_cast_e3 = false;
        {
            if (!e3_pred.is_valid || !e3->is_in_range(e3_pred.cast_position))
            {
                should_cast_e3 = false;
            }
            else
            {
                if (last_spell == HweiQE)
                {
                    should_cast_e3 = true;
                }
            }
        }

        if (!sdk::orbwalker->combo())
        {
            return;
        }

        /* === Q === */
        if (q->is_ready() && q->can_cast(target) && can_cast_mood())
        {
            if (should_cast_q2)
            {
                cast_qw(q2_pred.cast_position);
            }

            if (should_cast_q1)
            {
                cast_qq(q1_pred.cast_position);
            }

            if (should_cast_q3)
            {
                cast_qe(q3_pred.cast_position);
            }
        }

        /* === E === */
        if (e->is_ready() && e->can_cast(target) && can_cast_mood())
        {
            if (should_cast_e3)
            {
                cast_ee(e3_pred.cast_position);
            }

            if (should_cast_e1)
            {
                cast_eq(e1_pred.cast_position);
            }
        }
    }

    /* Events */

    void __fastcall game_update()
    {
        combo_logic();
    }
    

    void __fastcall present()
    {
        const auto& start_pos = script::vector2(1000.f, 2000.f);
        const auto& end_pos = script::vector2(2000.f, 4000.f);
        constexpr auto width = 150.f;

        script::rectangle rect(start_pos, end_pos, width);
        rect.update_polygon();
        rect.draw(0xFFFFFFFF, 4.f);

        LOG(rect.is_inside(script::vector2(g_sdk->hud_manager->get_cursor_position().to_2d())));
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
            const auto& name = cast->get_spell_data()->get_static_data()->get_name();

            if (name == std::string("HweiQQ") || 
                name == std::string("HweiQW") ||
                name == std::string("HweiQE") ||
                name == std::string("HweiWQ") ||
                name == std::string("HweiWW") ||
                name == std::string("HweiEQ") ||
                name == std::string("HweiEW") ||
                name == std::string("HweiEE") ||
                name == std::string("HweiR"))
            {
                set_last_casted_spell(name);
            }
        }
    }

    void __fastcall buff_gain( game_object* object, buff_instance* buff )
    {
        //
    }

    void __fastcall buff_loss( game_object* object, buff_instance* buff )
    {
        //
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
        if (utils::is_me(object))
        {
            const auto& name = cast->get_spell_data()->get_static_data()->get_name();

	        if (sdk::orbwalker->combo())
            {
		        if (w->is_ready())
                {
			        if (name == std::string("HweiQQ") || 
                        name == std::string("HweiQW") ||
                        name == std::string("HweiQE"))
			        {
                        cast_we();
			        }
		        }
	        }
        }
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
        if (utils::is_me(object))
        {
            switch (animation_hash)
            {
            case q_mood_hash:
                current_mood = 0;
                break;
            case w_mood_hash:
                current_mood = 1;
                break;
            case e_mood_hash:
                current_mood = 2;
                break;
            default:
                break;
            }
        }
    }

    void __fastcall cast_heal( game_object* object, game_object* target, float amount )
    {
        //
    }
}
