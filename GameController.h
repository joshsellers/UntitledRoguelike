#ifndef _GAME_CONTROLLER_H
#define _GAME_CONTROLLER_H

#include <SFML/Window/Joystick.hpp>
#include "GameControllerListener.h"
#include <iostream>

class GameController {
public:

    static void setControllerId(unsigned int id) {
        _id = id;
    }

    static unsigned int getControllerId() {
        return _id;
    }

    static float getLeftStickXAxis() {
        return removeDeadZone(sf::Joystick::getAxisPosition(getControllerId(), sf::Joystick::X));
    }

    static float getLeftStickYAxis() {
        return removeDeadZone(sf::Joystick::getAxisPosition(getControllerId(), sf::Joystick::Y));
    }

    static float getRightStickXAxis() {
        return removeDeadZone(sf::Joystick::getAxisPosition(getControllerId(), sf::Joystick::U));
    }

    static float getRightStickYAxis() {
        return removeDeadZone(sf::Joystick::getAxisPosition(getControllerId(), sf::Joystick::V));
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

    static bool isLeftStickDeadZoned() {
        return getLeftStickXAxis() == 0.f && getLeftStickYAxis() == 0.f;
    }

    static bool isRightStickDeadZoned() {
        return getRightStickXAxis() == 0.f && getRightStickYAxis() == 0.f;
    }

    static bool isConnected() {
        return sf::Joystick::isConnected(getControllerId());
    }

    // replace this with receiveControllerEvent(event) and send all joystick events to GameController so that
    // they can be passed to listeners
    static void updateControllerAxisValue(CONTROLLER_AXIS axis, float value) {
        if (axis == CONTROLLER_AXIS::TRIGGERS) {
            float deadZonedValue = std::abs(value) > _triggerDeadZone ? std::abs(value) : 0;

            if (deadZonedValue == 0) {
                if (_lastRightTriggerValue > 0)
                    listenerButtonReleaseCallback(CONTROLLER_BUTTON::RIGHT_TRIGGER);
                if (_lastLeftTriggerValue > 0)
                    listenerButtonReleaseCallback(CONTROLLER_BUTTON::LEFT_TRIGGER);
                
                _lastRightTriggerValue = deadZonedValue;
                _lastLeftTriggerValue = deadZonedValue;
            } else if (value < 0) {
                if (deadZonedValue > _triggerPressThreshold && _lastRightTriggerValue <= _triggerPressThreshold) 
                    listenerButtonPressCallback(CONTROLLER_BUTTON::RIGHT_TRIGGER);
                _lastRightTriggerValue = deadZonedValue;
            } else if (value > 0) {
                if (deadZonedValue > _triggerPressThreshold && _lastLeftTriggerValue <= _triggerPressThreshold) 
                    listenerButtonPressCallback(CONTROLLER_BUTTON::LEFT_TRIGGER);
                _lastLeftTriggerValue = deadZonedValue;
            }
        }
    }

    static void addListener(std::shared_ptr<GameControllerListener> listener) {
        _listeners.push_back(listener);
    }

private:
    inline static unsigned int _id = 0;
    
    inline static float _deadZone = 10.f;

    inline static float _lastLeftTriggerValue = 0.f;
    inline static float _lastRightTriggerValue = 0.f;
    inline static float _triggerDeadZone = 1.f;
    inline static float _triggerPressThreshold = 50.f;

    static float removeDeadZone(float axisValue) {
        return std::abs(axisValue) > _deadZone ? axisValue : 0.f;
    }

    inline static std::vector <std::shared_ptr<GameControllerListener>> _listeners;
    static void listenerButtonReleaseCallback(CONTROLLER_BUTTON button) {
        for (auto& listener : _listeners)
            listener->controllerButtonReleased(button);
    }

    static void listenerButtonPressCallback(CONTROLLER_BUTTON button) {
        for (auto& listener : _listeners)
            listener->controllerButtonPressed(button);
    }
};

#endif