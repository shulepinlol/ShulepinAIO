#include "debug.h"
#include "sdk.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <fstream>

namespace s_debug
{
    
    
    void __fastcall game_update();
    void __fastcall present();
    void __fastcall buff_gain( game_object* object, buff_instance* buff );
    
    void load_events()
    {
        g_sdk->event_manager->register_callback( event_manager::event::game_update, reinterpret_cast< void* >( game_update ) );
        g_sdk->event_manager->register_callback( event_manager::event::present, reinterpret_cast< void* >( present ) );
        g_sdk->event_manager->register_callback( event_manager::event::buff_gain, reinterpret_cast< void* >( buff_gain ) );
    }

    void unload_events()
    {
        g_sdk->event_manager->unregister_callback( event_manager::event::game_update, reinterpret_cast< void* >( game_update ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::present, reinterpret_cast< void* >( present ) );
        g_sdk->event_manager->unregister_callback( event_manager::event::buff_gain, reinterpret_cast< void* >( buff_gain ) );
    }
    
    void load()
    {
        load_events();
    }

    void unload()
    {
        unload_events();
    }

    void __fastcall buff_gain( game_object* object, buff_instance* buff )
    {
        const auto& time = g_sdk->clock_facade->get_game_time();

        // Construct the path to the desired directory
        char localAppDataPath[MAX_PATH];
        GetEnvironmentVariableA("LOCALAPPDATA", localAppDataPath, MAX_PATH);
        std::filesystem::path dirPath(localAppDataPath);
        dirPath /= "VEN\\League\\Logs\\ven_debug";

        // Create the directory if it doesn't exist
        std::filesystem::create_directories(dirPath);

        // Construct the full path to the log file
        std::filesystem::path filePath = dirPath / "on_buff_gain_log.txt";

        // Open the file in append mode
        std::ofstream file(filePath, std::ios::app);

        if (file.is_open())
        {
            file << "===== [ON_BUFF_GAIN] / " << time << " / ===== \n"
            << "    -> Object: " << object->get_char_name() << "\n"
            << "    -> Buff name: " << buff->get_name() << "\n"
            << "    -> Buff hash: " << buff->get_hash() << "\n"
            << "\n";

            file.close(); // Close the file
        }
    }
    
    void game_update() 
    {
        //
    }
    
    void present()
    {
        //
    }

}
