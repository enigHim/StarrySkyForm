#include "ConfigManager.h"

#include "starryskyform/StarrySkyForm.h"
#include "ll/api/mod/NativeMod.h"
#include "ll/api/io/Logger.h"

#include <fstream>
#include <filesystem>

namespace starrysky::config {

using json = nlohmann::json;

ConfigManager::ConfigManager() {
    auto& mod = StarrySkyForm::getInstance().getSelf();
    m_configDir = mod.getConfigDir().string();
}

ConfigManager::~ConfigManager() = default;

bool ConfigManager::load() {
    auto& logger = StarrySkyForm::getInstance().getSelf().getLogger();
    
    // Create config directory if not exists
    std::filesystem::path configPath(m_configDir);
    if (!std::filesystem::exists(configPath)) {
        std::filesystem::create_directories(configPath);
    }
    
    // Create forms directory
    std::filesystem::path formsPath = configPath / "forms";
    if (!std::filesystem::exists(formsPath)) {
        std::filesystem::create_directories(formsPath);
    }
    
    // Create default configs if not exists
    if (!createDefaultConfigs()) {
        logger.warn("Failed to create default configs");
    }
    
    // Load main config
    if (!loadMainConfig()) {
        logger.error("Failed to load main config");
        return false;
    }
    
    // Load form configs
    if (!loadFormConfigs()) {
        logger.error("Failed to load form configs");
        return false;
    }
    
    logger.info("Configuration loaded successfully");
    return true;
}

bool ConfigManager::reload() {
    m_formConfigs.clear();
    return load();
}

bool ConfigManager::save() {
    // Save main config
    json j;
    j["commandOpenForm"] = m_mainConfig.commandOpenForm;
    j["commandGiveForm"] = m_mainConfig.commandGiveForm;
    j["commandSetForm"] = m_mainConfig.commandSetForm;
    
    j["item"]["id"] = m_mainConfig.itemConfig.id;
    j["item"]["damage"] = m_mainConfig.itemConfig.damage;
    j["item"]["count"] = m_mainConfig.itemConfig.count;
    j["item"]["slot"] = m_mainConfig.itemConfig.slot;
    j["item"]["name"] = m_mainConfig.itemConfig.name;
    j["item"]["enchanted"] = m_mainConfig.itemConfig.enchanted;
    j["item"]["lockedByDefault"] = m_mainConfig.itemConfig.lockedByDefault;
    
    std::ofstream file(getConfigPath() + "/config.json");
    if (!file.is_open()) {
        return false;
    }
    file << j.dump(4);
    return true;
}

bool ConfigManager::loadMainConfig() {
    auto& logger = StarrySkyForm::getInstance().getSelf().getLogger();
    std::string configFile = getConfigPath() + "/config.json";
    
    if (!std::filesystem::exists(configFile)) {
        logger.warn("Config file not found, using defaults");
        return true;
    }
    
    std::ifstream file(configFile);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        if (j.contains("commandOpenForm")) {
            m_mainConfig.commandOpenForm = j["commandOpenForm"].get<std::string>();
        }
        if (j.contains("commandGiveForm")) {
            m_mainConfig.commandGiveForm = j["commandGiveForm"].get<std::string>();
        }
        if (j.contains("commandSetForm")) {
            m_mainConfig.commandSetForm = j["commandSetForm"].get<std::string>();
        }
        
        if (j.contains("item")) {
            auto& item = j["item"];
            if (item.contains("id")) m_mainConfig.itemConfig.id = item["id"].get<int>();
            if (item.contains("damage")) m_mainConfig.itemConfig.damage = item["damage"].get<int>();
            if (item.contains("count")) m_mainConfig.itemConfig.count = item["count"].get<int>();
            if (item.contains("slot")) m_mainConfig.itemConfig.slot = item["slot"].get<int>();
            if (item.contains("name")) m_mainConfig.itemConfig.name = item["name"].get<std::string>();
            if (item.contains("enchanted")) m_mainConfig.itemConfig.enchanted = item["enchanted"].get<bool>();
            if (item.contains("lockedByDefault")) m_mainConfig.itemConfig.lockedByDefault = item["lockedByDefault"].get<bool>();
        }
        
        logger.info("Main config loaded");
        return true;
    } catch (const std::exception& e) {
        logger.error("Error parsing config: {}", e.what());
        return false;
    }
}

bool ConfigManager::loadFormConfigs() {
    auto& logger = StarrySkyForm::getInstance().getSelf().getLogger();
    std::string formsDir = getFormsPath();
    
    if (!std::filesystem::exists(formsDir)) {
        return true;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(formsDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            std::string formName = entry.path().stem().string();
            std::ifstream file(entry.path());
            
            if (!file.is_open()) {
                logger.warn("Failed to open form config: {}", formName);
                continue;
            }
            
            try {
                json j;
                file >> j;
                
                FormConfig config;
                config.title = j.value("title", "Form");
                config.content = j.value("content", "");
                
                if (j.contains("buttons") && j["buttons"].is_object()) {
                    for (auto& [key, value] : j["buttons"].items()) {
                        FormButton button;
                        
                        // Parse button text and optional image
                        size_t pos = key.find('&');
                        if (pos != std::string::npos) {
                            button.text = key.substr(0, pos);
                            button.image = key.substr(pos + 1);
                        } else {
                            button.text = key;
                        }
                        
                        // Parse commands
                        if (value.is_array()) {
                            for (const auto& cmd : value) {
                                if (cmd.is_string()) {
                                    button.commands.push_back(cmd.get<std::string>());
                                }
                            }
                        } else if (value.is_string()) {
                            button.commands.push_back(value.get<std::string>());
                        }
                        
                        config.buttons.push_back(button);
                    }
                }
                
                m_formConfigs[formName] = config;
                logger.info("Loaded form config: {}", formName);
            } catch (const std::exception& e) {
                logger.error("Error parsing form config {}: {}", formName, e.what());
            }
        }
    }
    
    return true;
}

bool ConfigManager::createDefaultConfigs() {
    auto& logger = StarrySkyForm::getInstance().getSelf().getLogger();
    bool created = false;
    
    // Create default config.json
    std::string configFile = getConfigPath() + "/config.json";
    if (!std::filesystem::exists(configFile)) {
        json j;
        j["commandOpenForm"] = "openform";
        j["commandGiveForm"] = "giveform";
        j["commandSetForm"] = "setform";
        
        j["item"]["id"] = 720;
        j["item"]["damage"] = 0;
        j["item"]["count"] = 1;
        j["item"]["slot"] = 8;
        j["item"]["name"] = "§b星空§b菜§b§b单";
        j["item"]["enchanted"] = true;
        j["item"]["lockedByDefault"] = true;
        
        std::ofstream file(configFile);
        if (file.is_open()) {
            file << j.dump(4);
            created = true;
            logger.info("Created default config.json");
        }
    }
    
    // Create default main.json form
    std::string mainFormFile = getFormsPath() + "/main.json";
    if (!std::filesystem::exists(mainFormFile)) {
        json j;
        j["title"] = "§b星空菜单";
        j["content"] = "选择你需要的功能~";
        j["buttons"]["传送&textures/items/ender_pearl.png"] = json::array({"openform 传送"});
        j["buttons"]["公会&textures/ui/dressing_room_skins.png"] = json::array({"guild"});
        j["buttons"]["经济&textures/items/emerald.png"] = json::array({"openform 经济"});
        j["buttons"]["领地&textures/ui/icon_recipe_nature.png"] = json::array({"territory"});
        j["buttons"]["设置&textures/ui/settings_glyph_color_2x.png"] = json::array({"openform 设置"});
        j["buttons"]["其他&textures/ui/MashupIcon.png"] = json::array({"openform 其他"});
        
        std::ofstream file(mainFormFile);
        if (file.is_open()) {
            file << j.dump(4);
            created = true;
            logger.info("Created default main.json form");
        }
    }
    
    return created;
}

const MainConfig& ConfigManager::getMainConfig() const {
    return m_mainConfig;
}

const ItemConfig& ConfigManager::getItemConfig() const {
    return m_mainConfig.itemConfig;
}

FormConfig ConfigManager::getFormConfig(const std::string& formName) const {
    auto it = m_formConfigs.find(formName);
    if (it != m_formConfigs.end()) {
        return it->second;
    }
    return FormConfig{};
}

bool ConfigManager::hasFormConfig(const std::string& formName) const {
    return m_formConfigs.find(formName) != m_formConfigs.end();
}

std::string ConfigManager::getConfigPath() const {
    return m_configDir;
}

std::string ConfigManager::getFormsPath() const {
    return m_configDir + "/forms";
}

} // namespace starrysky::config
