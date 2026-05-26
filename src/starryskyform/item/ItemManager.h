#pragma once

#include <string>
#include <memory>

// Forward declarations
class Player;
class ItemStack;
class Container;

namespace starrysky::item {

class ItemManager {
public:
    ItemManager();
    ~ItemManager();

    bool load();

    // Create form item
    [[nodiscard]] std::unique_ptr<ItemStack> createFormItem(bool locked = false) const;
    
    // Check if item is a form item
    [[nodiscard]] bool isFormItem(const ItemStack& item) const;
    
    // Check if item is fully locked (cannot move, drop, craft)
    [[nodiscard]] bool isLockedItem(const ItemStack& item) const;

    // Check if item has drop/craft protection (always true for form items)
    [[nodiscard]] bool isDropCraftProtected(const ItemStack& item) const;
    
    // Give form item to player
    void giveFormItem(Player& player, bool locked = false);
    
    // Set form item lock state
    // locked = true  -> fully locked (cannot move, drop, craft)
    // locked = false -> partially locked (can move, but cannot drop or craft)
    void setItemLock(ItemStack& item, bool locked);
    
    // Clear all form items from inventory
    int clearAllFormItems(Player& player);
    
    // Check if player has form item in inventory
    [[nodiscard]] bool hasFormItem(Player& player) const;
    
    // Get the slot containing form item (-1 if not found)
    [[nodiscard]] int getFormItemSlot(Player& player) const;
    
    // Setup new player with locked form item in slot 9
    void setupNewPlayer(Player& player);

private:
    void applyVanishingCurse(ItemStack& item) const;
};

} // namespace starrysky::item
