#include "UIMessageDisplay.h"
#include "../core/MessageManager.h"
#include <iostream>

UIMessageDisplay::UIMessageDisplay(sf::Font font) : UIElement(30, 30 , 4, 4, false, false, font) {}

void UIMessageDisplay::update() {
}

void UIMessageDisplay::draw(sf::RenderTexture& surface) {
    if (!MessageManager::isPurging()) {
        int lastHeight = getRelativeHeight(3.f);
        for (auto& message : MessageManager::getMessages()) {
            if (message->active && !(message->messageType == DEBUG && !DISPLAY_DEBUG_MESSAGES)) {
                const bool special = message->messageType == SPECIAL;

                sf::Text messageText(message->text, _font);
                float fontSize = special ? 2.f : 1.f;
                int relativeFontSize = getRelativeWidth(fontSize);
                messageText.setCharacterSize(relativeFontSize);
                messageText.setFillColor(sf::Color(MESSAGE_TYPES[message->messageType].color));

                float width = messageText.getGlobalBounds().width;
                float height = messageText.getGlobalBounds().height;

                float x = getRelativeWidth(50.f) - width / 2;
                float y = getRelativeHeight(1.5f) + lastHeight + (special ? getRelativeHeight(0.25f) : 0.f);
                messageText.setPosition(sf::Vector2f(x, y));

                sf::RectangleShape background; 
                background.setFillColor(sf::Color(special ? 0xD1B000FF : 0x2D2D3099));
                background.setOutlineColor(sf::Color(special ? 0xA38A00FF : 0x3E3E4199));
                background.setOutlineThickness(getRelativeWidth(0.25f));

                float padding = getRelativeWidth(special ? 0.5f : 0.25f);
                background.setPosition(sf::Vector2f(x - padding, y - padding));
                background.setSize(sf::Vector2f(width + padding * 2, height + padding * 2));
                lastHeight = y - padding + height + padding * 2;

                surface.draw(background);
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

