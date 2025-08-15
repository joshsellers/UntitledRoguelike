#include "DevBossCmdLine.h"
#include "../../core/MessageManager.h"

DevBossCmdLine::DevBossCmdLine() {
    if (!_font.loadFromFile("res/font.ttf")) {
        MessageManager::displayMessage("Failed to load font!", 10, WARN);
    }
    _displayText.setCharacterSize(16);
    _displayText.setFont(_font);
    _displayText.setFillColor(sf::Color(0x00AA22FF));

    _bg.setFillColor(sf::Color(0x000000FF));
    _bg.setOutlineColor(sf::Color(0x444444FF));
    _bg.setOutlineThickness(2.f);
}

void DevBossCmdLine::update(const sf::Vector2f bossPos) {
    if (_isActive) {
        constexpr long long typeTime = 3000LL;
        const long long typeRate = typeTime / _currentCommandString.length();
        if (currentTimeMillis() - _lastCharacterTypeTime >= typeRate) {
            std::string buffer = "";
            for (int i = 0; i < _currentCommandIndex; i++) {
                if (_currentCommandString.at(i) != '*') buffer += _currentCommandString.at(i);
            }

            _displayText.setString(buffer);

            _currentCommandIndex++;
            _lastCharacterTypeTime = currentTimeMillis();

            if (_currentCommandIndex > _currentCommandString.length()) _isActive = false;
        }

        const sf::Vector2f textSize = { _displayText.getGlobalBounds().width, _displayText.getGlobalBounds().height };
        _pos = { bossPos.x - textSize.x / 2.f, bossPos.y - textSize.y - 8.f };
    }
}

void DevBossCmdLine::draw(sf::RenderTexture& surface) {
    if (_isActive) {
        _displayText.setPosition(_pos.x, _pos.y);

        constexpr float padding = 4.f;
        _bg.setPosition(_pos.x - padding, _pos.y - padding);
        _bg.setSize({_displayText.getGlobalBounds().width + padding * 2, _displayText.getGlobalBounds().height + padding * 4});

        if (_displayText.getString() != "") surface.draw(_bg);
        surface.draw(_displayText);
    }
}

void DevBossCmdLine::typeCommand(const std::string text) {
    if (!_isActive) {
        _displayText.setString("");
        _currentCommandIndex = 0;
        _currentCommandString = text + "***";
        _isActive = true;
        _lastCharacterTypeTime = currentTimeMillis() - 500LL;
    }
}

bool DevBossCmdLine::isActive() const {
    return _isActive;
}
