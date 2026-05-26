#pragma once

namespace starrysky::command {

class CommandRegistry {
public:
    CommandRegistry();
    ~CommandRegistry();

    bool registerCommands();
    void unregisterCommands();

private:
    bool registerOpenFormCommand();
    bool registerGiveFormCommand();
    bool registerSetFormCommand();
};

} // namespace starrysky::command
