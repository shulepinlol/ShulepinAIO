#include "template.h"

#include "hashes.h"
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

    /* Variables & data */
    
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
        q = new script::spell(0, 1595.f);
        w = new script::spell(1, 0.f);
        e = new script::spell(2, 0.f);
        r = new script::spell(3, 0.f);

        q->set_skillshot(pred_sdk::spell_type::linear, pred_sdk::targetting_type::edge_to_edge, 0.f, 70.f, FLT_MAX, {});
        q->set_charged_spell(700.f, 1450.f, 1.5f, BUFF_HASH("XerathArcanopulseChargeUp"));
    	q->set_proc_delay(0.5f);
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

    /* Events */
    
    void __fastcall game_update()
    {
        //
    }
    

    void __fastcall present()
    {
        //
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
