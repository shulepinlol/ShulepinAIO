#include <unordered_map>
#include <optional>
#include <string>

#include "sdk.hpp"

#include "arena_helper.h"
#include "dev_tool.h"

#include "irelia.h"
#include "lucian.h"
#include "varus.h"
#include "hwei.h"
#include "lee_sin.h"
#include "template.h"

extern "C" __declspec(dllexport) int SDKVersion = SDK_VERSION;

using champion_functions = std::pair<std::optional<std::function<void()>>, std::optional<std::function<void()>>>;

std::unordered_map<std::string, champion_functions> champion_actions = {
    {"Lucian", {{[]() { lucian::load(); }}, {[]() { lucian::unload(); }}}},
    {"Varus", {{[]() { varus::load(); }}, {[]() { varus::unload(); }}}},
};

extern "C" __declspec(dllexport) bool PluginLoad(core_sdk* sdk, void** custom_sdk)
{
    g_sdk = sdk;
    
    if (!sdk_init::orbwalker())
    {
        return false;
    }
    if (!sdk_init::target_selector())
    {
        return false;
    }
    if (!sdk_init::prediction())
    {
        return false;
    }
    if (!sdk_init::infotab())
    {
        return false;
    }
    if (!sdk_init::damage())
    {
        return false;
    }
    if (!sdk_init::health_prediction())
    {
        return false;
    }
    
    const auto char_name = g_sdk->object_manager->get_local_player()->get_char_name();

    const auto action_it = champion_actions.find(char_name);
    if (action_it != champion_actions.end() && action_it->second.first)
    {
        (*action_it->second.first)();
    }
    
    if (std::strcmp(g_sdk->game_info->get_game_mode(), "CHERRY") == 0)
    {
        arena_helper::load();
    }

    if (g_sdk->get_username() == std::string("shxlxpxn"))
    {
        dev_tool::load();
        template_champion::load();
    }
    
    return true;
}

extern "C" __declspec(dllexport) void PluginUnload()
{
    const auto char_name = g_sdk->object_manager->get_local_player()->get_char_name();

    const auto action_it = champion_actions.find(char_name);
    if (action_it != champion_actions.end() && action_it->second.second)
    {
        (*action_it->second.second)();
    }

    if (g_sdk->get_username() == std::string("shxlxpxn"))
    {
        dev_tool::unload();
        template_champion::unload();
    }
    
    arena_helper::unload();
}
