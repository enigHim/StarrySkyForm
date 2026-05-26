#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace starrysky::config {

struct FormButton {
    std::string text;
    std::string image; // Optional image path
    std::vector<std::string> commands;
};

struct FormConfig {
    std::string title;
    std::string content;
    std::vector<FormButton> buttons;
};

struct ItemConfig {
    int id = 720; // Clock
    int damage = 0;
    int count = 1;
    int slot = 8; // 9th slot (0-indexed)
    std::string name = "§b星空§b菜§b§b单";
    bool enchanted = true; // Vanishing curse
    bool lockedByDefault = true; // For new players
};

struct MainConfig {
    std::string commandOpenForm = "openform";
    std::string commandGiveForm = "giveform";
    std::string commandSetForm = "setform";
    ItemConfig itemConfig;
};

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();

    bool load();
    bool reload();
    bool save();

    [[nodiscard]] const MainConfig& getMainConfig() const;
    [[nodiscard]] const ItemConfig& getItemConfig() const;
    
    // Form configs
    [[nodiscard]] FormConfig getFormConfig(const std::string& formName) const;
    [[nodiscard]] bool hasFormConfig(const std::string& formName) const;
    
    // Paths
    [[nodiscard]] std::string getConfigPath() const;
    [[nodiscard]] std::string getFormsPath() const;

private:
    bool loadMainConfig();
    bool loadFormConfigs();
    bool createDefaultConfigs();
    
    MainConfig m_mainConfig;
    std::unordered_map<std::string, FormConfig> m_formConfigs;
    std::string m_configDir;
};

} // namespace starrysky::config
