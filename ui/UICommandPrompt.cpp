#include "UICommandPrompt.h"
#include <iostream>

UICommandPrompt::UICommandPrompt(World* world, sf::Font font) : _world(world),
    UIElement(30, 30, 4, 4, false, false, font) {
    _disableAutomaticTextAlignment = true;

    _bg.setSize(sf::Vector2f(_width, _height));
    _bg.setPosition(sf::Vector2f(_x, _y));
    _bg.setFillColor(sf::Color(0x2D2D30FF));
    _bg.setOutlineColor(sf::Color(0x3E3E41FF));
    _bg.setOutlineThickness(getRelativeWidth(0.25f));

    float fontSize = 1;
    int relativeFontSize = getRelativeWidth(fontSize);
    _text.setFont(_font);
    _text.setCharacterSize(relativeFontSize);
    _text.setFillColor(sf::Color(0x00AA22FF));

    _hiddenText.setFont(_font);
    _hiddenText.setCharacterSize(_text.getCharacterSize());
    _hiddenText.setFillColor(_text.getFillColor());
}

void UICommandPrompt::update() {

}

void UICommandPrompt::draw(sf::RenderTexture& surface) {
    sf::Text displayText = _text;
    std::string textString = displayText.getString();
    std::string displayTextString;
    for (int i = 0; i < textString.length(); i++) {
        displayTextString += textString.at(i);
        if (textString.at(i) == ';') {
            displayTextString += '\n';
        }
    }
    displayText.setString(displayTextString);

    float width = displayText.getGlobalBounds().width;
    float height = displayText.getGlobalBounds().height;

    float x = getRelativeWidth(50.f) - width / 2;
    float y = getRelativeHeight(88.f) - height / 2;
    displayText.setPosition(sf::Vector2f(x, y));

    float padding = getRelativeWidth(1.f);
    _bg.setPosition(sf::Vector2f(x - padding, y - padding));
    _bg.setSize(sf::Vector2f(width + padding * 2, height + padding * 2));

    surface.draw(_bg);
    if (_unlocked) surface.draw(displayText);
    else {
        _hiddenText.setPosition(_text.getPosition());
        _hiddenText.setString(_text.getString());
        std::string temp = _hiddenText.getString();
        for (int i = 0; i < temp.length(); i++) {
            temp.at(i) = '*';
        }
        _hiddenText.setString(temp);
        surface.draw(_hiddenText);
    }
}

void UICommandPrompt::mouseButtonPressed(const int mx, const int my, const int button) {
}

void UICommandPrompt::mouseButtonReleased(const int mx, const int my, const int button) {
}

void UICommandPrompt::mouseMoved(const int mx, const int my) {
}

void UICommandPrompt::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
}

void UICommandPrompt::keyReleased(sf::Keyboard::Key& key) {
    switch (key) {
        case sf::Keyboard::Up:
            if (_historyIndex > 0) {
                _historyIndex--;
            }
            break;
        case sf::Keyboard::Down:
            if (_historyIndex < _history.size() - 1) {
                _historyIndex++;
            }
            break;
        default:
            return;
    }

    if (_history.size() && _historyIndex < _history.size()) _text.setString(_history.at(_historyIndex));
}

void UICommandPrompt::textEntered(const sf::Uint32 character) {
    sf::String userInput = _text.getString();
    if (character != 13) {
        if (character == '\b' && userInput.getSize()) {
            userInput.erase(userInput.getSize() - 1, 1);
        } else if (character != '\b') {
            userInput += character;
        }
        _text.setString(userInput);
    } else {
        for (int i = 0; i < _history.size(); i++) {
            if (_history.at(i) == userInput) {
                _history.erase(_history.begin() + i);
                break;
            }
        }
        if (userInput != "") _history.push_back(userInput);
        _historyIndex = _history.size();
        if (_unlocked) {
            const std::vector<std::string> chainedCommands = splitString(userInput, ";");
            if (chainedCommands.size() > 1) {
                for (auto& command : chainedCommands) {
                    if (command == "") continue;
                    MessageManager::displayMessage(processCommand(command), 5);
                }
            } else {
                MessageManager::displayMessage(processCommand(userInput), 5);
            }
        } else if (processCommand("hash:" + userInput) == UNLOCK_HASH) {
            unlock();
            MessageManager::displayMessage("Command prompt has been unlocked", 5);
        } else MessageManager::displayMessage("Commmand prompt is locked", 5);
        _text.setString("");
    }
}

std::string UICommandPrompt::processCommand(sf::String commandInput) {
    std::string command = (std::string)commandInput;
    //boost::to_lower(command);

    size_t pos = 0;
    std::vector<std::string> parsedCommand;
    std::string delimiter = ":";
    while ((pos = command.find(delimiter)) != std::string::npos) {
        parsedCommand.push_back(command.substr(0, pos));
        command.erase(0, pos + delimiter.length());
    }
    parsedCommand.push_back(command);

    std::string commandHeader = parsedCommand.at(0);
    if (_commands.find(commandHeader) == _commands.end()) {
        return "Command not recognized: " + (std::string)("\"") + commandHeader + "\"";
    } else {
        return _commands.at(commandHeader).command(parsedCommand);
    }

    return "If you're reading this, josh made a mistake";
}

void UICommandPrompt::unlock() {
    _unlocked = true;
}

void UICommandPrompt::lock() {
    _unlocked = false;
}
