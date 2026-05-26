#include "FormManager.h"

#include "starryskyform/StarrySkyForm.h"
#include "starryskyform/config/ConfigManager.h"
#include "ll/api/io/Logger.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/service/ServerInfo.h"
#include "mc/world/actor/player/Player.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOutput.h"

namespace starrysky::form {

FormManager::FormManager() = default;
FormManager::~FormManager() = default;

bool FormManager::load() {
    auto& logger = StarrySkyForm::getInstance().getSelf().getLogger();
    logger.info("FormManager loaded");
    return true;
}

bool FormManager::reload() {
    return true;
}

void FormManager::openForm(Player& player, const std::string& formName) {
    auto& configManager = StarrySkyForm::getInstance().getConfigManager();
    
    if (!configManager.hasFormConfig(formName)) {
        player.sendMessage("§c表单不存在: " + formName);
        return;
    }
    
    auto config = configManager.getFormConfig(formName);
    sendSimpleForm(player, config);
}

void FormManager::openMainForm(Player& player) {
    openForm(player, "main");
}

void FormManager::sendSimpleForm(Player& player, const config::FormConfig& config) {
    auto form = ll::form::SimpleForm{};
    form.setTitle(config.title);
    form.setContent(config.content);
    
    for (const auto& button : config.buttons) {
        if (button.image.empty()) {
            form.appendButton(button.text, [this, &player, commands = button.commands](Player& pl) {
                executeCommands(pl, commands);
            });
        } else {
            form.appendButton(button.text, button.image, "path", [this, &player, commands = button.commands](Player& pl) {
                executeCommands(pl, commands);
            });
        }
    }
    
    form.sendTo(player);
}

void FormManager::executeCommands(Player& player, const std::vector<std::string>& commands) {
    auto& server = ll::service::ServerInfo::getInstance();
    
    for (const auto& cmd : commands) {
        // Execute command as player
        std::string fullCmd = cmd;
        
        // Replace {player} placeholder with player name
        size_t pos = 0;
        while ((pos = fullCmd.find("{player}", pos)) != std::string::npos) {
            fullCmd.replace(pos, 8, player.getRealName());
            pos += player.getRealName().length();
        }
        
        // Handle openform command specially
        if (fullCmd.find("openform ") == 0) {
            std::string formName = fullCmd.substr(9);
            // Schedule opening form for next tick to avoid recursion issues
            openForm(player, formName);
        } else {
            // Execute other commands
            server.getMinecraft()->getCommands().executeCommand(
                std::make_unique<CommandOrigin>(player),
                fullCmd,
                nullptr,
                false
            );
        }
    }
}

} // namespace starrysky::form
