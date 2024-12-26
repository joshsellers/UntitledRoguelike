#include "UISlider.h"
#include "UIButton.h"
#include "../core/MessageManager.h"
#include "../core/SoundManager.h"
#include "../core/gamepad/GamePad.h"
#include "UIHandler.h"

UISlider::UISlider(const std::string label, float x, float y, float width, sf::Font font, std::string id) : UIElement(x, y, width, 5, false, true, font) {
    _x = getRelativeWidth(x);
    _y = getRelativeHeight(y);
    _id = id;

    float fontSize = 1;
    int relativeFontSize = getRelativeWidth(fontSize);
    _text.setString(label);
    _text.setFont(_font);
    _text.setCharacterSize(relativeFontSize);
    _text.setFillColor(sf::Color(0xFFFFFFFF));
    _text.setPosition(_x, _y - getRelativeHeight(4.f));
    _disableAutomaticTextAlignment = true;

    const float handleWidth = getRelativeWidth(1.5);
    const float height = getRelativeHeight(4);
    _rTexture.create(handleWidth, 16 * 3);
    sf::RectangleShape center;
    center.setPosition(16, 1);
    center.setSize(sf::Vector2f(handleWidth - 16 * 2, 14));
    center.setTexture(UIHandler::getUISpriteSheet().get());
    center.setTextureRect(sf::IntRect(0, 17, 1, 14));
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
        borderLeft.setTextureRect(sf::IntRect(16 + 32 * i, 16, 4, 16));
        _rTexture.draw(borderLeft);
    }

    for (int i = 0; i < 3; i++) {
        sf::RectangleShape borderRight;
        borderRight.setPosition(handleWidth - 16, 16 * i);
        borderRight.setSize(sf::Vector2f(16, 16));
        borderRight.setTexture(UIHandler::getUISpriteSheet().get());
        borderRight.setTextureRect(sf::IntRect(32 + 32 * i, 16, 4, 16));
        _rTexture.draw(borderRight);
    }

    _rTexture.display();

    _defaultTexture = sf::IntRect(0, 0, handleWidth, 16);
    _hoverTexture = sf::IntRect(0, 16, handleWidth, 16);
    _clickTexture = sf::IntRect(0, 32, handleWidth, 16);

    _shape.setSize(sf::Vector2f(handleWidth, height));
    _shape.setTexture(&_rTexture.getTexture());
    _shape.setTextureRect(sf::IntRect(0, 0, handleWidth, 16));
    _shape.setPosition(sf::Vector2f(_x, _y - getRelativeHeight(1.5f)));

    _bar.setSize(getRelativePos(width, 1));
    _bar.setPosition(sf::Vector2f(_x, _y));
    _bar.setFillColor(sf::Color::Black);
}

void UISlider::update() {
    if (_isSelected && GamePad::isConnected()) {
        if (GamePad::isButtonPressed(GAMEPAD_BUTTON::DPAD_LEFT)) {
            setValue(_value - 0.01);
            if (_value < 0) setValue(0);
        } else if (GamePad::isButtonPressed(GAMEPAD_BUTTON::DPAD_RIGHT)) {
            setValue(_value + 0.01);
            if (_value > 1) setValue(1);
        }
    }

    const bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left);

    if (((_shape.getGlobalBounds().contains(_mPos.x, _mPos.y) && !mouseDown))) {
        _shape.setTextureRect(_hoverTexture);
    } else if ((!mouseDown && !_shape.getGlobalBounds().contains(_mPos.x, _mPos.y) && !_isSelected)) {
        _shape.setTextureRect(_defaultTexture);
    } else if ((_mouseSelected) || _isSelected) {
        _shape.setTextureRect(_clickTexture);
    }

    if (_mouseSelected) {
        const float handleX = _shape.getPosition().x;
        float newHandleX = _mPos.x - _shape.getGlobalBounds().width / 2;
        if (newHandleX + _shape.getGlobalBounds().width / 2 < _x) newHandleX = _x - _shape.getGlobalBounds().width / 2;
        else if (newHandleX + _shape.getGlobalBounds().width / 2 > _x + _width) newHandleX = _x + _width - _shape.getGlobalBounds().width / 2;
        _shape.setPosition(newHandleX, _shape.getPosition().y);

        _value = std::min((_shape.getPosition().x + _shape.getGlobalBounds().width / 2 - _x) / _width, 1.f);
    }

    if (_value != _lastValue) {
        _listener->buttonPressed(_id);
        _lastValue = _value;
    }
}

void UISlider::draw(sf::RenderTexture& surface) {
    surface.draw(_bar);
    surface.draw(_shape);
}

void UISlider::mouseButtonPressed(const int mx, const int my, const int button) {
    _mouseDown = true;
    if (_shape.getGlobalBounds().contains(_mPos.x, _mPos.y)) _mouseSelected = true;
}

void UISlider::mouseButtonReleased(const int mx, const int my, const int button) {
    _mouseDown = false;

    if (_mouseSelected && _id == "sfxslider") SoundManager::playSound("coinpickup");

    _mouseSelected = false;
}

void UISlider::controllerButtonPressed(GAMEPAD_BUTTON button) {
}

void UISlider::controllerButtonReleased(GAMEPAD_BUTTON button) {
    if (_id == "sfxslider" && _isSelected && (button == GAMEPAD_BUTTON::DPAD_LEFT || button == GAMEPAD_BUTTON::DPAD_RIGHT)) {
        SoundManager::playSound("coinpickup");
    }
}

void UISlider::mouseMoved(const int mx, const int my) {
    _mPos = sf::Vector2i(mx, my);
}

void UISlider::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
}

void UISlider::keyReleased(sf::Keyboard::Key& key) {
}

void UISlider::textEntered(const sf::Uint32 character) {
}

void UISlider::setValue(float value) {
    _value = value;
    _shape.setPosition(_x + _width * value - _shape.getGlobalBounds().width / 2, _shape.getPosition().y);
}

float UISlider::getValue() const {
    return _value;
}

const std::string UISlider::getId() const {
    return _id;
}

void UISlider::setListener(UIButtonListener* listener) {
    _listener = listener;
}
