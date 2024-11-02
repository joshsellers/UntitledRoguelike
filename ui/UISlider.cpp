#include "UISlider.h"
#include "UIButton.h"
#include "../core/MessageManager.h"
#include "../core/SoundManager.h"
#include "../core/gamepad/GamePad.h"

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

    _handle.setSize(getRelativePos(1.5, 4));
    _handle.setPosition(sf::Vector2f(_x, _y - getRelativeHeight(1.5f)));
    _handle.setFillColor(sf::Color(BUTTON_COLOR[0], BUTTON_COLOR[1], BUTTON_COLOR[2], 0xFF));
    _handle.setOutlineColor(sf::Color(BUTTON_COLOR_BORDER[0], BUTTON_COLOR_BORDER[1], BUTTON_COLOR_BORDER[2], 0xFF));
    _handle.setOutlineThickness(getRelativeWidth(0.25f));

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

    if (((_handle.getGlobalBounds().contains(_mPos.x, _mPos.y) && !mouseDown))) {
        _handle.setFillColor(sf::Color(BUTTON_COLOR[0], BUTTON_COLOR[1], BUTTON_COLOR[2], 0xFF));
        _handle.setOutlineColor(sf::Color(BUTTON_COLOR_BORDER_HOVER[0], BUTTON_COLOR_BORDER_HOVER[1], BUTTON_COLOR_BORDER_HOVER[2], 0xFF));
    } else if ((!mouseDown && !_handle.getGlobalBounds().contains(_mPos.x, _mPos.y) && !_isSelected)) {
        _handle.setFillColor(sf::Color(BUTTON_COLOR[0], BUTTON_COLOR[1], BUTTON_COLOR[2], 0xFF));
        _handle.setOutlineColor(sf::Color(BUTTON_COLOR_BORDER[0], BUTTON_COLOR_BORDER[1], BUTTON_COLOR_BORDER[2], 0xFF));
    } else if ((_mouseSelected) || _isSelected) {
        _handle.setFillColor(sf::Color(BUTTON_COLOR_CLICK[0], BUTTON_COLOR_CLICK[1], BUTTON_COLOR_CLICK[2], 0xFF));
        _handle.setOutlineColor(sf::Color(BUTTON_COLOR_CLICK[0], BUTTON_COLOR_CLICK[1], BUTTON_COLOR_CLICK[2], 0xFF));
    }

    if (_mouseSelected) {
        const float handleX = _handle.getPosition().x;
        float newHandleX = _mPos.x - _handle.getGlobalBounds().width / 2;
        if (newHandleX + _handle.getGlobalBounds().width / 2 < _x) newHandleX = _x - _handle.getGlobalBounds().width / 2;
        else if (newHandleX + _handle.getGlobalBounds().width / 2 > _x + _width) newHandleX = _x + _width - _handle.getGlobalBounds().width / 2;
        _handle.setPosition(newHandleX, _handle.getPosition().y);

        _value = std::min((_handle.getPosition().x + _handle.getGlobalBounds().width / 2 - _x) / _width, 1.f);
    }

    if (_value != _lastValue) {
        _listener->buttonPressed(_id);
        _lastValue = _value;
    }
}

void UISlider::draw(sf::RenderTexture& surface) {
    surface.draw(_bar);
    surface.draw(_handle);
}

void UISlider::mouseButtonPressed(const int mx, const int my, const int button) {
    _mouseDown = true;
    if (_handle.getGlobalBounds().contains(_mPos.x, _mPos.y)) _mouseSelected = true;
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
    _handle.setPosition(_x + _width * value - _handle.getGlobalBounds().width / 2, _handle.getPosition().y);
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
