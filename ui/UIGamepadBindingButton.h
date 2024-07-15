#ifndef _UI_GAMEPAD_BINDING_BUTTON_H
#define _UI_GAMEPAD_BINDING_BUTTON_H

#include "../core/InputBindings.h"
#include "UIButton.h"
#include "../core/gamepad/GamePadButtons.h"

class UIGamepadBindingButton : public UIButton {
public:
    UIGamepadBindingButton(float x, float y, float width, float height, sf::Font font,
        InputBindingManager::BINDABLE_ACTION action);

    void update();

    virtual void mouseButtonReleased(const int mx, const int my, const int button); 
    virtual void controllerButtonReleased(GAMEPAD_BUTTON button);

private:
    const InputBindingManager::BINDABLE_ACTION _action;
    GAMEPAD_BUTTON _gamepadBinding;
};

#endif