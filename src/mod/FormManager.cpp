#include "mod/FormManager.h"

#include "mod/Config.h"

#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/mod/NativeMod.h"
#include "mc/server/commands/PlayerCommandOrigin.h"
#include "mc/server/commands/ServerCommandOrigin.h"
#include "mc/world/actor/player/Player.h"

#include <fstream>

#include "nlohmann/json.hpp"

namespace starry_sky_form {

FormManager& FormManager::getInstance() {
    static FormManager instance;
    return instance;
}

void FormManager::loadAllForms() {
    mForms.clear();
    auto formsDir = getFormsDir();
    if (!std::filesystem::exists(formsDir)) {
        return;
    }
    auto& logger = ll::mod::NativeMod::current()->getLogger();
    for (const auto& entry : std::filesystem::directory_iterator(formsDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            auto        u8name = entry.path().stem().u8string();
            std::string name(u8name.begin(), u8name.end());
            if (loadForm(name)) {
                logger.info("已加载表单: {}", name);
            } else {
                logger.error("加载表单失败: {}", name);
            }
        }
    }
}

bool FormManager::hasForm(const std::string& name) const {
    return mForms.count(name) > 0;
}

bool FormManager::loadForm(const std::string& name) {
    std::u8string u8name(name.begin(), name.end());
    auto          path = getFormsDir() / std::filesystem::path(u8name + u8".json");
    if (!std::filesystem::exists(path)) {
        return false;
    }
    try {
        std::ifstream ifs(path);
        auto          data = nlohmann::ordered_json::parse(ifs);

        FormData form;
        form.title   = data.value("title", "");
        form.content = data.value("content", "");

        if (data.contains("buttons") && data["buttons"].is_object()) {
            for (auto& [key, val] : data["buttons"].items()) {
                ButtonEntry btn;
                auto        sep = key.find('&');
                if (sep != std::string::npos) {
                    btn.text      = key.substr(0, sep);
                    btn.imagePath = key.substr(sep + 1);
                } else {
                    btn.text = key;
                }
                if (val.is_array()) {
                    for (auto& cmd : val) {
                        if (cmd.is_string()) {
                            btn.commands.push_back(cmd.get<std::string>());
                        }
                    }
                }
                form.buttons.push_back(std::move(btn));
            }
        }

        mForms[name] = std::move(form);
        return true;
    } catch (...) {
        return false;
    }
}

void FormManager::sendForm(Player& player, const std::string& name) {
    auto it = mForms.find(name);
    if (it == mForms.end()) {
        player.sendMessage("§c表单 " + name + " 不存在");
        return;
    }

    const auto& formData = it->second;
    ll::form::SimpleForm form(formData.title, formData.content);

    std::vector<std::vector<std::string>> allCommands;
    for (const auto& btn : formData.buttons) {
        if (btn.imagePath.empty()) {
            form.appendButton(btn.text);
        } else {
            form.appendButton(btn.text, btn.imagePath, "path");
        }
        allCommands.push_back(btn.commands);
    }

    form.sendTo(player, [allCommands](Player& p, int index, ll::form::FormCancelReason) {
        if (index < 0) {
            return;
        }
        if (static_cast<size_t>(index) >= allCommands.size()) {
            return;
        }
        auto& mgr = FormManager::getInstance();
        for (const auto& cmd : allCommands[index]) {
            mgr.executeCommand(p, cmd);
        }
    });
}

static std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
    return str;
}

void FormManager::executeCommand(Player& player, const std::string& command) {
    if (command.empty()) {
        return;
    }

    // Replace @s with quoted player name (handles names with spaces)
    std::string playerName = "\"" + player.getRealName() + "\"";
    std::string cmd        = replaceAll(command, "@s", playerName);

    // If the command starts with "openform", open the form directly
    if (cmd.rfind("openform ", 0) == 0) {
        std::string formName = cmd.substr(9);
        sendForm(player, formName);
        return;
    }

    auto& registrar = ll::command::CommandRegistrar::getServerInstance();

    // If prefixed with "console:", execute as server console
    if (cmd.rfind("console:", 0) == 0) {
        std::string consoleCmd = cmd.substr(8);
        auto&       level      = static_cast<ServerLevel&>(player.getLevel());
        ServerCommandOrigin origin("Server", level, ::CommandPermissionLevel::Owner, player.getDimensionId());
        registrar.executeCommand(consoleCmd, origin);
        return;
    }

    // Execute command as the player
    auto& level = player.getLevel();
    PlayerCommandOrigin origin(level, player.getOrCreateUniqueID());
    registrar.executeCommand(cmd, origin);
}

} // namespace starry_sky_form
