#pragma once

#include "ll/api/event/EventBus.h"
#include "ll/api/mod/NativeMod.h"

namespace starry_sky_form {

class MyMod {

public:
    static MyMod& getInstance();

    MyMod() : mSelf(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    bool load();
    bool enable();
    bool disable();

private:
    ll::mod::NativeMod& mSelf;

    ll::event::ListenerPtr mPlayerJoinListener;
    ll::event::ListenerPtr mPlayerUseItemListener;
};

} // namespace starry_sky_form
