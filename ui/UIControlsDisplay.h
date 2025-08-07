#ifndef _UI_CONTROLS_DISPLAY_H
#define _UI_CONTROLS_DISPLAY_H

#include "UIElement.h"

struct ControlLabel {
    ControlLabel(const sf::Vector2f pos, const GAMEPAD_BUTTON button, const std::string text) 
        : pos(pos), button(button), text(text) {}

    const sf::Vector2f pos;
    const GAMEPAD_BUTTON button;
    const std::string text;
};

class UIControlsDisplay : public UIElement {
public:

protected:

private:
    // std::map of GAMEPAD_BUTTON to texture coords
    // vector of (probably shared pointers to) ControlLabels
};

#endif