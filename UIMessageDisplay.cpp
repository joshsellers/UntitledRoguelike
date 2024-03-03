#include "UIMessageDisplay.h"
#include "MessageManager.h"
#include <iostream>

UIMessageDisplay::UIMessageDisplay(sf::Font font) : UIElement(30, 30 , 4, 4, false, false, font) {}

void UIMessageDisplay::update() {
}

void UIMessageDisplay::draw(sf::RenderTexture& surface) {
    int index = 0;
    for (auto& message : MessageManager::getMessages()) {
        if (message->active) {
            sf::Text messageText(message->text, _font);
            float fontSize = 1.f;
            int relativeFontSize = getRelativeWidth(fontSize);
            messageText.setCharacterSize(relativeFontSize);
            messageText.setFillColor(sf::Color(MESSAGE_TYPES[message->messageType].color));

            float width = messageText.getGlobalBounds().width;
            float height = messageText.getGlobalBounds().height;

            float x = getRelativeWidth(50.f) - width / 2;
            float y = getRelativeHeight(3.f) + (MessageManager::getMessageCount() - index) * getRelativeWidth(2.f);
            messageText.setPosition(sf::Vector2f(x, y));

            sf::RectangleShape background;
            background.setFillColor(sf::Color(0x2D2D3099));
            background.setOutlineColor(sf::Color(0x3E3E4199));
            background.setOutlineThickness(getRelativeWidth(0.25f));

            float padding = getRelativeWidth(0.25f);
            background.setPosition(sf::Vector2f(x - padding, y - padding));
            background.setSize(sf::Vector2f(width + padding * 2, height + padding * 2));

            surface.draw(background);
            surface.draw(messageText);

            index++;
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

