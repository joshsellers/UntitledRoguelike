#ifndef _GAMEPAD_LISTENER_H
#define _GAMEPAD_LISTENER_H

#include "GamePadButtons.h"

class GamePadListener {
public:
    virtual void controllerButtonReleased(GAMEPAD_BUTTON button) = 0;
    virtual void controllerButtonPressed(GAMEPAD_BUTTON button) = 0;
};

#endif