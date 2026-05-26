#include "StarrySkyForm.h"

#include "config/ConfigManager.h"
#include "form/FormManager.h"
#include "item/ItemManager.h"
#include "command/CommandRegistry.h"
#include "event/EventListener.h"

#include "ll/api/mod/NativeMod.h"
#include "ll/api/io/Logger.h"

namespace starrysky {

struct StarrySkyForm::Impl {
    std::unique_ptr<config::ConfigManager> configManager;
    std::unique_ptr<form::FormManager> formManager;
    std::unique_ptr<item::ItemManager> itemManager;
    std::unique_ptr<command::CommandRegistry> commandRegistry;
    std::unique_ptr<event::EventListener> eventListener;
};

StarrySkyForm::StarrySkyForm() : mImpl(std::make_unique<Impl>()) {}

StarrySkyForm& StarrySkyForm::getInstance() {
    static StarrySkyForm instance;
    return instance;
}

ll::mod::NativeMod& StarrySkyForm::getSelf() const {
    static auto* mod = ll::mod::NativeMod::current();
    return *mod;
}

bool StarrySkyForm::load() {
    auto& logger = getSelf().getLogger();
    logger.info("Loading StarrySkyForm...");

    // Initialize managers
    mImpl->configManager = std::make_unique<config::ConfigManager>();
    if (!mImpl->configManager->load()) {
        logger.error("Failed to load configuration!");
        return false;
    }

    mImpl->formManager = std::make_unique<form::FormManager>();
    if (!mImpl->formManager->load()) {
        logger.error("Failed to load forms!");
        return false;
    }

    mImpl->itemManager = std::make_unique<item::ItemManager>();
    
    mImpl->commandRegistry = std::make_unique<command::CommandRegistry>();
    if (!mImpl->commandRegistry->registerCommands()) {
        logger.error("Failed to register commands!");
        return false;
    }

    mImpl->eventListener = std::make_unique<event::EventListener>();
    if (!mImpl->eventListener->registerListeners()) {
        logger.error("Failed to register event listeners!");
        return false;
    }

    logger.info("StarrySkyForm loaded successfully!");
    return true;
}

bool StarrySkyForm::enable() {
    auto& logger = getSelf().getLogger();
    logger.info("Enabling StarrySkyForm...");
    
    // Additional enable logic if needed
    
    logger.info("StarrySkyForm enabled successfully!");
    return true;
}

bool StarrySkyForm::disable() {
    auto& logger = getSelf().getLogger();
    logger.info("Disabling StarrySkyForm...");
    
    // Cleanup
    if (mImpl->eventListener) {
        mImpl->eventListener->unregisterListeners();
    }
    
    logger.info("StarrySkyForm disabled!");
    return true;
}

bool StarrySkyForm::unload() {
    auto& logger = getSelf().getLogger();
    logger.info("Unloading StarrySkyForm...");
    
    // Cleanup managers
    mImpl->eventListener.reset();
    mImpl->commandRegistry.reset();
    mImpl->itemManager.reset();
    mImpl->formManager.reset();
    mImpl->configManager.reset();
    
    logger.info("StarrySkyForm unloaded!");
    return true;
}

config::ConfigManager& StarrySkyForm::getConfigManager() const {
    return *mImpl->configManager;
}

form::FormManager& StarrySkyForm::getFormManager() const {
    return *mImpl->formManager;
}

item::ItemManager& StarrySkyForm::getItemManager() const {
    return *mImpl->itemManager;
}

} // namespace starrysky

// Entry point for LeviLamina
extern "C" {
    _declspec(dllexport) bool ll_mod_load(ll::mod::NativeMod& mod) {
        return starrysky::StarrySkyForm::getInstance().load();
    }
    
    _declspec(dllexport) bool ll_mod_enable(ll::mod::NativeMod& mod) {
        return starrysky::StarrySkyForm::getInstance().enable();
    }
    
    _declspec(dllexport) bool ll_mod_disable(ll::mod::NativeMod& mod) {
        return starrysky::StarrySkyForm::getInstance().disable();
    }
    
    _declspec(dllexport) bool ll_mod_unload(ll::mod::NativeMod& mod) {
        return starrysky::StarrySkyForm::getInstance().unload();
    }
}
