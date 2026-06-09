#include "mod/Config.h"

#include "ll/api/mod/NativeMod.h"
#include "nlohmann/json.hpp"

#include <fstream>

namespace starry_sky_form {

static Config g_config;

Config& getConfig() {
    return g_config;
}

std::filesystem::path getConfigDir() {
    return ll::mod::NativeMod::current()->getConfigDir();
}

std::filesystem::path getFormsDir() {
    return getConfigDir() / "forms";
}

void initConfig() {
    auto dir = getConfigDir();
    std::filesystem::create_directories(dir);

    auto configPath = dir / "config.json";
    if (!std::filesystem::exists(configPath)) {
        ll::config::saveConfig(g_config, configPath);
    } else {
        ll::config::loadConfig(g_config, configPath);
    }

    std::filesystem::create_directories(getFormsDir());
    ensureDefaultForms();
}

void loadConfig() {
    auto configPath = getConfigDir() / "config.json";
    ll::config::loadConfig(g_config, configPath);
}

void saveConfig() {
    auto configPath = getConfigDir() / "config.json";
    ll::config::saveConfig(g_config, configPath);
}

static void writeDefaultForm(const std::string& filename, const std::string& content) {
    auto path = getFormsDir() / filename;
    if (!std::filesystem::exists(path)) {
        std::ofstream ofs(path);
        ofs << content;
    }
}

void ensureDefaultForms() {
    writeDefaultForm(
        "main.json",
        R"({
  "title": "§b主菜单",
  "content": "选择你需要的功能~",
  "buttons": {
    "子菜单&textures/items/ender_pearl.png": ["openform sub_main"],
    "给我一个菜单钟&textures/ui/dressing_room_skins.png": ["giveform"],
    "删除菜单钟&textures/items/emerald.png": ["clearform"],
    "测试&textures/items/emerald.png": ["console:list", "console:title @s title 测试", "console:give @s apple"]
  }
})"
    );

    writeDefaultForm(
        "sub_main.json",
        R"({
  "title": "§b菜单",
  "content": "选择你需要的功能~",
  "buttons": {
    "公共传送&textures/items/ender_pearl.png": ["warp 1"],
    "领地&textures/ui/dressing_room_skins.png": ["pland"],
    "返回&textures/items/emerald.png": ["openform main"]
  }
})"
    );
}

} // namespace starry_sky_form
