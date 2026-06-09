#include "mod/Commands.h"
#include "mod/Config.h"
#include "mod/FormManager.h"
#include "mod/ItemUtil.h"

#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOriginType.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/server/commands/CommandPermissionLevel.h"
#include "mc/world/actor/player/Player.h"

namespace starry_sky_form {

static Player* getPlayerFromOrigin(::CommandOrigin const& origin) {
    if (origin.getOriginType() != ::CommandOriginType::Player) {
        return nullptr;
    }
    auto* entity = origin.getEntity();
    if (!entity) {
        return nullptr;
    }
    return static_cast<Player*>(entity);
}

struct OpenFormParam {
    std::string formName;
};

void registerCommands() {
    auto& cfg = getConfig();

    // /openform [formname]
    {
        auto& cmd = ll::command::CommandRegistrar::getServerInstance().getOrCreateCommand(
            cfg.openCommandName,
            "打开表单菜单",
            ::CommandPermissionLevel::Any
        );
        cmd.overload<OpenFormParam>()
            .optional("formName")
            .execute([](::CommandOrigin const& origin, ::CommandOutput& output, OpenFormParam const& param) {
                auto* player = getPlayerFromOrigin(origin);
                if (!player) {
                    output.error("只有玩家可以使用此命令");
                    return;
                }
                std::string name = param.formName.empty() ? "main" : param.formName;
                FormManager::getInstance().sendForm(*player, name);
            });
    }

    // /giveform
    {
        auto& cmd = ll::command::CommandRegistrar::getServerInstance().getOrCreateCommand(
            cfg.giveCommandName,
            "获得菜单钟",
            ::CommandPermissionLevel::Any
        );
        cmd.overload().execute([](::CommandOrigin const& origin, ::CommandOutput& output) {
            auto* player = getPlayerFromOrigin(origin);
            if (!player) {
                output.error("只有玩家可以使用此命令");
                return;
            }
            if (hasSpecialClock(*player)) {
                output.error("§c你已经拥有菜单钟了");
                return;
            }
            giveSpecialClock(*player);
            output.success("已获得菜单钟");
        });
    }

    // /reloadform
    {
        auto& cmd = ll::command::CommandRegistrar::getServerInstance().getOrCreateCommand(
            cfg.reloadCommandName,
            "重新加载表单配置",
            ::CommandPermissionLevel::GameDirectors
        );
        cmd.overload().execute([](::CommandOrigin const& origin, ::CommandOutput& output) {
            loadConfig();
            FormManager::getInstance().loadAllForms();
            output.success("表单配置已重新加载");
        });
    }

    // /clearform
    {
        auto& cmd = ll::command::CommandRegistrar::getServerInstance().getOrCreateCommand(
            cfg.clearCommandName,
            "清除菜单钟",
            ::CommandPermissionLevel::Any
        );
        cmd.overload().execute([](::CommandOrigin const& origin, ::CommandOutput& output) {
            auto* player = getPlayerFromOrigin(origin);
            if (!player) {
                output.error("只有玩家可以使用此命令");
                return;
            }
            removeSpecialClock(*player);
            output.success("已清除菜单钟");
        });
    }
}

void unregisterCommands() {
    // Commands are automatically cleaned up by the framework
}

} // namespace starry_sky_form
