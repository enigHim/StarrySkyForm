#pragma once

#include <memory>

namespace ll::event {
class Listener;
}

namespace starrysky::event {

class EventListener {
public:
    EventListener();
    ~EventListener();

    bool registerListeners();
    void unregisterListeners();

private:
    std::shared_ptr<ll::event::Listener> m_playerJoinListener;
    std::shared_ptr<ll::event::Listener> m_playerUseItemListener;
    std::shared_ptr<ll::event::Listener> m_itemDropListener;
    std::shared_ptr<ll::event::Listener> m_inventoryChangeListener;
    std::shared_ptr<ll::event::Listener> m_craftListener;
};

} // namespace starrysky::event
