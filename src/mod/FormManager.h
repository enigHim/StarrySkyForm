#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace ll::form {
class SimpleForm;
}

class Player;

namespace starry_sky_form {

struct ButtonEntry {
    std::string              text;
    std::string              imagePath;
    std::vector<std::string> commands;
};

struct FormData {
    std::string              title;
    std::string              content;
    std::vector<ButtonEntry> buttons;
};

class FormManager {
public:
    static FormManager& getInstance();

    void loadAllForms();
    bool hasForm(const std::string& name) const;
    void sendForm(Player& player, const std::string& name);
    void executeCommand(Player& player, const std::string& command);

private:
    FormManager() = default;

    bool loadForm(const std::string& name);

    std::unordered_map<std::string, FormData> mForms;
};

} // namespace starry_sky_form
