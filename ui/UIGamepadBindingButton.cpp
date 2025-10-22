#include "UIGamepadBindingButton.h"
#include "../core/MessageManager.h"

UIGamepadBindingButton::UIGamepadBindingButton(float x, float y, float width, float height, sf::Font font, InputBindingManager::BINDABLE_ACTION action) :
    UIButton(x, y, width, height, "", font, nullptr, "", false, false), _action(action) {
    _gamepadBinding = InputBindingManager::getGamepadBinding(_action);
    setLabelText(InputBindingManager::getActionName(_action) + ": " + InputBindingManager::getGamepadButtonName(_gamepadBinding));

    _disableMouseMovementDeselection = true;
}

void UIGamepadBindingButton::update() {
    _gamepadBinding = InputBindingManager::getGamepadBinding(_action);
    setLabelText(InputBindingManager::getActionName(_action) + ": " + InputBindingManager::getGamepadButtonName(_gamepadBinding));

    sf::FloatRect bounds = getBounds();
    if (!_mouseDown && (bounds.contains(getMousePos().x, getMousePos().y)) && !_isSelected) {
        setAppearance(BUTTON_HOVER_CONFIG);
    } else if (_isSelected) {
        setAppearance(BUTTON_CLICKED_CONFIG);
    } else {
        setAppearance(BUTTON_CONFIG);
    }    
}

void UIGamepadBindingButton::mouseButtonReleased(const int mx, const int my, const int button) {
    _mouseDown = false;
    sf::FloatRect bounds = getBounds();
    if (bounds.contains(mx, my)) {
        _isSelected = true;
    } else _isSelected = false;
}

void UIGamepadBindingButton::controllerButtonReleased(GAMEPAD_BUTTON button) {
    if (_isSelected 
        && button != GAMEPAD_BUTTON::DPAD_UP
        && button != GAMEPAD_BUTTON::DPAD_DOWN
        && button != GAMEPAD_BUTTON::DPAD_LEFT
        && button != GAMEPAD_BUTTON::DPAD_RIGHT) {
        _gamepadBinding = button;
        InputBindingManager::gamePadBindingSelected(_action, _gamepadBinding);
        setLabelText(InputBindingManager::getActionName(_action) + ": " + InputBindingManager::getGamepadButtonName(_gamepadBinding));
        _isSelected = false;
    }
}