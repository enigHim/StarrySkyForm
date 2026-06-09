#include "mod/MyMod.h"

#include "mod/Commands.h"
#include "mod/Config.h"
#include "mod/FormManager.h"
#include "mod/ItemUtil.h"

#include "ll/api/event/EventBus.h"
#include "ll/api/event/player/PlayerJoinEvent.h"
#include "ll/api/event/player/PlayerUseItemEvent.h"
#include "ll/api/mod/RegisterHelper.h"
#include "mc/world/actor/player/Player.h"

namespace starry_sky_form {

MyMod& MyMod::getInstance() {
    static MyMod instance;
    return instance;
}

bool MyMod::load() {
    getSelf().getLogger().debug("Loading...");
    initConfig();
    FormManager::getInstance().loadAllForms();
    return true;
}

bool MyMod::enable() {
    getSelf().getLogger().debug("Enabling...");

    registerCommands();

    auto& eventBus = ll::event::EventBus::getInstance();

    // Player join event - give special clock on first join
    mPlayerJoinListener = eventBus.emplaceListener<ll::event::PlayerJoinEvent>(
        [](ll::event::PlayerJoinEvent& event) {
            auto& player = event.self();
            if (!hasSpecialClock(player)) {
                giveSpecialClock(player);
            }
        }
    );

    // Player use item event - open form when using special clock
    mPlayerUseItemListener = eventBus.emplaceListener<ll::event::PlayerUseItemEvent>(
        [](ll::event::PlayerUseItemEvent& event) {
            auto& player    = event.self();
            auto& itemStack = event.item();

            std::string customName = itemStack.getCustomName();
            if (itemStack.getTypeName() == "minecraft:clock" && !customName.empty() &&
                customName == getConfig().clockName) {
                FormManager::getInstance().sendForm(player, "main");
            }
        }
    );

    return true;
}

bool MyMod::disable() {
    getSelf().getLogger().debug("Disabling...");

    auto& eventBus = ll::event::EventBus::getInstance();
    eventBus.removeListener(mPlayerJoinListener);
    eventBus.removeListener(mPlayerUseItemListener);

    unregisterCommands();

    return true;
}

} // namespace starry_sky_form

LL_REGISTER_MOD(starry_sky_form::MyMod, starry_sky_form::MyMod::getInstance());
