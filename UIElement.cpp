#include "UIElement.h"

UIElement::UIElement(float x, float y, float width, float height, 
    bool drawSprite, bool drawText, sf::Font font) {
    _x = x;
    _y = y;
    _drawSprite = drawSprite;
    _drawText = drawText;

    _font = font;

    int relativeX = (float)WINDOW_WIDTH * (_x / 100);
    int relativeY = (float)WINDOW_HEIGHT * (_y / 100);
    _sprite.setPosition(relativeX, relativeY);

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
