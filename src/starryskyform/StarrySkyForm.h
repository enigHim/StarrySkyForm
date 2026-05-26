#pragma once

#include "Global.h"
#include "ll/api/mod/NativeMod.h"
#include <memory>

namespace starrysky {

// Forward declarations
namespace config {
class ConfigManager;
}
namespace form {
class FormManager;
}
namespace item {
class ItemManager;
}

class StarrySkyForm {
    StarrySkyForm();

public:
    [[nodiscard]] ll::mod::NativeMod& getSelf() const;

    bool load();
    bool enable();
    bool disable();
    bool unload();

    static StarrySkyForm& getInstance();

    [[nodiscard]] config::ConfigManager& getConfigManager() const;
    [[nodiscard]] form::FormManager& getFormManager() const;
    [[nodiscard]] item::ItemManager& getItemManager() const;

private:
    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

} // namespace starrysky
