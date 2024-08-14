#ifndef _GAME_PAD_H
#define _GAME_PAD_H

#include <SFML/Window/Joystick.hpp>
#include "GamePadListener.h"
#include <iostream>
#include <windows.h>
#include <Xinput.h>
#include <thread>
#include "../MessageManager.h"

constexpr int MAX_CONTROLLER_VIBRATION = 65535;

class GamePad {
public:

    static void setControllerId(unsigned int id) {
        _id = id;
    }

    static unsigned int getControllerId() {
        return _id;
    }

    static void vibrate(int vibrationAmount, long long time) {
        if (_isVibrating) return;
        vibrationAmount = std::min(MAX_CONTROLLER_VIBRATION, vibrationAmount);
        _isVibrating = true;
        std::thread vibrateThread(&GamePad::runVibration, vibrationAmount, time);
        vibrateThread.detach();
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

    static bool isButtonPressed(GAMEPAD_BUTTON button) {
        return (unsigned int)button < (unsigned int)GAMEPAD_BUTTON::LEFT_TRIGGER ?
            sf::Joystick::isButtonPressed(getControllerId(), (unsigned int)button)
            : _triggerIsPressed[(int)button - (int)GAMEPAD_BUTTON::LEFT_TRIGGER];
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

    static void receiveControllerEvent(sf::Event& event) {
        switch (event.type) {
            case sf::Event::JoystickMoved: 
                updateControllerAxisValue((GAMEPAD_AXIS)event.joystickMove.axis, event.joystickMove.position);
                break;
            case sf::Event::JoystickConnected:
                MessageManager::displayMessage("Controller connected", 5);
                break;
            case sf::Event::JoystickDisconnected:
                MessageManager::displayMessage("Controller disconnected", 5);
                for (auto& listener : _listeners) listener->gamepadDisconnected();
                break;
            case sf::Event::JoystickButtonReleased:
                listenerButtonReleaseCallback((GAMEPAD_BUTTON)event.joystickButton.button);
                break;
            case sf::Event::JoystickButtonPressed:
                listenerButtonPressCallback((GAMEPAD_BUTTON)event.joystickButton.button);
                break;
        }
    }

    static void addListener(std::shared_ptr<GamePadListener> listener) {
        _listeners.push_back(listener);
    }

private:
    inline static unsigned int _id = 0;
    
    inline static float _deadZone = 10.f;

    inline static float _lastLeftTriggerValue = 0.f;
    inline static float _lastRightTriggerValue = 0.f;
    inline static float _triggerDeadZone = 1.f;
    inline static float _triggerPressThreshold = 50.f;

    inline static float _lastDPadXValue = 0.f;
    inline static float _lastDPadYValue = 0.f;

    inline static bool _triggerIsPressed[(int)GAMEPAD_BUTTON::DPAD_RIGHT - (int)GAMEPAD_BUTTON::LEFT_TRIGGER + 1];

    inline static bool _isVibrating = false;

    static void runVibration(int vibrationAmount, long long time) {
        XINPUT_VIBRATION vibration;
        ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
        vibration.wLeftMotorSpeed = vibrationAmount;
        vibration.wRightMotorSpeed = vibrationAmount;
        XInputSetState(0, &vibration);

        Sleep(time);

        vibration.wLeftMotorSpeed = 0;
        vibration.wRightMotorSpeed = 0;
        XInputSetState(0, &vibration);

        _isVibrating = false;
    }

    static float removeDeadZone(float axisValue) {
        return std::abs(axisValue) > _deadZone ? axisValue : 0.f;
    }

    static void updateControllerAxisValue(GAMEPAD_AXIS axis, float value) {
        switch (axis) {
            case GAMEPAD_AXIS::TRIGGERS:
            {
                float deadZonedValue = std::abs(value) > _triggerDeadZone ? std::abs(value) : 0;

                if (deadZonedValue == 0) {
                    if (_lastRightTriggerValue > 0)
                        listenerButtonReleaseCallback(GAMEPAD_BUTTON::RIGHT_TRIGGER);
                    if (_lastLeftTriggerValue > 0)
                        listenerButtonReleaseCallback(GAMEPAD_BUTTON::LEFT_TRIGGER);

                    _lastRightTriggerValue = deadZonedValue;
                    _lastLeftTriggerValue = deadZonedValue;
                } else if (value < 0) {
                    if (deadZonedValue > _triggerPressThreshold && _lastRightTriggerValue <= _triggerPressThreshold)
                        listenerButtonPressCallback(GAMEPAD_BUTTON::RIGHT_TRIGGER);
                    _lastRightTriggerValue = deadZonedValue;
                } else if (value > 0) {
                    if (deadZonedValue > _triggerPressThreshold && _lastLeftTriggerValue <= _triggerPressThreshold)
                        listenerButtonPressCallback(GAMEPAD_BUTTON::LEFT_TRIGGER);
                    _lastLeftTriggerValue = deadZonedValue;
                }
                break;
            }
            case GAMEPAD_AXIS::DPAD_X:
                if (value == 0 && _lastDPadXValue == -100) listenerButtonReleaseCallback(GAMEPAD_BUTTON::DPAD_LEFT);
                else if (value == -100 && _lastDPadXValue == 0) listenerButtonPressCallback(GAMEPAD_BUTTON::DPAD_LEFT);
                else if (value == 0 && _lastDPadXValue == 100) listenerButtonReleaseCallback(GAMEPAD_BUTTON::DPAD_RIGHT);
                else if (value == 100 && _lastDPadXValue == 0) listenerButtonPressCallback(GAMEPAD_BUTTON::DPAD_RIGHT);
                _lastDPadXValue = value;
                break;
            case GAMEPAD_AXIS::DPAD_Y:
                if (value == 0 && _lastDPadYValue == -100) listenerButtonReleaseCallback(GAMEPAD_BUTTON::DPAD_DOWN);
                else if (value == -100 && _lastDPadYValue == 0) listenerButtonPressCallback(GAMEPAD_BUTTON::DPAD_DOWN);
                else if (value == 0 && _lastDPadYValue == 100) listenerButtonReleaseCallback(GAMEPAD_BUTTON::DPAD_UP);
                else if (value == 100 && _lastDPadYValue == 0) listenerButtonPressCallback(GAMEPAD_BUTTON::DPAD_UP);
                _lastDPadYValue = value;
                break;
        }
    }

    inline static std::vector<std::shared_ptr<GamePadListener>> _listeners;
    static void listenerButtonReleaseCallback(GAMEPAD_BUTTON button) {
        if ((int)button >= (int)GAMEPAD_BUTTON::LEFT_TRIGGER)
            _triggerIsPressed[(int)button - (int)GAMEPAD_BUTTON::LEFT_TRIGGER] = false;

        for (auto& listener : _listeners)
            listener->controllerButtonReleased(button);
    }

    static void listenerButtonPressCallback(GAMEPAD_BUTTON button) {
        if ((int)button >= (int)GAMEPAD_BUTTON::LEFT_TRIGGER)
            _triggerIsPressed[(int)button - (int)GAMEPAD_BUTTON::LEFT_TRIGGER] = true;

        for (auto& listener : _listeners)
            listener->controllerButtonPressed(button);
    }
};

#endif