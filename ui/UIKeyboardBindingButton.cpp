#include "UIKeyboardBindingButton.h"

UIKeyboardBindingButton::UIKeyboardBindingButton(float x, float y, float width, float height, sf::Font font, InputBindingManager::BINDABLE_ACTION action) :
    UIButton(x, y, width, height, "", font, nullptr, ""), _action(action) {
    _keyBinding = InputBindingManager::getKeyboardBinding(_action);
    setLabelText(InputBindingManager::getActionName(_action) + ": " + InputBindingManager::getKeyName(_keyBinding));
}

void UIKeyboardBindingButton::update() {
    sf::FloatRect bounds = _sprite.getGlobalBounds();
    if (!_mouseDown && (bounds.contains(getMousePos().x, getMousePos().y)) && !_isSelected) {
        _sprite.setTexture(*getHoverTexture());
    } else if (_isSelected) {
        _sprite.setTexture(*getClickTexture());
    } else {
        _sprite.setTexture(*_texture);
    }
}

void UIKeyboardBindingButton::mouseButtonReleased(const int mx, const int my, const int button) {
    _mouseDown = false;
    sf::FloatRect bounds = _sprite.getGlobalBounds();
    if (bounds.contains(mx, my)) {
        _isSelected = true;
    } else _isSelected = false;
}

void UIKeyboardBindingButton::keyReleased(sf::Keyboard::Key& key) {
    if (_isSelected) {
        _keyBinding = key;
        InputBindingManager::keyboardBindingSelected(_action, _keyBinding);
    }
}
