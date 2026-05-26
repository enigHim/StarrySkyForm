#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

// Forward declarations
namespace ll::mod {
class NativeMod;
}

namespace starrysky {

// Plugin version
constexpr const char* PLUGIN_NAME = "StarrySkyForm";
constexpr const char* PLUGIN_VERSION = "1.0.0";

// Item constants
constexpr const char* FORM_ITEM_NAME = "§b星空§b菜§b§b单";
constexpr int FORM_ITEM_ID = 720; // Clock item
constexpr int FORM_ITEM_SLOT = 8; // 9th slot (0-indexed: 8)
constexpr int FORM_ITEM_COUNT = 1;
constexpr int FORM_ITEM_DAMAGE = 0;

// NBT tags
constexpr const char* NBT_FORM_ITEM_TAG = "starrysky_form_item";
constexpr const char* NBT_FORM_ITEM_LOCKED = "starrysky_form_locked";

// Command names
constexpr const char* CMD_OPEN_FORM = "openform";
constexpr const char* CMD_GIVE_FORM = "giveform";
constexpr const char* CMD_SET_FORM = "setform";

} // namespace starrysky
