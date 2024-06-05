#include "UIElement.h"

sf::Vector2f UIElement::getRelativePos(sf::Vector2f pos) {
    return sf::Vector2f((float)WINDOW_WIDTH * (pos.x / 100), (float)WINDOW_HEIGHT * (pos.y / 100));
}

sf::Vector2f UIElement::getRelativePos(float x, float y) {
    return getRelativePos(sf::Vector2f(x, y));
}

float UIElement::getRelativeWidth(float size) {
    return (float)WINDOW_WIDTH * (size / 100.f);
}

float UIElement::getRelativeHeight(float size) {
    return (float)WINDOW_HEIGHT * (size / 100.f);
}

UIElement::UIElement(float x, float y, float width, float height,
    bool drawSprite, bool drawText, sf::Font font) {
    _x = x;
    _y = y;
    _drawSprite = drawSprite;
    _drawText = drawText;

    _font = font;

    if (drawSprite) _sprite.setPosition(getRelativePos(sf::Vector2f(_x, _y)));

    _width = WINDOW_WIDTH * (width / 100);
    _height = WINDOW_WIDTH * (height / 100);
}

void UIElement::render(sf::RenderTexture& surface) {
    if (!_disableAutomaticTextAlignment) {
        _text.setPosition(
            _sprite.getPosition().x + _sprite.getGlobalBounds().width / 2 - _text.getGlobalBounds().width / 2,
            _sprite.getPosition().y + _text.getGlobalBounds().height / 2
        );
    }
    if (_drawSprite) surface.draw(_sprite);
    if (_drawText) surface.draw(_text);

    draw(surface);
}

void UIElement::controllerButtonReleased(GAMEPAD_BUTTON button) {}

void UIElement::controllerButtonPressed(GAMEPAD_BUTTON button) {
}

void UIElement::keyPressed(sf::Keyboard::Key& key) {}

void UIElement::keyReleased(sf::Keyboard::Key& key) {}

void UIElement::show() {
    _isActive = true;
}

void UIElement::hide() {
    _isActive = false;
}

bool UIElement::isActive() const {
    return _isActive;
}

void UIElement::setSelectionId(int selectionId) {
    _selectionId = selectionId;
}

int UIElement::getSelectionId() {
    return _selectionId;
}

void UIElement::setCharacterSize(float size) {
    _text.setCharacterSize(getRelativeWidth(size));
}
