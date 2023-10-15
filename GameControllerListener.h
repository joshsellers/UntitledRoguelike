#ifndef _GAME_CONTROLLER_LISTENER_H
#define _GAME_CONTROLLER_LISTENER_H

#include "GameControllerButtons.h"

class GameControllerListener {
public:
    virtual void controllerButtonReleased(CONTROLLER_BUTTON button) = 0;
    virtual void controllerButtonPressed(CONTROLLER_BUTTON button) = 0;
};

#endif