#ifndef _UI_BUTTON_LISTENER_H
#define _UI_BUTTON_LISTENER_H

#include <string>

class UIButtonListener {
public:
    virtual void buttonPressed(std::string buttonCode) = 0;
};

#endif