#include "EndGameSequence.h"
#include <fstream>
#include "MessageManager.h"
#include "../ui/UIElement.h"
#include "music/MusicManager.h"

void EndGameSequence::start() {
    _startTimeMillis = currentTimeMillis();
    _isActive = true;
    loadCreditsText();
    MusicManager::setSituation(MUSIC_SITUATION::CREDITS);

    for (const auto& listener : _listeners) listener->onEndGameSequenceStart();
}

void EndGameSequence::end() {
    _isActive = false;


    _bgAlpha = 0x00;
    _scrollPosY = 102;
    _creditsStrings.clear();
    _fadeComplete = false;
    for (const auto& listener : _listeners) listener->onEndGameSequenceEnd();
}

void EndGameSequence::update() {
    constexpr long long fadeStartDelay = 2000LL;
    if (currentTimeMillis() - _startTimeMillis >= fadeStartDelay) {

        constexpr long long fadeRate = 25LL;
        if (currentTimeMillis() - _lastFadeIncrease >= fadeRate && _bgAlpha < 0xAA) {
            _bgAlpha++;
        } else if (_bgAlpha == 0xAA) {
            _fadeComplete = true;
            constexpr float creditsRollSpeed = 0.14f;
            _scrollPosY -= creditsRollSpeed;
        }
    }
}

void EndGameSequence::draw(sf::RenderTexture& surface) {
    sf::RectangleShape bg;
    bg.setPosition(0, 0);
    bg.setSize(UIElement::getRelativePos(100, 100));
    bg.setFillColor(sf::Color(_bgAlpha));
    surface.draw(bg);

    const float lineSpacing = UIElement::getRelativeHeight(5.f);
    for (int i = 0; i < _creditsStrings.size(); i++) {
        _creditsLabel.setString(_creditsStrings.at(i));
        _creditsLabel.setPosition(UIElement::getRelativeWidth(50.f) - _creditsLabel.getGlobalBounds().width / 2.f, UIElement::getRelativeHeight(_scrollPosY) + i * lineSpacing);

        if (i == _creditsStrings.size() - 1 && _creditsLabel.getGlobalBounds().top + _creditsLabel.getGlobalBounds().height < 0) {
            end();
        }

        surface.draw(_creditsLabel);
    }
}

bool EndGameSequence::isActive() {
    return _isActive;
}

bool EndGameSequence::fadeComplete() {
    return _fadeComplete;
}

void EndGameSequence::addListener(std::shared_ptr<EndGameSequenceListener> listener) {
    _listeners.push_back(listener);
}

void EndGameSequence::loadCreditsText() {
    const std::string filePath = "res/credits.txt";
    std::ifstream in(filePath);

    if (in.good()) {
        std::string line;
        while (getline(in, line)) {
            _creditsStrings.push_back(line);
        }
    } else {
        MessageManager::displayMessage("Failed to open " + filePath, 5, WARN);
    }

    in.close();

    if (!_font.loadFromFile("res/font.ttf")) {
        MessageManager::displayMessage("Failed to load font!", 5, WARN);
        return;
    }

    _creditsLabel.setFont(_font);
    _creditsLabel.setFillColor(sf::Color(0xFFFFFFFF));
    _creditsLabel.setOutlineThickness(UIElement::getRelativeWidth(0.25f));
    _creditsLabel.setCharacterSize(UIElement::getRelativeWidth(1.5f));

    _creditsLabel.setPosition(UIElement::getRelativePos(0, _scrollPosY));
}
