#include "UIKeyboardBindingButton.h"

UIKeyboardBindingButton::UIKeyboardBindingButton(float x, float y, float width, float height, sf::Font font, InputBindingManager::BINDABLE_ACTION action) :
    UIButton(x, y, width, height, "", font, nullptr, "", false, false), _action(action) {
    _keyBinding = InputBindingManager::getKeyboardBinding(_action);
    setLabelText(InputBindingManager::getActionName(_action) + ": " + InputBindingManager::getKeyName(_keyBinding));

    _disableMouseMovementDeselection = true;
}

void UIKeyboardBindingButton::update() {
    _keyBinding = InputBindingManager::getKeyboardBinding(_action);
    setLabelText(InputBindingManager::getActionName(_action) + ": " + InputBindingManager::getKeyName(_keyBinding));

    sf::FloatRect bounds = getBounds();
    if (!_mouseDown && (bounds.contains(getMousePos().x, getMousePos().y)) && !_isSelected) {
        setAppearance(BUTTON_HOVER_CONFIG);
    } else if (_isSelected) {
        setAppearance(BUTTON_CLICKED_CONFIG);
    } else {
        setAppearance(BUTTON_CONFIG);
    }
}

void UIKeyboardBindingButton::mouseButtonReleased(const int mx, const int my, const int button) {
    _mouseDown = false;
    sf::FloatRect bounds = getBounds();
    if (bounds.contains(mx, my)) {
        _isSelected = true;
    } else _isSelected = false;
}

void UIKeyboardBindingButton::keyReleased(sf::Keyboard::Key& key) {
    if (_isSelected) {
        _keyBinding = key;
        InputBindingManager::keyboardBindingSelected(_action, _keyBinding);
        setLabelText(InputBindingManager::getActionName(_action) + ": " + InputBindingManager::getKeyName(_keyBinding));
        _isSelected = false;
    }
}
