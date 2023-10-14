#ifndef _GAME_CONTROLLER_H
#define _GAME_CONTROLLER_H

#include <SFML/Window/Joystick.hpp>

enum class CONTROLLER_BUTTON : unsigned int {
    A = 0,
    B = 1,
    Y = 3,
    X = 2,

    START = 7,
    SELECT = 6,

    LEFT_STICK = 8,
    RIGHT_STICK = 9,

    LEFT_BUMPER = 4,
    RIGHT_BUMPER = 5
};

class GameController {
public:

    static void setControllerId(unsigned int id) {
        _id = id;
    }

    static unsigned int getControllerId() {
        return _id;
    }

    static float getLeftStickXAxis() {
        return removeDeadZone(sf::Joystick::getAxisPosition(0, sf::Joystick::X));
    }

    static float getLeftStickYAxis() {
        return removeDeadZone(sf::Joystick::getAxisPosition(0, sf::Joystick::Y));
    }

    static float getRightStickXAxis() {
        return removeDeadZone(sf::Joystick::getAxisPosition(0, sf::Joystick::U));
    }

    static float getRightStickYAxis() {
        return removeDeadZone(sf::Joystick::getAxisPosition(0, sf::Joystick::V));
    }

    static bool isButtonPressed(CONTROLLER_BUTTON button) {
        return sf::Joystick::isButtonPressed(getControllerId(), (unsigned int) button);
    }

    static void setDeadZone(float deadZone) {
        _deadZone = deadZone;
    }

    static float getDeadZone() {
        return _deadZone;
    }

    static bool isConnected() {
        return sf::Joystick::isConnected(getControllerId());
    }

private:
    inline static unsigned int _id = 0;
    
    inline static float _deadZone = 10.f;

    static float removeDeadZone(float axisValue) {
        return std::abs(axisValue) > _deadZone ? axisValue : 0.f;
    }
};

#endif