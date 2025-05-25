#include "UILabel.h"
#include "../core/Util.h"
#include "../core/MessageManager.h"

UILabel::UILabel(const sf::String label, float x, float y, float characterSize, sf::Font font, float imageWidth, float imageHeight, bool square) 
    : UIElement(x, y, 10, 10, false, true, font) {
    _text.setFont(_font);
    _text.setString(label);
    _text.setCharacterSize(getRelativeWidth(characterSize));

    _x = getRelativePos(x, 0).x;
    _y = getRelativePos(0, y).y;

    if (stringStartsWith(_text.getString(), "IMAGE")) {
        if (!_spriteSheet->loadFromFile(splitString(_text.getString(), ":")[1])) {
            MessageManager::displayMessage(_text.getString() + " could not be loaded", 5, WARN);
        } else {
            _shape.setTexture(_spriteSheet.get(), true);
            _shape.setSize(sf::Vector2f(_shape.getTextureRect().width, _shape.getTextureRect().height));
            sf::Vector2f imageScale(
                getRelativeWidth(imageWidth) / _shape.getGlobalBounds().width, 
                square ? getRelativeWidth(imageHeight) / _shape.getGlobalBounds().height : getRelativeHeight(imageHeight) / _shape.getGlobalBounds().height
            );
            _shape.setScale(imageScale);
            _shape.setPosition(_x, _y);
            _drawSprite = true;
            _drawText = false;
        }
    } else {
        _width = _text.getGlobalBounds().width;

        _text.setPosition(_x - _width / 2.f, _y);
    }

    _disableAutomaticTextAlignment = true;
}

void UILabel::update() {
}

void UILabel::draw(sf::RenderTexture& surface) {
}

void UILabel::setText(sf::String text, bool recenter) {
    _text.setString(text);
    if (recenter) {
        _width = _text.getGlobalBounds().width;

        _text.setPosition(_x - _width / 2.f, _y);
    }
}

void UILabel::setTextOutlineThickness(float thickness) {
    _text.setOutlineThickness(thickness);
}

void UILabel::mouseButtonPressed(const int mx, const int my, const int button) {
}

void UILabel::mouseButtonReleased(const int mx, const int my, const int button) {
}

void UILabel::controllerButtonPressed(GAMEPAD_BUTTON button) {
}

void UILabel::controllerButtonReleased(GAMEPAD_BUTTON button) {
}

void UILabel::mouseMoved(const int mx, const int my) {
}

void UILabel::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
}

void UILabel::keyReleased(sf::Keyboard::Key& key) {
}

void UILabel::textEntered(const sf::Uint32 character) {
}
