#pragma once

#include "starryskyform/config/ConfigManager.h"
#include <string>
#include <functional>

// Forward declarations
class Player;
class CommandOrigin;

namespace starrysky::form {

class FormManager {
public:
    FormManager();
    ~FormManager();

    bool load();
    bool reload();

    // Open a form for a player
    void openForm(Player& player, const std::string& formName);
    
    // Open main form
    void openMainForm(Player& player);
    
    // Execute commands from form button
    void executeCommands(Player& player, const std::vector<std::string>& commands);

private:
    void sendSimpleForm(Player& player, const config::FormConfig& config);
};

} // namespace starrysky::form
