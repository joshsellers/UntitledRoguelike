#include "UIButton.h"
#include <thread>
#include <iostream>
#include "UIHandler.h"

UIButton::UIButton(float x, float y, float width, float height, sf::String labelText, sf::Font font,
    UIButtonListener* listener, std::string buttonCode)
    : UIElement(
        x, y, width, height, true, true, font
    ) {

    _rTexture.create(_width, 16 * 3);
    sf::RectangleShape center;
    center.setPosition(16, 1);
    center.setSize(sf::Vector2f(_width - 16 * 2, 14));
    center.setTexture(UIHandler::getUISpriteSheet().get());
    center.setTextureRect(sf::IntRect(0, 1, 1, 14));
    _rTexture.draw(center);
    center.setPosition(16, 17);
    _rTexture.draw(center);
    center.setPosition(16, 34);
    _rTexture.draw(center);

    for (int i = 0; i < 3; i++) {
        sf::RectangleShape borderLeft;
        borderLeft.setPosition(0, 16 * i);
        borderLeft.setSize(sf::Vector2f(16, 16));
        borderLeft.setTexture(UIHandler::getUISpriteSheet().get());
        borderLeft.setTextureRect(sf::IntRect(16 + 32 * i, 0, 4, 16));
        _rTexture.draw(borderLeft);
    }

    for (int i = 0; i < 3; i++) {
        sf::RectangleShape borderRight;
        borderRight.setPosition(_width - 16, 16 * i);
        borderRight.setSize(sf::Vector2f(16, 16));
        borderRight.setTexture(UIHandler::getUISpriteSheet().get());
        borderRight.setTextureRect(sf::IntRect(32 + 32 * i, 0, 4, 16));
        _rTexture.draw(borderRight);
    }

    _rTexture.display();

    _defaultTexture = sf::IntRect(0, 0, _width, 16);
    _hoverTexture = sf::IntRect(0, 16, _width, 16);
    _clickTexture = sf::IntRect(0, 32, _width, 16);

    _shape.setSize(sf::Vector2f(_width, _height));
    _shape.setTexture(&_rTexture.getTexture());
    _shape.setTextureRect(sf::IntRect(0, 0, _width, 16));

    float fontSize = 1.5;
    int relativeFontSize = (float)WINDOW_WIDTH * (fontSize / 100);
    _text.setFont(_font);
    _text.setCharacterSize(relativeFontSize);
    _text.setFillColor(sf::Color::White);
    _text.setString(labelText);

    _buttonCode = buttonCode;
    _listener = listener;
}

void UIButton::update() {
    sf::FloatRect bounds = _shape.getGlobalBounds();
    if (!_mouseDown && (bounds.contains(_mx, _my) || _isSelected)) {
        _shape.setTextureRect(_hoverTexture);
    } else if (bounds.contains(_mx, _my) || (_mouseDown && _isSelected)) {
        _shape.setTextureRect(_clickTexture);
    } else {
        _shape.setTextureRect(_defaultTexture);
    }

    if (!USING_MOUSE && !_mouseDown && !_isSelected) _shape.setTextureRect(_defaultTexture);

    if (pressWhenSelected && _isSelected && !_wasJustSelected) {
        _listener->buttonPressed(_buttonCode);
    }
    _wasJustSelected = _isSelected;
}

void UIButton::draw(sf::RenderTexture& surface) {}

void UIButton::setLabelText(std::string labelText) {
    _text.setString(labelText);
}

void UIButton::controllerButtonPressed(GAMEPAD_BUTTON button) {
    if (_isSelected && button == GAMEPAD_BUTTON::A && !pressWhenSelected) _mouseDown = true;
}

void UIButton::controllerButtonReleased(GAMEPAD_BUTTON button) {
    if (_isSelected && button == GAMEPAD_BUTTON::A && !pressWhenSelected) {
        _mouseDown = false;
        _listener->buttonPressed(_buttonCode);
    }
}

void UIButton::mouseButtonPressed(const int mx, const int my, const int button) {
    _mouseDown = true;
}

void UIButton::mouseButtonReleased(const int mx, const int my, const int button) {
    _mouseDown = false;
    sf::FloatRect bounds = _shape.getGlobalBounds();
    if (bounds.contains(mx, my)) {
        /*std::thread callbackThread(
            &UIButtonListener::buttonPressed, _listener, _buttonCode
        );
        callbackThread.detach();*/
        _listener->buttonPressed(_buttonCode);
    }
}

void UIButton::mouseMoved(const int mx, const int my) {
    _mx = mx;
    _my = my;
}

void UIButton::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
}

void UIButton::textEntered(const sf::Uint32 character) {}

void UIButton::show() {
    _isActive = true;
    _mouseDown = false;
}

sf::IntRect UIButton::getHoverTexture() {
    return _hoverTexture;
}

sf::IntRect UIButton::getClickTexture() {
    return _clickTexture;
}

sf::IntRect UIButton::getDefaultTexture() {
    return _defaultTexture;
}

sf::Vector2i UIButton::getMousePos() {
    return sf::Vector2i(_mx, _my);
}
