#include "lee_sin.h"
#include "sdk.hpp"
#include "spell.h"
#include "utils.h"

namespace lee_sin
{
    /* Spells */
    script::spell* q;
    script::spell* w;
    script::spell* e;
    script::spell* r;

    /* Variables & data */
	constexpr uint32_t q2_buff_hash = 1086070810;
	constexpr uint32_t stacks_buff_hash = 3421330838;

	float last_ward_cast_t = 0.f;
	bool should_perform_insec = false;
	bool should_cast_r = false;
	game_object* insec_target;
	
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

    /* Menu items */
    namespace config::combo
    {
        bool use_q;
        bool use_w;
        bool use_e;
        bool use_r;
    }

    namespace config::combo::q
    {
        
    }

    namespace config::combo::q::prediction
    {
        float max_range;
        int hitchance;
    }

    namespace config::combo::q::ignore
    {
        std::unordered_map<std::string, bool> list;
    }

    namespace config::combo::w
    {
        
    }

    namespace config::combo::e
    {
        
    }

	namespace config::combo::e::ignore
    {
    	std::unordered_map<std::string, bool> list;
    }

    namespace config::combo::r
    {
        
    }

	namespace config::combo::r::ignore
    {
    	std::unordered_map<std::string, bool> list;
    }
    
    void register_spells()
    {
        q = new script::spell(0, 1200.f);
        w = new script::spell(1, 700.f);
        e = new script::spell(2, 450.f);
        r = new script::spell(3, 375.f);

        q->set_skillshot(pred_sdk::spell_type::linear, pred_sdk::targetting_type::center_to_edge, 0.25f, 60.f, 1800.f, {pred_sdk::collision_type::unit, pred_sdk::collision_type::yasuo_wall});

        e->set_delay(0.25f);

        r->set_delay(0.25f);
    }
    
    void create_config()
    {
        const auto& player = g_sdk->object_manager->get_local_player();
        const auto config = g_sdk->menu_manager->add_category("shulepin_aio_lee_sin","ShulepinAIO - " + player->get_char_name());
        {
            const auto combo = config->add_sub_category("shulepin_aio_varus_combo", "Combo Settings");
			{
				combo->add_separator();
				combo->add_label("                          |> Spell Manager <|                          ");
				combo->add_separator();
				combo->add_checkbox("shulepin_aio_varus_combo_use_q", "Use [Q]", true, [](const bool value) { config::combo::use_q = value; });
				combo->add_checkbox("shulepin_aio_varus_combo_use_w", "Use [W]", true, [](const bool value) { config::combo::use_w = value; });
				combo->add_checkbox("shulepin_aio_varus_combo_use_e", "Use [E]", true, [](const bool value) { config::combo::use_e = value; });
				combo->add_checkbox("shulepin_aio_varus_combo_use_r", "Use [R]", true, [](const bool value) { config::combo::use_r = value; });
				combo->add_separator();
				combo->add_label("                          |> Spell Settings <|                          ");
				combo->add_separator();
				const auto combo_q = combo->add_sub_category("shulepin_aio_varus_combo_q", "[Q] Settings");
				{
					combo_q->add_separator();
					const auto combo_q_prediction = combo_q->add_sub_category("shulepin_aio_varus_combo_q_prediction", "Prediction Settings");
					{
						combo_q_prediction->add_slider_float("shulepin_aio_varus_combo_q_max_range", "Max range", q->get_range() / 2, q->get_range(), 10.f, q->get_range() - 50.f, [](const float value) {config::combo::q::prediction::max_range = value; });
						combo_q_prediction->add_slider_int("shulepin_aio_varus_combo_q_hit_chance", "Hitchance", 0, 100, 1, 0, [](const int value) { config::combo::q::prediction::hitchance = value; });
					}
					const auto combo_q_black_list = combo_q->add_sub_category("shulepin_aio_varus_combo_q_black_list", "Ignore List");
					{
						for (const auto& hero : g_sdk->object_manager->get_heroes())
						{
							if (!utils::is_enemy(hero)) continue;
							const auto& char_name = hero->get_char_name();
							combo_q_black_list->add_checkbox("shulepin_aio_varus_combo_q_black_list_" + char_name, char_name, false, [char_name](const bool value) { config::combo::q::ignore::list[char_name] = value; });
						}
					}
				}
				const auto combo_w = combo->add_sub_category("shulepin_aio_varus_combo_w", "[W] Settings");
				{
					//
				}
				const auto combo_e = combo->add_sub_category("shulepin_aio_varus_combo_e", "[E] Settings");
				{
					const auto combo_e_black_list = combo_e->add_sub_category("shulepin_aio_varus_combo_e_black_list", "Ignore List");
					{
						for (const auto& hero : g_sdk->object_manager->get_heroes())
						{
							if (!utils::is_enemy(hero)) continue;
							const auto& char_name = hero->get_char_name();
							combo_e_black_list->add_checkbox("shulepin_aio_varus_combo_e_black_list_" + char_name, char_name, false, [char_name](const bool value) { config::combo::e::ignore::list[char_name] = value; });
						}
					}
				}
				const auto combo_r = combo->add_sub_category("shulepin_aio_varus_combo_r", "[R] Settings");
				{
					const auto combo_r_black_list = combo_r->add_sub_category("shulepin_aio_varus_combo_r_black_list", "Ignore List");
					{
						for (const auto& hero : g_sdk->object_manager->get_heroes())
						{
							if (!utils::is_enemy(hero)) continue;
							const auto& char_name = hero->get_char_name();
							combo_r_black_list->add_checkbox("shulepin_aio_varus_combo_r_black_list_" + char_name, char_name, false, [char_name](const bool value) { config::combo::r::ignore::list[char_name] = value; });
						}
					}
				}
			}
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

	int get_ward_slot()
    {
    	const auto& player = g_sdk->object_manager->get_local_player();
		
    	int yellow_trinket_slot = -1;
    	const auto has_yellow_trinket = player->has_item(3340, &yellow_trinket_slot);
    	const auto is_trinket_ready = yellow_trinket_slot > 0 && player->get_spell_cast_state(yellow_trinket_slot + 6) == 0;
    	
    	int control_ward_slot = -1;
    	const auto has_control_ward = player->has_item(2055, &control_ward_slot);
    	
    	if (has_yellow_trinket && is_trinket_ready)
    	{
    		return yellow_trinket_slot + 6;
    	}
    	else if (has_control_ward)
    	{
    		return control_ward_slot + 6;
    	}

    	return -1;
    }
	
	int get_stacks()
    {
    	const auto& player = g_sdk->object_manager->get_local_player();
    	const auto& buff = player->get_buff_by_hash(stacks_buff_hash);
    	
    	if (buff)
    	{
    		return buff->get_stacks();
    	}

    	return 0;
    }
	
	bool is_second_state(const int slot)
    {
    	const auto& player = g_sdk->object_manager->get_local_player();
    	const auto spell_name = player->get_spell(slot)->get_data()->get_static_data()->get_name();
    	const auto suffix = "Two";
    	
    	const size_t t_len = strlen(spell_name);
    	const size_t suffix_len = strlen(suffix);

    	return t_len >= suffix_len && strcmp(spell_name + t_len - suffix_len, suffix) == 0;
    }

	bool cast_ward_slot(const math::vector3 cast_position)
    {
    	const auto& player = g_sdk->object_manager->get_local_player();
    	const auto& time = g_sdk->clock_facade->get_game_time();
    	const auto ward_slot = get_ward_slot();
    		
    	if (ward_slot > 0 &&
    		w->is_ready() &&
    		!is_second_state(1) &&
    		last_ward_cast_t + 0.5f < time &&
    		cast_position.distance(player->get_server_position()) < 600.f)
    	{
    		player->cast_spell(ward_slot, cast_position);
    		last_ward_cast_t = time;
    		return true;
    	}

    	return false;
    }

	math::vector3 get_insec_position(game_object* target)
    {
    	const auto& player = g_sdk->object_manager->get_local_player();

    	return target->get_position().extended(player->get_server_position(), -r->get_range() / 2.f, true);
    }
	
	/* Logic */

	void q_combat_logic()
    {
		const auto& player = g_sdk->object_manager->get_local_player();
    	
	    if (!q->is_ready() ||
	    	sdk::orbwalker->would_cancel_attack())
	    {
		    return;
	    }

    	if (sdk::orbwalker->combo())
    	{
    		if (!config::combo::use_q)
    		{
    			return;
    		}

    		const auto target = utils::get_target_with_list(1250.f, config::combo::q::ignore::list);
    		const auto& stacks = get_stacks();

    		if (!target)
    		{
    			return;
    		}
    		
    		if (is_second_state(0))
    		{
    			if (target->get_buff_by_hash(q2_buff_hash) &&
					utils::is_position_inside_turret_range(target->get_position()) ||
					!q->is_issue_order_passed(2.8f))
    			{
    				return;
    			}

    			q->cast_spell();
    		}
    		else
    		{
				if (sdk::orbwalker->is_in_auto_attack_range(player, target) && stacks > 0)
				{
					return;
				}
    			
    			const auto pred = q->get_prediction(target, config::combo::q::prediction::hitchance, config::combo::q::prediction::max_range);
    			if (pred.is_valid)
    			{
    				q->cast_spell(pred.cast_position);
    			}
    		}
    	}
    }

	void w_combat_logic()
    {
    	const auto& player = g_sdk->object_manager->get_local_player();
    	
    	if (!w->is_ready() ||
			sdk::orbwalker->would_cancel_attack())
    	{
    		return;
    	}

    	if (sdk::orbwalker->combo())
    	{
    		if (!config::combo::use_w)
    		{
    			return;
    		}

    		const auto& target = utils::get_target(w->get_range());
    		const auto& stacks = get_stacks();

    		if (!target)
    		{
    			return;
    		}

    		if (is_second_state(1))
    		{
    			if (stacks > 0)
    			{
    				return;
    			}

    			w->cast_spell(player);
    		}
    		else
    		{
    			if (stacks > 0 || q->is_ready() || e->is_ready())
    			{
    				return;
    			}
    			
    			w->cast_spell(player);
    		}
    	}
    }

	void e_combat_logic()
    {
    	const auto& player = g_sdk->object_manager->get_local_player();
    	
    	if (!e->is_ready() ||
			sdk::orbwalker->would_cancel_attack())
    	{
    		return;
    	}

    	if (sdk::orbwalker->combo())
    	{
    		if (!config::combo::use_e)
    		{
    			return;
    		}

    		const auto& target = utils::get_target_with_list(e->get_range(), config::combo::e::ignore::list);
    		const auto& stacks = get_stacks();

    		if (!target)
    		{
    			return;
    		}

    		if (is_second_state(2))
    		{
    			if (stacks > 0)
    			{
    				return;
    			}

    			e->cast_spell();
    		}
    		else
    		{
				if (stacks > 0 ||
					!sdk::orbwalker->is_in_auto_attack_range(player, target))
				{
					return;
				}
    			
    			e->cast_spell();
    		}
    	}
    }

	void r_combat_logic()
    {
	    
    }
	
	void combat_logic()
    {
	    q_combat_logic();
    	w_combat_logic();
    	e_combat_logic();
    	r_combat_logic();
    }
	
    /* Events */
    
    void __fastcall game_update()
    {
        combat_logic();
    }
	
    void __fastcall present()
    {
		const auto& player = g_sdk->object_manager->get_local_player();
    	const auto& time = g_sdk->clock_facade->get_game_time();

    	should_perform_insec = false;
    	
    	if (sdk::orbwalker->flee())
    	{
			if (!r->is_ready())
			{
				return;
			}
    		
    		const auto insec_target = utils::get_target(1500.f);

    		if (!insec_target)
    		{
    			return;
    		}

    		const auto insec_position = get_insec_position(insec_target);
    		
    		// Q
    		if (q->is_ready() &&
    			player->get_server_position().distance(insec_target->get_position()) > 600.f ||
    			insec_target->is_dashing())
    		{
    			if (is_second_state(0))
    			{
    				q->cast_spell();
    			}
    			else
    			{
    				const auto pred = q->get_prediction(insec_target, config::combo::q::prediction::hitchance, config::combo::q::prediction::max_range);
    				if (pred.is_valid)
    				{
    					q->cast_spell(pred.cast_position);
    				}
				    else
				    {
				    	// Assuming you have a function to get all minions
				    	const auto minions = g_sdk->object_manager->get_minions();

				    	// Initialize variables
				    	game_object* closest_minion = nullptr;
				    	float closest_minion_distance = std::numeric_limits<float>::max();

				    	// Your hero position
				    	math::vector3 my_position = player->get_server_position();

				    	// Iterate through each minion
				    	for (auto& minion : minions) {
							if (!minion ||
								!utils::is_enemy(minion) ||
								!utils::is_valid_target(minion))
							{
								continue;
							}
				    		
				    		float distance_to_my_hero = my_position.distance(minion->get_position());
				    		float distance_to_insec_target = minion->get_position().distance(insec_target->get_position());
				    		
				    		if (distance_to_my_hero < closest_minion_distance && distance_to_insec_target < 600.f)
				    		{
				    			closest_minion = minion;
				    			closest_minion_distance = distance_to_my_hero;
				    		}
				    	}

				    	if (closest_minion)
				    	{
				    		q->cast_spell(closest_minion->get_position());
				    	}
				    }
    			}
    		}

    		// Ward
    		cast_ward_slot(insec_position);

            // R
    		if (r->is_ready() &&
    			r->is_in_range(insec_target) &&
    			w->is_issue_order_passed(0.15f) &&
    			should_cast_r)
    		{
    			r->cast_spell(insec_target, 0.1f);
    		}
    		
    		should_perform_insec = true;
    	}
    }

    void __fastcall wndproc( uint32_t msg, uint32_t wparam, uint32_t lparam )
    {
        //
    }

    void __fastcall create_object( game_object* object )
    {
    	if (object && object->get_owner())
    	{
    		if (object->is_ward() &&
    			utils::is_me(object->get_owner()) &&
    			w->is_ready() &&
    			!is_second_state(1) &&
    			should_perform_insec)
    		{
    			w->cast_spell(object);
    			should_cast_r = true;
    		}
    	}
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
        //
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
        	const auto name = cast->get_spell_data()->get_static_data()->get_name();

        	if (name == std::string("BlindMonkRKick"))
        	{
        		should_cast_r = false;
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
        //
    }

    void __fastcall cast_heal( game_object* object, game_object* target, float amount )
    {
        //
    }
}
