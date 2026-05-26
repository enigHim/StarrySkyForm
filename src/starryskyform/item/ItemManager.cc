#include "ItemManager.h"

#include "starryskyform/StarrySkyForm.h"
#include "starryskyform/Global.h"
#include "starryskyform/config/ConfigManager.h"
#include "ll/api/io/Logger.h"
#include "ll/api/service/GamingStatus.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/actor/player/PlayerInventory.h"
#include "mc/world/inventory/Inventory.h"
#include "mc/world/inventory/InventorySource.h"
#include "mc/world/inventory/InventoryAction.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/item/ItemStackBase.h"
#include "mc/world/item/Item.h"
#include "mc/world/item/enchanting/Enchant.h"
#include "mc/world/item/enchanting/EnchantUtils.h"
#include "mc/world/item/enchanting/ItemEnchants.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/IntTag.h"
#include "mc/nbt/StringTag.h"
#include "mc/nbt/ByteTag.h"

namespace starrysky::item {

ItemManager::ItemManager() = default;
ItemManager::~ItemManager() = default;

bool ItemManager::load() {
    auto& logger = StarrySkyForm::getInstance().getSelf().getLogger();
    logger.info("ItemManager loaded");
    return true;
}

std::unique_ptr<ItemStack> ItemManager::createFormItem(bool locked) const {
    auto& config = StarrySkyForm::getInstance().getConfigManager().getItemConfig();
    
    // Create item stack
    auto item = std::make_unique<ItemStack>(ItemStack(config.id, config.count, config.damage));
    
    // Set custom name
    item->setCustomName(config.name);
    
    // Apply vanishing curse enchantment
    if (config.enchanted) {
        applyVanishingCurse(*item);
    }
    
    // Set NBT tags
    auto& tag = item->getOrCreateComponent<CompoundTag>(NBT_FORM_ITEM_TAG);
    tag.putBoolean(NBT_FORM_ITEM_TAG, true);
    tag.putBoolean(NBT_FORM_ITEM_LOCKED, locked);
    
    return item;
}

bool ItemManager::isFormItem(const ItemStack& item) const {
    auto& config = StarrySkyForm::getInstance().getConfigManager().getItemConfig();
    
    // Check item ID and name
    if (item.getId() != config.id) {
        return false;
    }
    
    std::string customName = item.getCustomName();
    if (customName != config.name) {
        return false;
    }
    
    return true;
}

bool ItemManager::isLockedItem(const ItemStack& item) const {
    auto* tag = item.getTag();
    if (tag && tag->contains(NBT_FORM_ITEM_TAG)) {
        auto* formTag = tag->getCompound(NBT_FORM_ITEM_TAG);
        if (formTag && formTag->contains(NBT_FORM_ITEM_LOCKED)) {
            return formTag->getBoolean(NBT_FORM_ITEM_LOCKED);
        }
    }
    return false;
}

bool ItemManager::isDropCraftProtected(const ItemStack& item) const {
    // All form items are always protected from drop and craft,
    // regardless of lock state
    return isFormItem(item);
}

void ItemManager::giveFormItem(Player& player, bool locked) {
    auto& config = StarrySkyForm::getInstance().getConfigManager().getItemConfig();
    auto item = createFormItem(locked);
    
    // Set item in specified slot
    auto& inventory = player.getInventory();
    auto* container = inventory.getContainer();
    if (container) {
        container->setItem(config.slot, *item);
        player.sendMessage("§a成功给予快捷菜单~");
    }
}

void ItemManager::setItemLock(ItemStack& item, bool locked) {
    auto* tag = item.getTag();
    if (!tag) {
        tag = &item.getOrCreateComponent<CompoundTag>("");
    }
    
    auto* formTag = tag->getCompound(NBT_FORM_ITEM_TAG);
    if (!formTag) {
        formTag = &tag->putCompound(NBT_FORM_ITEM_TAG, std::make_unique<CompoundTag>());
    }
    
    formTag->putBoolean(NBT_FORM_ITEM_LOCKED, locked);
}

int ItemManager::clearAllFormItems(Player& player) {
    auto& inventory = player.getInventory();
    auto* container = inventory.getContainer();

    if (!container) return 0;

    int clearedCount = 0;

    // Remove all form items
    for (int i = 0; i < container->getContainerSize(); i++) {
        auto* item = container->getItem(i);
        if (item && !item->isNull() && isFormItem(*item)) {
            clearedCount += item->getCount();
            container->removeItem(i, item->getCount());
        }
    }

    return clearedCount;
}

bool ItemManager::hasFormItem(Player& player) const {
    auto& inventory = player.getInventory();
    auto* container = inventory.getContainer();
    
    if (!container) return false;
    
    for (int i = 0; i < container->getContainerSize(); i++) {
        auto* item = container->getItem(i);
        if (item && !item->isNull() && isFormItem(*item)) {
            return true;
        }
    }
    
    return false;
}

int ItemManager::getFormItemSlot(Player& player) const {
    auto& inventory = player.getInventory();
    auto* container = inventory.getContainer();
    
    if (!container) return -1;
    
    for (int i = 0; i < container->getContainerSize(); i++) {
        auto* item = container->getItem(i);
        if (item && !item->isNull() && isFormItem(*item)) {
            return i;
        }
    }
    
    return -1;
}

void ItemManager::setupNewPlayer(Player& player) {
    auto& config = StarrySkyForm::getInstance().getConfigManager().getItemConfig();
    
    // Clear any existing form items first
    clearExtraFormItems(player);
    
    // Give locked form item in configured slot
    giveFormItem(player, config.lockedByDefault);
}

void ItemManager::applyVanishingCurse(ItemStack& item) const {
    // Apply vanishing curse enchantment (enchantment ID 28)
    auto enchants = item.getEnchantsFromUserData();
    if (!enchants) {
        enchants = std::make_unique<ItemEnchants>(ItemEnchants::EMPTY);
    }
    
    // Add vanishing curse (enchantment ID 28, level 1)
    enchants->addEnchant(Enchant::getEnchant(28), 1);
    item.setEnchantsFromUserData(*enchants);
}

} // namespace starrysky::item
