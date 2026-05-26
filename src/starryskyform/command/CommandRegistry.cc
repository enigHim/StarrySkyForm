#include "CommandRegistry.h"

#include "starryskyform/StarrySkyForm.h"
#include "starryskyform/Global.h"
#include "starryskyform/config/ConfigManager.h"
#include "starryskyform/form/FormManager.h"
#include "starryskyform/item/ItemManager.h"
#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/command/runtime/RuntimeCommand.h"
#include "ll/api/command/runtime/RuntimeOverload.h"
#include "ll/api/io/Logger.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/server/commands/CommandOriginType.h"
#include "mc/world/actor/player/Player.h"

namespace starrysky::command {

CommandRegistry::CommandRegistry() = default;
CommandRegistry::~CommandRegistry() = default;

bool CommandRegistry::registerCommands() {
    auto& logger = StarrySkyForm::getInstance().getSelf().getLogger();
    
    if (!registerOpenFormCommand()) {
        logger.error("Failed to register openform command");
        return false;
    }
    
    if (!registerGiveFormCommand()) {
        logger.error("Failed to register giveform command");
        return false;
    }
    
    if (!registerSetFormCommand()) {
        logger.error("Failed to register setform command");
        return false;
    }
    
    logger.info("Commands registered successfully");
    return true;
}

void CommandRegistry::unregisterCommands() {
    // Commands are automatically unregistered when plugin is disabled
}

bool CommandRegistry::registerOpenFormCommand() {
    auto& registrar = ll::command::CommandRegistrar::getInstance(false);
    auto& config = StarrySkyForm::getInstance().getConfigManager().getMainConfig();
    
    auto& cmd = registrar.getOrCreateCommand(
        config.commandOpenForm,
        "打开星空菜单表单"
    );
    
    // /openform - opens main form
    cmd.overload().execute([](CommandOrigin const& origin, CommandOutput& output) {
        if (origin.getOriginType() != CommandOriginType::Player) {
            output.error("此命令只能由玩家执行");
            return;
        }
        
        auto* player = static_cast<Player*>(origin.getEntity());
        if (!player) {
            output.error("无法获取玩家");
            return;
        }
        
        StarrySkyForm::getInstance().getFormManager().openMainForm(*player);
    });
    
    // /openform <formName> - opens specific form
    cmd.overload<ll::command::RuntimeCommand>()
        .text("form")
        .required("formName", ll::command::ParamKind::String)
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& param) {
            if (origin.getOriginType() != CommandOriginType::Player) {
                output.error("此命令只能由玩家执行");
                return;
            }
            
            auto* player = static_cast<Player*>(origin.getEntity());
            if (!player) {
                output.error("无法获取玩家");
                return;
            }
            
            auto formName = std::get<std::string>(param["formName"].value());
            StarrySkyForm::getInstance().getFormManager().openForm(*player, formName);
        });
    
    return true;
}

bool CommandRegistry::registerGiveFormCommand() {
    auto& registrar = ll::command::CommandRegistrar::getInstance(false);
    auto& config = StarrySkyForm::getInstance().getConfigManager().getMainConfig();
    auto& itemConfig = StarrySkyForm::getInstance().getConfigManager().getItemConfig();
    
    auto& cmd = registrar.getOrCreateCommand(
        config.commandGiveForm,
        "获得一个表单物品"
    );
    
    cmd.overload().execute([&itemConfig](CommandOrigin const& origin, CommandOutput& output) {
        if (origin.getOriginType() != CommandOriginType::Player) {
            output.error("此命令只能由玩家执行");
            return;
        }
        
        auto* player = static_cast<Player*>(origin.getEntity());
        if (!player) {
            output.error("无法获取玩家");
            return;
        }
        
        auto& itemManager = StarrySkyForm::getInstance().getItemManager();
        
        // Check if player already has form item
        if (itemManager.hasFormItem(*player)) {
            output.error("你拥有表单，无法获得");
            player->sendMessage("§c你拥有表单，无法获得");
            return;
        }
        
        // Give form item (locked or unlocked based on config)
        itemManager.giveFormItem(*player, itemConfig.lockedByDefault);
        output.success("已给予表单物品");
    });
    
    return true;
}

bool CommandRegistry::registerSetFormCommand() {
    auto& registrar = ll::command::CommandRegistrar::getInstance(false);
    auto& config = StarrySkyForm::getInstance().getConfigManager().getMainConfig();
    
    auto& cmd = registrar.getOrCreateCommand(
        config.commandSetForm,
        "设置表单状态"
    );
    
    // Register enum for lock/unlock/reload/clear
    if (!registrar.hasEnum("starrysky_set_action")) {
        registrar.tryRegisterRuntimeEnum(
            "starrysky_set_action",
            {
                {"lock", 0},
                {"unlock", 1},
                {"reload", 2},
                {"clear", 3}
            }
        );
    }
    
    cmd.overload<ll::command::RuntimeCommand>()
        .required("action", ll::command::ParamKind::Enum, "starrysky_set_action")
        .execute([](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& param) {
            auto action = std::get<ll::command::RuntimeEnum>(param["action"].value());
            
            // Handle reload command (OP only)
            if (action.index == 2) {
                // Console always allowed; players must be OP
                if (origin.getOriginType() == CommandOriginType::Player) {
                    auto* player = static_cast<Player*>(origin.getEntity());
                    if (!player || !player->isOperator()) {
                        output.error("此命令仅限管理员(OP)执行");
                        return;
                    }
                }

                auto& configManager = StarrySkyForm::getInstance().getConfigManager();
                if (configManager.reload()) {
                    output.success("表单配置文件已重新加载");
                } else {
                    output.error("表单配置文件重载失败");
                }
                return;
            }

            // Handle clear command (player clears their own inventory, no OP required)
            if (action.index == 3) {
                if (origin.getOriginType() != CommandOriginType::Player) {
                    output.error("此命令只能由玩家执行");
                    return;
                }

                auto* player = static_cast<Player*>(origin.getEntity());
                if (!player) {
                    output.error("无法获取玩家");
                    return;
                }

                auto& itemManager = StarrySkyForm::getInstance().getItemManager();
                int clearedCount = itemManager.clearAllFormItems(*player);

                if (clearedCount > 0) {
                    output.success("已清除 {} 个表单物品", clearedCount);
                    player->sendMessage("§a已清除背包中的表单物品");
                } else {
                    output.error("背包中没有找到表单物品");
                    player->sendMessage("§c背包中没有找到表单物品");
                }
                return;
            }

            // Lock/Unlock commands require player
            if (origin.getOriginType() != CommandOriginType::Player) {
                output.error("此命令只能由玩家执行");
                return;
            }

            auto* player = static_cast<Player*>(origin.getEntity());
            if (!player) {
                output.error("无法获取玩家");
                return;
            }

            bool lock = (action.index == 0); // 0 = lock, 1 = unlock

            auto& itemManager = StarrySkyForm::getInstance().getItemManager();
            auto& inventory = player->getInventory();
            auto* container = inventory.getContainer();

            if (!container) {
                output.error("无法获取背包");
                return;
            }

            int modifiedCount = 0;

            // Find and modify all form items in inventory
            for (int i = 0; i < container->getContainerSize(); i++) {
                auto* item = container->getItem(i);
                if (item && !item->isNull() && itemManager.isFormItem(*item)) {
                    itemManager.setItemLock(*item, lock);
                    modifiedCount++;
                }
            }

            if (modifiedCount > 0) {
                if (lock) {
                    output.success("已锁定 {} 个表单物品", modifiedCount);
                    player->sendMessage("§a已锁定表单");
                } else {
                    output.success("已解锁 {} 个表单物品", modifiedCount);
                    player->sendMessage("§a已解锁表单");
                }
            } else {
                output.error("背包中没有找到表单物品");
                player->sendMessage("§c背包中没有找到表单物品");
            }
        });
    
    return true;
}

} // namespace starrysky::command
