#include "EventListener.h"

#include "starryskyform/StarrySkyForm.h"
#include "starryskyform/Global.h"
#include "starryskyform/config/ConfigManager.h"
#include "starryskyform/form/FormManager.h"
#include "starryskyform/item/ItemManager.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/player/PlayerJoinEvent.h"
#include "ll/api/event/player/PlayerUseItemEvent.h"
#include "ll/api/event/player/PlayerDropItemEvent.h"
#include "ll/api/event/inventory/InventoryChangeEvent.h"
#include "ll/api/event/inventory/CraftEvent.h"
#include "ll/api/io/Logger.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/item/ItemStack.h"

namespace starrysky::event {

EventListener::EventListener() = default;
EventListener::~EventListener() = default;

bool EventListener::registerListeners() {
    auto& logger = StarrySkyForm::getInstance().getSelf().getLogger();
    auto& eventBus = ll::event::EventBus::getInstance();

    // Player join event - give form item to new players
    m_playerJoinListener = eventBus.emplaceListener<ll::event::player::PlayerJoinEvent>(
        [](ll::event::player::PlayerJoinEvent& event) {
            auto& player = event.self();
            auto& itemManager = StarrySkyForm::getInstance().getItemManager();
            auto& config = StarrySkyForm::getInstance().getConfigManager().getItemConfig();

            // Check if player already has form item
            if (!itemManager.hasFormItem(player)) {
                // Give locked form item to new players
                itemManager.giveFormItem(player, config.lockedByDefault);
            }
        }
    );

    // Player use item event - open form when using the form item
    m_playerUseItemListener = eventBus.emplaceListener<ll::event::player::PlayerUseItemEvent>(
        [](ll::event::player::PlayerUseItemEvent& event) {
            auto& player = event.self();
            auto& item = event.item();
            auto& itemManager = StarrySkyForm::getInstance().getItemManager();

            // Check if the item is the form item
            if (itemManager.isFormItem(item)) {
                // Cancel the event to prevent normal item use
                event.cancel();

                // Open the main form
                StarrySkyForm::getInstance().getFormManager().openMainForm(player);
            }
        }
    );

    // Player drop item event - prevent dropping ALL form items
    // (drop protection is always active, regardless of lock state)
    m_itemDropListener = eventBus.emplaceListener<ll::event::player::PlayerDropItemEvent>(
        [](ll::event::player::PlayerDropItemEvent& event) {
            auto& item = event.item();
            auto& itemManager = StarrySkyForm::getInstance().getItemManager();

            // All form items cannot be dropped, whether locked or unlocked
            if (itemManager.isDropCraftProtected(item)) {
                event.cancel();
                event.self().sendMessage("§c表单物品无法丢弃");
            }
        }
    );

    // Inventory change event - prevent moving locked form items
    m_inventoryChangeListener = eventBus.emplaceListener<ll::event::inventory::InventoryChangeEvent>(
        [](ll::event::inventory::InventoryChangeEvent& event) {
            auto& itemManager = StarrySkyForm::getInstance().getItemManager();

            // Check source item (if being moved)
            auto* sourceItem = event.sourceItem();
            if (sourceItem && !sourceItem->isNull() && itemManager.isFormItem(*sourceItem)) {
                if (itemManager.isLockedItem(*sourceItem)) {
                    // Cancel the move for locked items
                    event.cancel();
                    if (auto* player = event.self().getOwner(); player) {
                        player->sendMessage("§c锁定的表单物品无法移动");
                    }
                    return;
                }
            }
        }
    );

    // Craft event - prevent using form items in crafting
    m_craftListener = eventBus.emplaceListener<ll::event::inventory::CraftEvent>(
        [](ll::event::inventory::CraftEvent& event) {
            auto& itemManager = StarrySkyForm::getInstance().getItemManager();

            // Check all items in the crafting grid
            for (const auto& item : event.items()) {
                if (item && !item->isNull() && itemManager.isFormItem(*item)) {
                    // Cancel the craft
                    event.cancel();
                    if (auto* player = event.self().getOwner(); player) {
                        player->sendMessage("§c表单物品无法用于合成");
                    }
                    return;
                }
            }
        }
    );

    logger.info("Event listeners registered successfully");
    return true;
}

void EventListener::unregisterListeners() {
    auto& eventBus = ll::event::EventBus::getInstance();

    if (m_playerJoinListener) {
        eventBus.removeListener(m_playerJoinListener);
        m_playerJoinListener.reset();
    }

    if (m_playerUseItemListener) {
        eventBus.removeListener(m_playerUseItemListener);
        m_playerUseItemListener.reset();
    }

    if (m_itemDropListener) {
        eventBus.removeListener(m_itemDropListener);
        m_itemDropListener.reset();
    }

    if (m_inventoryChangeListener) {
        eventBus.removeListener(m_inventoryChangeListener);
        m_inventoryChangeListener.reset();
    }

    if (m_craftListener) {
        eventBus.removeListener(m_craftListener);
        m_craftListener.reset();
    }
}

} // namespace starrysky::event
