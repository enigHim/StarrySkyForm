#pragma once

#include "mc/world/item/ItemStack.h"

class Player;

namespace starry_sky_form {

ItemStack createSpecialClock();
void      giveSpecialClock(Player& player);
bool      hasSpecialClock(Player& player);
void      removeSpecialClock(Player& player);

} // namespace starry_sky_form
