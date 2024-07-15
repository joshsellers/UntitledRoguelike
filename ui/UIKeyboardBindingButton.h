#ifndef _UI_KEYBOARD_BINDING_BUTTON_H
#define _UI_KEYBOARD_BINDING_BUTTON_H

#include "UIButton.h"
#include "../core/InputBindings.h"

class UIKeyboardBindingButton : public UIButton {
public:
    UIKeyboardBindingButton(float x, float y, float width, float height, sf::Font font, 
        InputBindingManager::BINDABLE_ACTION action);

    void update();

    virtual void mouseButtonReleased(const int mx, const int my, const int button);
    virtual void keyReleased(sf::Keyboard::Key& key);

private:
    const InputBindingManager::BINDABLE_ACTION _action;
    sf::Keyboard::Key _keyBinding;
};

#endif

