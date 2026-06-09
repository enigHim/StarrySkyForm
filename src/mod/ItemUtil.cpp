#include "mod/ItemUtil.h"
#include "mod/Config.h"

#include "mc/deps/nbt/ByteTag.h"
#include "mc/deps/nbt/CompoundTag.h"
#include "mc/deps/nbt/CompoundTagVariant.h"
#include "mc/world/Container.h"
#include "mc/world/actor/player/Inventory.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/item/ItemLockMode.h"

namespace starry_sky_form {

ItemStack createSpecialClock() {
    ItemStack item;
    item.reinit("minecraft:clock", 1, 0);

    // Build full user data with display name + item lock
    auto tag              = std::make_unique<CompoundTag>();
    (*tag)["display"]     = CompoundTag{{{"Name", getConfig().clockName}}};
    (*tag)["minecraft:item_lock"]     = ByteTag{(schar)ItemLockMode::LockInInventory};
    (*tag)["minecraft:keep_on_death"] = ByteTag{(schar)1};
    item.setUserData(std::move(tag));

    return item;
}

void giveSpecialClock(Player& player) {
    ItemStack item = createSpecialClock();
    player.add(item);
    player.refreshInventory();
}

bool hasSpecialClock(Player& player) {
    auto& inv       = player.getInventory();
    int   size      = inv.getContainerSize();
    auto& clockName = getConfig().clockName;
    for (int i = 0; i < size; ++i) {
        const ItemStack& item = inv.getItem(i);
        if (!item.isNull() && item.getTypeName() == "minecraft:clock") {
            std::string customName = item.getCustomName();
            if (!customName.empty() && customName == clockName) {
                return true;
            }
        }
    }
    return false;
}

void removeSpecialClock(Player& player) {
    auto& inv       = player.getInventory();
    int   size      = inv.getContainerSize();
    auto& clockName = getConfig().clockName;
    for (int i = 0; i < size; ++i) {
        const ItemStack& item = inv.getItem(i);
        if (!item.isNull() && item.getTypeName() == "minecraft:clock") {
            std::string customName = item.getCustomName();
            if (!customName.empty() && customName == clockName) {
                inv.clearSlot(i);
            }
        }
    }
    player.refreshInventory();
}

} // namespace starry_sky_form
