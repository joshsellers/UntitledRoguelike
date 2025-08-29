#include "UIMessageDisplay.h"
#include "../core/MessageManager.h"
#include <iostream>
#include "UIHandler.h"

UIMessageDisplay::UIMessageDisplay(sf::Font font) : UIElement(30, 30 , 4, 4, false, false, font) {}

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

                const float pixelSize = getRelativeWidth(0.5f);
                x -= padding;
                width += padding * 2;

                const bool dbg = message->messageType == DEBUG || message->messageType == WARN || message->messageType == ERR;
                const int specialOffsetAmt = dbg ? 96 : 48;
                const int specialOffset = special || dbg ? specialOffsetAmt : 0;

                sf::RectangleShape leftEdge;
                leftEdge.setSize({ pixelSize * 3.f, height });
                leftEdge.setPosition(x, y);
                leftEdge.setTexture(UIHandler::getUISpriteSheet().get());
                leftEdge.setTextureRect({ 112 + specialOffset, 339, 3, 9 });

                sf::RectangleShape leftTopCorner;
                leftTopCorner.setSize({ pixelSize * 2.f, pixelSize * 2.f });
                leftTopCorner.setPosition(x + pixelSize * 1.f, y - pixelSize * 2.f);
                leftTopCorner.setTexture(UIHandler::getUISpriteSheet().get());
                leftTopCorner.setTextureRect({ 113 + specialOffset, 321, 2, 2 });

                sf::RectangleShape leftBottomCorner;
                leftBottomCorner.setSize({ pixelSize * 3.f, pixelSize * 3.f });
                leftBottomCorner.setPosition(x, y + leftEdge.getGlobalBounds().height);
                leftBottomCorner.setTexture(UIHandler::getUISpriteSheet().get());
                leftBottomCorner.setTextureRect({ 112 + specialOffset, 332, 3, 3 });

                sf::RectangleShape center;
                center.setSize({ width - leftEdge.getGlobalBounds().width, height });
                center.setPosition(x + leftEdge.getGlobalBounds().width, y);
                center.setTexture(UIHandler::getUISpriteSheet().get());
                center.setTextureRect({ 96 + specialOffset, 339, 1, 9 });

                sf::RectangleShape centerTop;
                centerTop.setSize({ width - leftTopCorner.getGlobalBounds().width, pixelSize * 2.f });
                centerTop.setPosition(leftTopCorner.getGlobalBounds().width + leftTopCorner.getPosition().x, y - pixelSize * 2.f);
                centerTop.setTexture(UIHandler::getUISpriteSheet().get());
                centerTop.setTextureRect({ 96 + specialOffset, 321, 1, 2 });

                sf::RectangleShape centerBottom;
                centerBottom.setSize({ width - leftEdge.getGlobalBounds().width, pixelSize * 3.f });
                centerBottom.setPosition(x + leftEdge.getGlobalBounds().width, y + center.getGlobalBounds().height);
                centerBottom.setTexture(UIHandler::getUISpriteSheet().get());
                centerBottom.setTextureRect({ 96 + specialOffset, 332, 1, 3 });

                sf::RectangleShape rightEdge;
                rightEdge.setSize({ pixelSize * 3.f, height });
                rightEdge.setPosition(x + leftEdge.getGlobalBounds().width + center.getGlobalBounds().width, y);
                rightEdge.setTexture(UIHandler::getUISpriteSheet().get());
                rightEdge.setTextureRect({ 128 + specialOffset, 339, 3, 9 });

                sf::RectangleShape rightTopCorner;
                rightTopCorner.setSize({ pixelSize * 2.f, pixelSize * 2.f });
                rightTopCorner.setPosition(x + leftEdge.getGlobalBounds().width + center.getGlobalBounds().width, y - pixelSize * 2.f);
                rightTopCorner.setTexture(UIHandler::getUISpriteSheet().get());
                rightTopCorner.setTextureRect({ 128 + specialOffset, 321, 2, 2 });

                sf::RectangleShape rightBottomCorner;
                rightBottomCorner.setSize({ pixelSize * 3.f, pixelSize * 3.f });
                rightBottomCorner.setPosition(x + leftEdge.getGlobalBounds().width + center.getGlobalBounds().width, y + rightEdge.getGlobalBounds().height);
                rightBottomCorner.setTexture(UIHandler::getUISpriteSheet().get());
                rightBottomCorner.setTextureRect({ 128 + specialOffset, 332, 3, 3 });

                const float offset = 1.f; //(center.getPosition().x - (leftEdge.getPosition().x + leftEdge.getGlobalBounds().width)) * 100.f;
                leftEdge.move(offset, 0);
                leftTopCorner.move(offset, 0);
                leftBottomCorner.move(offset, 0);

                lastHeight = y + centerTop.getGlobalBounds().height + center.getGlobalBounds().height + centerBottom.getGlobalBounds().height;

                surface.draw(leftEdge);
                surface.draw(leftTopCorner);
                surface.draw(leftBottomCorner);
                surface.draw(center);
                surface.draw(centerTop);
                surface.draw(centerBottom);
                surface.draw(rightEdge);
                surface.draw(rightTopCorner);
                surface.draw(rightBottomCorner);

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

