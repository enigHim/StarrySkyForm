#pragma once

#include <filesystem>
#include <string>

#include "ll/api/Config.h"

namespace starry_sky_form {

struct Config {
    int         version = 1;
    std::string openCommandName   = "openform";
    std::string reloadCommandName = "reloadform";
    std::string giveCommandName   = "giveform";
    std::string clearCommandName  = "clearform";
    std::string clockName         = "\xc2\xa7" "b菜单:";
};

Config& getConfig();
std::filesystem::path getConfigDir();
std::filesystem::path getFormsDir();

void initConfig();
void loadConfig();
void saveConfig();

void ensureDefaultForms();

} // namespace starry_sky_form
