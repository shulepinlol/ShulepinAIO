#include "dev_tool.h"
#include "sdk.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <fstream>

#include "hashes.h"
#include "utils.h"

namespace dev_tool
{
    void __fastcall game_update();
    void __fastcall present();
    void __fastcall buff_gain(game_object* object, buff_instance* buff);

    namespace config::log
    {
	    bool on_buff_gain;
    }

    void create_config()
    {
        const auto config = g_sdk->menu_manager->add_category("shulepin_aio_dev_tool", "ShulepinAIO - Dev Tool");
        {
            const auto log = config->add_sub_category("shulepin_aio_dev_tool_log", "Log");
            {
                log->add_checkbox("shulepin_aio_dev_tool_log_buff_gain", "Log [on_buff_gain]", true, [](const bool value) { config::log::on_buff_gain = value; });
            }
        }
    }

    void load_events()
    {
        g_sdk->event_manager->register_callback(event_manager::event::game_update, reinterpret_cast<void*>(game_update));
        g_sdk->event_manager->register_callback(event_manager::event::present, reinterpret_cast<void*>(present));
        g_sdk->event_manager->register_callback(event_manager::event::buff_gain, reinterpret_cast<void*>(buff_gain));
    }

    void unload_events()
    {
        g_sdk->event_manager->unregister_callback(event_manager::event::game_update, reinterpret_cast<void*>(game_update));
        g_sdk->event_manager->unregister_callback(event_manager::event::present, reinterpret_cast<void*>(present));
        g_sdk->event_manager->unregister_callback(event_manager::event::buff_gain, reinterpret_cast<void*>(buff_gain));
    }

    void load()
    {
        create_config();
        load_events();
    }

    void unload()
    {
        unload_events();

        char local_app_data_path[MAX_PATH];
        GetEnvironmentVariableA("LOCALAPPDATA", local_app_data_path, MAX_PATH);
        std::filesystem::path dir_path(local_app_data_path);
        dir_path /= R"(VEN\League\Logs\ven_debug)";
        const std::filesystem::path file_path = dir_path / "on_buff_gain_log.txt";

        if (std::filesystem::exists(file_path))
        {
            std::filesystem::remove(file_path);
        }
    }

    std::string buff_type_to_string(buff_type type)
    {
        std::ostringstream os;
        switch (type)
        {
	        case buff_type::internal: os << "internal"; break;
	        case buff_type::aura: os << "aura"; break;
	        case buff_type::combat_enchancer: os << "combat_enchancer"; break;
	        case buff_type::combat_dehancer: os << "combat_dehancer"; break;
	        case buff_type::spell_shield: os << "spell_shield"; break;
	        case buff_type::stun: os << "stun"; break;
	        case buff_type::invisibility: os << "invisibility"; break;
	        case buff_type::silence: os << "silence"; break;
	        case buff_type::taunt: os << "taunt"; break;
	        case buff_type::berserk: os << "berserk"; break;
	        case buff_type::polymorph: os << "polymorph"; break;
	        case buff_type::slow: os << "slow"; break;
	        case buff_type::snare: os << "snare"; break;
	        case buff_type::damage: os << "damage"; break;
	        case buff_type::heal: os << "heal"; break;
	        case buff_type::haste: os << "haste"; break;
	        case buff_type::spell_immunity: os << "spell_immunity"; break;
	        case buff_type::physical_immunity: os << "physical_immunity"; break;
	        case buff_type::invulnerability: os << "invulnerability"; break;
	        case buff_type::attack_speed_slow: os << "attack_speed_slow"; break;
	        case buff_type::near_sight: os << "near_sight"; break;
	        case buff_type::currency: os << "currency"; break;
	        case buff_type::fear: os << "fear"; break;
	        case buff_type::charm: os << "charm"; break;
	        case buff_type::poison: os << "poison"; break;
	        case buff_type::suppression: os << "suppression"; break;
	        case buff_type::blind: os << "blind"; break;
	        case buff_type::counter: os << "counter"; break;
	        case buff_type::shred: os << "shred"; break;
	        case buff_type::flee: os << "flee"; break;
	        case buff_type::knockup: os << "knockup"; break;
	        case buff_type::knockback: os << "knockback"; break;
	        case buff_type::disarm: os << "disarm"; break;
	        case buff_type::grounded: os << "grounded"; break;
	        case buff_type::drowsy: os << "drowsy"; break;
	        case buff_type::asleep: os << "asleep"; break;
	        case buff_type::obscured: os << "obscured"; break;
	        case buff_type::clickproof_to_enemies: os << "clickproof_to_enemies"; break;
	        case buff_type::unkillable: os << "unkillable"; break;
	        default: os << "unknown";
        }
        os << " [" << static_cast<int>(type) << "]";
        return os.str();
    }

    void __fastcall buff_gain(game_object* object, buff_instance* buff)
    {
        if (!config::log::on_buff_gain)
        {
            return;
        }

        const auto& time = g_sdk->clock_facade->get_game_time();

        std::ostringstream log_stream;
        log_stream << "===== [ON_BUFF_GAIN] / " << time << " / ===== \n"
            << "    -> Object: " << object->get_char_name() << "\n"
            << "    -> Buff name: " << buff->get_name() << "\n"
            << "    -> Buff hash: " << buff->get_hash() << "\n"
			<< "    -> Buff caster: " << buff->get_caster()->get_char_name() << "\n"
			<< "    -> Buff type: " << buff_type_to_string(buff->get_type()) << "\n"
            << "\n";

        LOG(log_stream.str().c_str());

        char local_app_data_path[MAX_PATH];
        GetEnvironmentVariableA("LOCALAPPDATA", local_app_data_path, MAX_PATH);
        std::filesystem::path dir_path(local_app_data_path);
        dir_path /= R"(VEN\League\Logs\ven_debug)";

        create_directories(dir_path);

        const std::filesystem::path file_path = dir_path / "on_buff_gain_log.txt";

        std::ofstream file(file_path, std::ios::app);

        if (file.is_open())
        {
            file << log_stream.str();
            file.close();
        }
    }

    void game_update()
    {
        
    }

    void present()
    {
        //
    }

}
