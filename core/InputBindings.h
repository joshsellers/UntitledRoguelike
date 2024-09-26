#ifndef _INPUT_BINDINGS_H
#define _INPUT_BINDINGS_H

#include "gamepad/GamePadButtons.h"
#include <map>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <fstream>
#include <vector>
#include "MessageManager.h"
#include "Util.h"
#include <filesystem>

class InputBindingManager {
public:
    enum class BINDABLE_ACTION : unsigned int {
        WALK,
        DODGE,
        RELOAD,
        SHOOT,
        INTERACT,
        TOGGLE_INVENTORY,
        TOGGLE_PAUSE,
        SKIP_COOLDOWN
    };

    static GAMEPAD_BUTTON getGamepadBinding(BINDABLE_ACTION action) {
        return GAMEPAD_BINDINGS[action];
    }

    static sf::Keyboard::Key getKeyboardBinding(BINDABLE_ACTION action) {
        return KEYBOARD_BINDINGS[action];
    }

    static std::string getGamepadButtonName(GAMEPAD_BUTTON button) {
        return GAMEPAD_BUTTON_NAMES[button];
    }

    static std::string getKeyName(sf::Keyboard::Key key) {
        return KEY_NAMES[key];
    }

    static std::string getActionName(BINDABLE_ACTION action) {
        return ACTION_NAMES[action];
    }

    static void init() {
        for (int i = 0; i <= 100; i++) {
            sf::Keyboard::Key key = (sf::Keyboard::Key)i;
            KEY_NAMES[key] = getKeyNameFromKey(key);
        }

        loadKeyboardBindings();
        loadGamepadBindings();
    }

    static void gamePadBindingSelected(BINDABLE_ACTION action, GAMEPAD_BUTTON button) {
        GAMEPAD_BINDINGS[action] = button;
        saveBindings("gamepadbindings.config");
    }

    static void keyboardBindingSelected(BINDABLE_ACTION action, sf::Keyboard::Key key) {
        KEYBOARD_BINDINGS[action] = key;
        saveBindings("keybindings.config");
    }

    static void resetBindings() {
        KEYBOARD_BINDINGS = DEFAULT_KEYBOARD_BINDINGS;
        GAMEPAD_BINDINGS = DEFAULT_GAMEPAD_BINDINGS;
        saveBindings("gamepadbindings.config");
        saveBindings("keybindings.config");
    }

private:
    static void loadKeyboardBindings() {
        std::string localLowPath = getLocalLowPath() + "\\keybindings.config";
        std::ifstream in(localLowPath);

        if (!in.good()) {
            MessageManager::displayMessage("Could not find keybindings.config", 5, DEBUG);
            in.close();
            return;
        } else {
            std::string line;
            while (getline(in, line)) {
                std::vector<std::string> parsedLine = splitString(line, "=");
                bool foundAction = false;
                for (auto& actionName : ACTION_NAMES) {
                    if (actionName.second == parsedLine[0]) {
                        foundAction = true;
                        bool foundKey = false;
                        for (auto& keyName : KEY_NAMES) {
                            if (keyName.second == parsedLine[1]) {
                                KEYBOARD_BINDINGS[actionName.first] = keyName.first;
                                foundKey = true;
                                break;
                            }
                        }

                        if (!foundKey) MessageManager::displayMessage("Could not find key named \"" + parsedLine[1] + "\"", 5, WARN);
                        break;
                    }
                }

                if (!foundAction) MessageManager::displayMessage("Could not find bindable action named \"" + parsedLine[0] + "\"", 5, WARN);
            }
        }

        in.close();
    }

    static void loadGamepadBindings() {
        std::string localLowPath = getLocalLowPath() + "\\gamepadbindings.config";
        std::ifstream in(localLowPath);

        if (!in.good()) {
            MessageManager::displayMessage("Could not find gamepadbindings.config", 5, DEBUG);
            in.close();
            return;
        } else {
            std::string line;
            while (getline(in, line)) {
                std::vector<std::string> parsedLine = splitString(line, "=");
                bool foundAction = false;
                for (auto& actionName : ACTION_NAMES) {
                    if (actionName.second == parsedLine[0]) {
                        foundAction = true;
                        bool foundButton = false;
                        for (auto& buttonName : GAMEPAD_BUTTON_NAMES) {
                            if (buttonName.second == parsedLine[1]) {
                                GAMEPAD_BINDINGS[actionName.first] = buttonName.first;
                                foundButton = true;
                                break;
                            }
                        }

                        if (!foundButton) MessageManager::displayMessage("Could not find button named \"" + parsedLine[1] + "\"", 5, WARN);
                        break;
                    }
                }

                if (!foundAction) MessageManager::displayMessage("Could not find bindable action named \"" + parsedLine[0] + "\"", 5, WARN);
            }
        }

        in.close();
    }

    static void saveBindings(std::string fileName) {
        std::string localLowPath = getLocalLowPath();
        if (localLowPath == "err") return;
        if (!std::filesystem::is_directory(localLowPath + "\\")) {
            std::filesystem::create_directory(localLowPath);
        }

        std::string path = localLowPath + "\\" + fileName;

        try {
            if (!std::filesystem::remove(path)) {
                MessageManager::displayMessage("Could not replace " + fileName, 5, DEBUG);
            }
        } catch (const std::filesystem::filesystem_error& err) {
            MessageManager::displayMessage("Could not replace " + fileName + ": " + (std::string)err.what(), 5, ERR);
        }

        try {
            std::ofstream out(path);
            if (fileName == "keybindings.config") {
                for (auto& binding : KEYBOARD_BINDINGS) {
                    out << getActionName(binding.first) << "=" << getKeyName(binding.second) << std::endl;
                }
            } else if (fileName == "gamepadbindings.config") {
                for (auto& binding : GAMEPAD_BINDINGS) {
                    out << getActionName(binding.first) << "=" << getGamepadButtonName(binding.second) << std::endl;
                }
            }
            out.close();
        } catch (std::exception ex) {
            MessageManager::displayMessage("Error writing to " + path + ": " + (std::string)ex.what(), 5, ERR);
        }
    }

    inline static std::map<BINDABLE_ACTION, GAMEPAD_BUTTON> GAMEPAD_BINDINGS = {
        {BINDABLE_ACTION::WALK, GAMEPAD_BUTTON::RIGHT_BUMPER},
        {BINDABLE_ACTION::DODGE, GAMEPAD_BUTTON::LEFT_BUMPER},
        {BINDABLE_ACTION::RELOAD, GAMEPAD_BUTTON::X},
        {BINDABLE_ACTION::SHOOT, GAMEPAD_BUTTON::RIGHT_TRIGGER},
        {BINDABLE_ACTION::INTERACT, GAMEPAD_BUTTON::X},
        {BINDABLE_ACTION::TOGGLE_INVENTORY, GAMEPAD_BUTTON::SELECT},
        {BINDABLE_ACTION::TOGGLE_PAUSE, GAMEPAD_BUTTON::START},
        {BINDABLE_ACTION::SKIP_COOLDOWN, GAMEPAD_BUTTON::B}
    };
    inline static std::map<BINDABLE_ACTION, GAMEPAD_BUTTON> DEFAULT_GAMEPAD_BINDINGS = GAMEPAD_BINDINGS;

    inline static std::map<BINDABLE_ACTION, sf::Keyboard::Key> KEYBOARD_BINDINGS = {
        {BINDABLE_ACTION::WALK, sf::Keyboard::Key::LShift},
        {BINDABLE_ACTION::DODGE, sf::Keyboard::Key::Space},
        {BINDABLE_ACTION::RELOAD, sf::Keyboard::Key::R},
        {BINDABLE_ACTION::INTERACT, sf::Keyboard::Key::E},
        {BINDABLE_ACTION::TOGGLE_INVENTORY, sf::Keyboard::Key::Tab},
        {BINDABLE_ACTION::TOGGLE_PAUSE, sf::Keyboard::Key::Escape},
        {BINDABLE_ACTION::SKIP_COOLDOWN, sf::Keyboard::Key::B}
    };
    inline static std::map<BINDABLE_ACTION, sf::Keyboard::Key> DEFAULT_KEYBOARD_BINDINGS = KEYBOARD_BINDINGS;

    inline static std::map<BINDABLE_ACTION, std::string> ACTION_NAMES = {
        {BINDABLE_ACTION::WALK, "slow walk"},
        {BINDABLE_ACTION::DODGE, "dodge"},
        {BINDABLE_ACTION::RELOAD, "reload"},
        {BINDABLE_ACTION::SHOOT, "shoot"},
        {BINDABLE_ACTION::INTERACT, "interact"},
        {BINDABLE_ACTION::TOGGLE_INVENTORY, "show/hide inventory"},
        {BINDABLE_ACTION::TOGGLE_PAUSE, "pause"},
        {BINDABLE_ACTION::SKIP_COOLDOWN, "skip to next wave"}
    };

    inline static std::map<GAMEPAD_BUTTON, std::string> GAMEPAD_BUTTON_NAMES = {
        {GAMEPAD_BUTTON::A, "A"},
        {GAMEPAD_BUTTON::B, "B"},
        {GAMEPAD_BUTTON::Y, "Y"},
        {GAMEPAD_BUTTON::X, "X"},

        {GAMEPAD_BUTTON::START, "start"},
        {GAMEPAD_BUTTON::SELECT, "select"},

        {GAMEPAD_BUTTON::LEFT_STICK, "press left stick"},
        {GAMEPAD_BUTTON::RIGHT_STICK, "press right stick"},

        {GAMEPAD_BUTTON::LEFT_BUMPER, "left bumper"},
        {GAMEPAD_BUTTON::RIGHT_BUMPER, "right bumper"},

        {GAMEPAD_BUTTON::LEFT_TRIGGER, "left trigger"},
        {GAMEPAD_BUTTON::RIGHT_TRIGGER, "right trigger"},
    };

    inline static std::map<sf::Keyboard::Key, std::string> KEY_NAMES;

    static const std::string getKeyNameFromKey(const sf::Keyboard::Key key) {
        switch (key) {
            default:
            case sf::Keyboard::Unknown:
                return "Unknown";
            case sf::Keyboard::A:
                return "A";
            case sf::Keyboard::B:
                return "B";
            case sf::Keyboard::C:
                return "C";
            case sf::Keyboard::D:
                return "D";
            case sf::Keyboard::E:
                return "E";
            case sf::Keyboard::F:
                return "F";
            case sf::Keyboard::G:
                return "G";
            case sf::Keyboard::H:
                return "H";
            case sf::Keyboard::I:
                return "I";
            case sf::Keyboard::J:
                return "J";
            case sf::Keyboard::K:
                return "K";
            case sf::Keyboard::L:
                return "L";
            case sf::Keyboard::M:
                return "M";
            case sf::Keyboard::N:
                return "N";
            case sf::Keyboard::O:
                return "O";
            case sf::Keyboard::P:
                return "P";
            case sf::Keyboard::Q:
                return "Q";
            case sf::Keyboard::R:
                return "R";
            case sf::Keyboard::S:
                return "S";
            case sf::Keyboard::T:
                return "T";
            case sf::Keyboard::U:
                return "U";
            case sf::Keyboard::V:
                return "V";
            case sf::Keyboard::W:
                return "W";
            case sf::Keyboard::X:
                return "X";
            case sf::Keyboard::Y:
                return "Y";
            case sf::Keyboard::Z:
                return "Z";
            case sf::Keyboard::Num0:
                return "Num0";
            case sf::Keyboard::Num1:
                return "Num1";
            case sf::Keyboard::Num2:
                return "Num2";
            case sf::Keyboard::Num3:
                return "Num3";
            case sf::Keyboard::Num4:
                return "Num4";
            case sf::Keyboard::Num5:
                return "Num5";
            case sf::Keyboard::Num6:
                return "Num6";
            case sf::Keyboard::Num7:
                return "Num7";
            case sf::Keyboard::Num8:
                return "Num8";
            case sf::Keyboard::Num9:
                return "Num9";
            case sf::Keyboard::Escape:
                return "Escape";
            case sf::Keyboard::LControl:
                return "LControl";
            case sf::Keyboard::LShift:
                return "LShift";
            case sf::Keyboard::LAlt:
                return "LAlt";
            case sf::Keyboard::LSystem:
                return "LSystem";
            case sf::Keyboard::RControl:
                return "RControl";
            case sf::Keyboard::RShift:
                return "RShift";
            case sf::Keyboard::RAlt:
                return "RAlt";
            case sf::Keyboard::RSystem:
                return "RSystem";
            case sf::Keyboard::Menu:
                return "Menu";
            case sf::Keyboard::LBracket:
                return "LBracket";
            case sf::Keyboard::RBracket:
                return "RBracket";
            case sf::Keyboard::SemiColon:
                return "SemiColon";
            case sf::Keyboard::Comma:
                return "Comma";
            case sf::Keyboard::Period:
                return "Period";
            case sf::Keyboard::Quote:
                return "Quote";
            case sf::Keyboard::Slash:
                return "Slash";
            case sf::Keyboard::BackSlash:
                return "BackSlash";
            case sf::Keyboard::Tilde:
                return "Tilde";
            case sf::Keyboard::Equal:
                return "Equal";
            case sf::Keyboard::Dash:
                return "Dash";
            case sf::Keyboard::Space:
                return "Space";
            case sf::Keyboard::Return:
                return "Return";
            case sf::Keyboard::BackSpace:
                return "BackSpace";
            case sf::Keyboard::Tab:
                return "Tab";
            case sf::Keyboard::PageUp:
                return "PageUp";
            case sf::Keyboard::PageDown:
                return "PageDown";
            case sf::Keyboard::End:
                return "End";
            case sf::Keyboard::Home:
                return "Home";
            case sf::Keyboard::Insert:
                return "Insert";
            case sf::Keyboard::Delete:
                return "Delete";
            case sf::Keyboard::Add:
                return "Add";
            case sf::Keyboard::Subtract:
                return "Subtract";
            case sf::Keyboard::Multiply:
                return "Multiply";
            case sf::Keyboard::Divide:
                return "Divide";
            case sf::Keyboard::Left:
                return "Left";
            case sf::Keyboard::Right:
                return "Right";
            case sf::Keyboard::Up:
                return "Up";
            case sf::Keyboard::Down:
                return "Down";
            case sf::Keyboard::Numpad0:
                return "Numpad0";
            case sf::Keyboard::Numpad1:
                return "Numpad1";
            case sf::Keyboard::Numpad2:
                return "Numpad2";
            case sf::Keyboard::Numpad3:
                return "Numpad3";
            case sf::Keyboard::Numpad4:
                return "Numpad4";
            case sf::Keyboard::Numpad5:
                return "Numpad5";
            case sf::Keyboard::Numpad6:
                return "Numpad6";
            case sf::Keyboard::Numpad7:
                return "Numpad7";
            case sf::Keyboard::Numpad8:
                return "Numpad8";
            case sf::Keyboard::Numpad9:
                return "Numpad9";
            case sf::Keyboard::F1:
                return "F1";
            case sf::Keyboard::F2:
                return "F2";
            case sf::Keyboard::F3:
                return "F3";
            case sf::Keyboard::F4:
                return "F4";
            case sf::Keyboard::F5:
                return "F5";
            case sf::Keyboard::F6:
                return "F6";
            case sf::Keyboard::F7:
                return "F7";
            case sf::Keyboard::F8:
                return "F8";
            case sf::Keyboard::F9:
                return "F9";
            case sf::Keyboard::F10:
                return "F10";
            case sf::Keyboard::F11:
                return "F11";
            case sf::Keyboard::F12:
                return "F12";
            case sf::Keyboard::F13:
                return "F13";
            case sf::Keyboard::F14:
                return "F14";
            case sf::Keyboard::F15:
                return "F15";
            case sf::Keyboard::Pause:
                return "Pause";
        }
    }
};

#endif