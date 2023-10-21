#include "UITextField.h"
#include "UIButton.h"

UITextField::UITextField(const sf::String label, float x, float y, sf::Font font) 
    : UIElement(x, y, 1, 1, false, false, font) {
    _disableAutomaticTextAlignment = true;

    _bg.setSize(sf::Vector2f(_width, _height));
    _bg.setPosition(sf::Vector2f(_x, _y));
    _bg.setFillColor(sf::Color(BUTTON_COLOR[0], BUTTON_COLOR[1], BUTTON_COLOR[2], 0xFF));
    _bg.setOutlineColor(sf::Color(BUTTON_COLOR_BORDER[0], BUTTON_COLOR_BORDER[1], BUTTON_COLOR_BORDER[2], 0xFF));
    _bg.setOutlineThickness(getRelativeWidth(0.25f));

    float fontSize = 1;
    int relativeFontSize = getRelativeWidth(fontSize);
    _text.setFont(_font);
    _text.setCharacterSize(relativeFontSize);
    _text.setFillColor(sf::Color(0xFFFFFFFF));

    _label.setString(label);
    _label.setFont(_font);
    _label.setCharacterSize(relativeFontSize);
    _label.setFillColor(sf::Color(0xFFFFFFFF));
}

void UITextField::update() {
    if (!_isSelected && _lastSelected) _isArmed = false;
    _lastSelected = _isSelected;
}

void UITextField::draw(sf::RenderTexture& surface) {
    if (((_bg.getGlobalBounds().contains(_mPos.x, _mPos.y) && !_mouseDown) || _isSelected) && !_isArmed) {
        _bg.setFillColor(sf::Color(BUTTON_COLOR[0], BUTTON_COLOR[1], BUTTON_COLOR[2], 0xFF));
        _bg.setOutlineColor(sf::Color(BUTTON_COLOR_BORDER_HOVER[0], BUTTON_COLOR_BORDER_HOVER[1], BUTTON_COLOR_BORDER_HOVER[2], 0xFF));
    } else if ((!_mouseDown || !_bg.getGlobalBounds().contains(_mPos.x, _mPos.y)) && !_isArmed) {
        _bg.setFillColor(sf::Color(BUTTON_COLOR[0], BUTTON_COLOR[1], BUTTON_COLOR[2], 0xFF));
        _bg.setOutlineColor(sf::Color(BUTTON_COLOR_BORDER[0], BUTTON_COLOR_BORDER[1], BUTTON_COLOR_BORDER[2], 0xFF));
    } else if (_isArmed) {
        _bg.setFillColor(sf::Color(BUTTON_COLOR_CLICK[0], BUTTON_COLOR_CLICK[1], BUTTON_COLOR_CLICK[2], 0xFF));
        _bg.setOutlineColor(sf::Color(BUTTON_COLOR_CLICK[0], BUTTON_COLOR_CLICK[1], BUTTON_COLOR_CLICK[2], 0xFF));
    }

    float width = _text.getGlobalBounds().width;
    float height = _text.getGlobalBounds().height;

    float x = getRelativeWidth(_x) - width / 2;
    float y = getRelativeHeight(_y);
    _text.setPosition(sf::Vector2f(x, y));
    _label.setPosition(sf::Vector2f(getRelativeWidth(_x) - (_label.getGlobalBounds().width / 2), y - _bg.getGlobalBounds().height));

    float padding = getRelativeWidth(1.f);
    _bg.setPosition(sf::Vector2f(x - padding, y - padding));
    _bg.setSize(sf::Vector2f(width + padding * 2, height + padding * 2));

    surface.draw(_bg);
    surface.draw(_text);
    surface.draw(_label);
}

void UITextField::setDefaultText(sf::String text) {
    _text.setString(text);
}

void UITextField::mouseButtonPressed(const int mx, const int my, const int button) {
    _mouseDown = true;
}

void UITextField::mouseButtonReleased(const int mx, const int my, const int button) {
    _isArmed = _bg.getGlobalBounds().contains(mx, my);
    _mouseDown = false;
}

void UITextField::controllerButtonPressed(CONTROLLER_BUTTON button) {
    if (_isSelected && button == CONTROLLER_BUTTON::A) _mouseDown = true;
}

void UITextField::controllerButtonReleased(CONTROLLER_BUTTON button) {
    if (_isSelected && button == CONTROLLER_BUTTON::A) {
        _mouseDown = false;
        _isArmed = true;
    }
}

void UITextField::mouseMoved(const int mx, const int my) {
    _mPos.x = mx;
    _mPos.y = my;
}

void UITextField::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
}

void UITextField::keyReleased(sf::Keyboard::Key& key) {
}

void UITextField::textEntered(const sf::Uint32 character) {
    if (_isArmed) {
        sf::String userInput = _text.getString();
        if (character == '\b' && userInput.getSize()) {
            userInput.erase(userInput.getSize() - 1, 1);
        } else if (character != '\b') {
            userInput += character;
        }
        _text.setString(userInput);
    }
}

std::string UITextField::getText() const {
    return ((std::string)_text.getString());
}

void UITextField::setCharacterSize(float size) {
    _text.setCharacterSize(getRelativeWidth(size));
    _label.setCharacterSize(getRelativeWidth(size));
}
