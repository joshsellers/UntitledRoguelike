#include "UIMessageDisplay.h"
#include "../core/MessageManager.h"
#include <iostream>
#include "UIHandler.h"

UIMessageDisplay::UIMessageDisplay(sf::Font font) : UIElement(30, 30 , 4, 4, false, false, font) {
    const auto& spriteSheet = UIHandler::getUISpriteSheet().get();
    _leftEdge.setTexture(spriteSheet);
    _leftTopCorner.setTexture(spriteSheet);
    _leftBottomCorner.setTexture(spriteSheet);
    _center.setTexture(spriteSheet);
    _centerTop.setTexture(spriteSheet);
    _centerBottom.setTexture(spriteSheet);
    _rightEdge.setTexture(spriteSheet);
    _rightTopCorner.setTexture(spriteSheet);
    _rightBottomCorner.setTexture(spriteSheet);
}

void UIMessageDisplay::update() {
}

void UIMessageDisplay::draw(sf::RenderTexture& surface) {
    if (!MessageManager::isPurging()) {
        int lastHeight = getRelativeHeight(3.f);
        for (auto& message : MessageManager::getMessages()) {
            if (message->active && !((message->messageType == DEBUG || message->messageType == WARN || message->messageType == ERR) && !DISPLAY_DEBUG_MESSAGES)) {
                const bool special = message->messageType == SPECIAL;
                const bool tutorial = message->messageType == TUTORIAL;

                sf::Text messageText(message->text, _font);
                const float fontSize = 1.25f;
                int relativeFontSize = getRelativeWidth(fontSize);
                messageText.setCharacterSize(relativeFontSize);
                messageText.setFillColor(sf::Color(MESSAGE_TYPES[message->messageType].color));

                float width = messageText.getGlobalBounds().width;
                float height = messageText.getGlobalBounds().height;

                float x = getRelativeWidth(50.f) - width / 2;
                float y = getRelativeHeight(1.f) + lastHeight;
                messageText.setPosition(sf::Vector2f(x, y - getRelativeHeight(0.5f)));

                const float padding = getRelativeWidth(1.5f);
                x -= padding;
                width += padding * 2;

                const bool dbg = message->messageType == DEBUG || message->messageType == WARN || message->messageType == ERR;
                const int specialOffsetAmt = dbg ? 96 : 48;
                const int specialOffset = special || dbg ? specialOffsetAmt : 0;

                const float pixelSize = getRelativeWidth(0.3f);

                _leftEdge.setSize({ pixelSize * 3.f, height });
                _leftEdge.setPosition(x, y);
                _leftEdge.setTextureRect({ 112 + specialOffset, 339, 3, 9 });

                _leftTopCorner.setSize({ pixelSize * 2.f, pixelSize * 2.f });
                _leftTopCorner.setPosition(x + pixelSize * 1.f, y - pixelSize * 2.f);
                _leftTopCorner.setTextureRect({ 113 + specialOffset, 321, 2, 2 });

                _leftBottomCorner.setSize({ pixelSize * 3.f, pixelSize * 3.f });
                _leftBottomCorner.setPosition(x, y + _leftEdge.getGlobalBounds().height);
                _leftBottomCorner.setTextureRect({ 112 + specialOffset, 332, 3, 3 });

                _center.setSize({ width - _leftEdge.getGlobalBounds().width, height });
                _center.setPosition(x + _leftEdge.getGlobalBounds().width, y);
                _center.setTextureRect({ 96 + specialOffset, 339, 1, 9 });

                _centerTop.setSize({ width - _leftTopCorner.getGlobalBounds().width, pixelSize * 2.f });
                _centerTop.setPosition(_leftTopCorner.getGlobalBounds().width + _leftTopCorner.getPosition().x, y - pixelSize * 2.f);
                _centerTop.setTextureRect({ 96 + specialOffset, 321, 1, 2 });

                _centerBottom.setSize({ width - _leftEdge.getGlobalBounds().width, pixelSize * 3.f });
                _centerBottom.setPosition(x + _leftEdge.getGlobalBounds().width, y + _center.getGlobalBounds().height);
                _centerBottom.setTextureRect({ 96 + specialOffset, 332, 1, 3 });

                _rightEdge.setSize({ pixelSize * 3.f, height });
                _rightEdge.setPosition(x + _leftEdge.getGlobalBounds().width + _center.getGlobalBounds().width, y);
                _rightEdge.setTextureRect({ 128 + specialOffset, 339, 3, 9 });

                _rightTopCorner.setSize({ pixelSize * 2.f, pixelSize * 2.f });
                _rightTopCorner.setPosition(x + _leftEdge.getGlobalBounds().width + _center.getGlobalBounds().width, y - pixelSize * 2.f);
                _rightTopCorner.setTextureRect({ 128 + specialOffset, 321, 2, 2 });

                _rightBottomCorner.setSize({ pixelSize * 3.f, pixelSize * 3.f });
                _rightBottomCorner.setPosition(x + _leftEdge.getGlobalBounds().width + _center.getGlobalBounds().width, y + _rightEdge.getGlobalBounds().height);
                _rightBottomCorner.setTextureRect({ 128 + specialOffset, 332, 3, 3 });

                const float gapCorrectionOffset = 1.f; //(center.getPosition().x - (leftEdge.getPosition().x + leftEdge.getGlobalBounds().width)) * 100.f;
                _leftEdge.move(gapCorrectionOffset, 0);
                _leftTopCorner.move(gapCorrectionOffset, 0);
                _leftBottomCorner.move(gapCorrectionOffset, 0);

                lastHeight = y + _centerTop.getGlobalBounds().height + _center.getGlobalBounds().height + _centerBottom.getGlobalBounds().height;

                surface.draw(_leftEdge);
                surface.draw(_leftTopCorner);
                surface.draw(_leftBottomCorner);
                surface.draw(_center);
                surface.draw(_centerTop);
                surface.draw(_centerBottom);
                surface.draw(_rightEdge);
                surface.draw(_rightTopCorner);
                surface.draw(_rightBottomCorner);

                surface.draw(messageText);
            }
        }
    }
}

void UIMessageDisplay::mouseButtonPressed(const int mx, const int my, const int button) {
}

void UIMessageDisplay::mouseButtonReleased(const int mx, const int my, const int button) {
}

void UIMessageDisplay::mouseMoved(const int mx, const int my) {
}

void UIMessageDisplay::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
}

void UIMessageDisplay::keyReleased(sf::Keyboard::Key& key) {
}

void UIMessageDisplay::textEntered(const sf::Uint32 character) {
}

