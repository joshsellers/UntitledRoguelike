#include "UIElement.h"

sf::Vector2f UIElement::getRelativePos(sf::Vector2f pos) {
    return sf::Vector2f((float)WINDOW_WIDTH * (pos.x / 100), (float)WINDOW_HEIGHT * (pos.y / 100));
}

float UIElement::getRelativeSize(float size) {
    return (float)WINDOW_WIDTH * (size / 100);
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
            _sprite.getPosition().x + _sprite.getGlobalBounds().width / 12,
            _sprite.getPosition().y + _sprite.getGlobalBounds().height / 6
        );
    }
    if (_drawSprite) surface.draw(_sprite);
    if (_drawText) surface.draw(_text);

    draw(surface);
}

void UIElement::show() {
    _isActive = true;
}

void UIElement::hide() {
    _isActive = false;
}

bool UIElement::isActive() const {
    return _isActive;
}
