#include "GamePad.h"
#include "../InputBindings.h"

void GamePad::setControllerId(unsigned int id) {
    _id = id;

    const auto& info = sf::Joystick::getIdentification(id);
    MessageManager::displayMessage("Gamepad name: " + info.name.toAnsiString(), 5, DEBUG);
    MessageManager::displayMessage("Gamepad VID: " + intToHex(info.vendorId), 5, DEBUG);
    MessageManager::displayMessage("Gamepad PID: " + intToHex(info.productId), 5, DEBUG);

    _vid = info.vendorId;
    _pid = info.productId;

    //if (_pid == DUALSENSE_PID) InputBindingManager::setDualSenseBindings();
}

unsigned int GamePad::getControllerId() {
    return _id;
}

unsigned int GamePad::getVendorId() {
    return _vid;
}

unsigned int GamePad::getProductId() {
    return _pid;
}

void GamePad::vibrate(int vibrationAmount, long long time) {
    if (_isVibrating || _pid == DUALSENSE_PID || (!STEAMAPI_INITIATED && _vid == SONY_VID)) return;
    vibrationAmount = std::min(MAX_CONTROLLER_VIBRATION, vibrationAmount);
    _isVibrating = true;
    std::thread vibrateThread(&GamePad::runVibration, vibrationAmount, time);
    vibrateThread.detach();
}

float GamePad::getLeftStickXAxis() {
    return removeDeadZone(sf::Joystick::getAxisPosition(getControllerId(), sf::Joystick::X));
}

float GamePad::getLeftStickYAxis() {
    return removeDeadZone(sf::Joystick::getAxisPosition(getControllerId(), sf::Joystick::Y));
}

float GamePad::getRightStickXAxis() {
    return removeDeadZone(sf::Joystick::getAxisPosition(getControllerId(), _pid == DUALSENSE_PID ? sf::Joystick::Z : sf::Joystick::U));
}

float GamePad::getRightStickYAxis() {
    return removeDeadZone(sf::Joystick::getAxisPosition(getControllerId(), _pid == DUALSENSE_PID ? sf::Joystick::R : sf::Joystick::V));
}

bool GamePad::isButtonPressed(GAMEPAD_BUTTON button) {
    if (_pid == DUALSENSE_PID) return sf::Joystick::isButtonPressed(getControllerId(), (unsigned int)translateButton(translateButton(button)));

    return (unsigned int)button < (unsigned int)GAMEPAD_BUTTON::LEFT_TRIGGER ?
        sf::Joystick::isButtonPressed(getControllerId(), (unsigned int)button)
        : _triggerIsPressed[(int)button - (int)GAMEPAD_BUTTON::LEFT_TRIGGER];
}

void GamePad::setDeadZone(float deadZone) {
    _deadZone = deadZone;
}

float GamePad::getDeadZone() {
    return _deadZone;
}

bool GamePad::isLeftStickDeadZoned() {
    return getLeftStickXAxis() == 0.f && getLeftStickYAxis() == 0.f;
}

bool GamePad::isRightStickDeadZoned() {
    return getRightStickXAxis() == 0.f && getRightStickYAxis() == 0.f;
}

bool GamePad::isConnected() {
    return sf::Joystick::isConnected(getControllerId());
}

void GamePad::receiveControllerEvent(sf::Event& event) {
    switch (event.type) {
        case sf::Event::JoystickMoved:
            updateControllerAxisValue((GAMEPAD_AXIS)event.joystickMove.axis, event.joystickMove.position);
            break;
        case sf::Event::JoystickConnected:
        {
            MessageManager::displayMessage("Controller connected", 5);

            int controllerId = -1;
            for (int i = 0; i < 7; i++) {
                if (sf::Joystick::isConnected(i)) {
                    controllerId = i;
                    break;
                }
            }
            if (controllerId != -1) setControllerId(controllerId);

            for (auto& listener : _listeners) listener->gamepadConnected();
            break;
        }
        case sf::Event::JoystickDisconnected:
            MessageManager::displayMessage("Controller disconnected", 5);
            for (auto& listener : _listeners) listener->gamepadDisconnected();
            break;
        case sf::Event::JoystickButtonReleased:
            listenerButtonReleaseCallback(translateButton((GAMEPAD_BUTTON)event.joystickButton.button));
            break;
        case sf::Event::JoystickButtonPressed:
            listenerButtonPressCallback(translateButton((GAMEPAD_BUTTON)event.joystickButton.button));
            break;
    }
}

void GamePad::addListener(std::shared_ptr<GamePadListener> listener) {
    _listeners.push_back(listener);
}

void GamePad::runVibration(int vibrationAmount, long long time) {
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

float GamePad::removeDeadZone(float axisValue) {
    return std::abs(axisValue) > _deadZone ? axisValue : 0.f;
}

void GamePad::updateControllerAxisValue(GAMEPAD_AXIS axis, float value) {
    if (_pid == DUALSENSE_PID && (axis == GAMEPAD_AXIS::RIGHT_STICK_X || axis == GAMEPAD_AXIS::TRIGGERS)) return;
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

void GamePad::listenerButtonReleaseCallback(GAMEPAD_BUTTON button) {
    if ((int)button >= (int)GAMEPAD_BUTTON::LEFT_TRIGGER)
        _triggerIsPressed[(int)button - (int)GAMEPAD_BUTTON::LEFT_TRIGGER] = false;

    for (auto& listener : _listeners)
        listener->controllerButtonReleased(button);
}

void GamePad::listenerButtonPressCallback(GAMEPAD_BUTTON button) {
    if ((int)button >= (int)GAMEPAD_BUTTON::LEFT_TRIGGER)
        _triggerIsPressed[(int)button - (int)GAMEPAD_BUTTON::LEFT_TRIGGER] = true;

    for (auto& listener : _listeners)
        listener->controllerButtonPressed(button);
}

GAMEPAD_BUTTON GamePad::translateButton(GAMEPAD_BUTTON button) {
    if (_pid != DUALSENSE_PID) return button;

    GAMEPAD_BUTTON newButton = button;

    switch (button) {
        case GAMEPAD_BUTTON::SELECT:
            newButton = GAMEPAD_BUTTON::LEFT_TRIGGER;
            break;
        case GAMEPAD_BUTTON::START:
            newButton = GAMEPAD_BUTTON::RIGHT_TRIGGER;
            break;
        case GAMEPAD_BUTTON::LEFT_STICK:
            newButton = GAMEPAD_BUTTON::SELECT;
            break;
        case GAMEPAD_BUTTON::RIGHT_STICK:
            newButton = GAMEPAD_BUTTON::START;
            break;
        case GAMEPAD_BUTTON::A:
            newButton = GAMEPAD_BUTTON::X;
            break;
        case GAMEPAD_BUTTON::B:
            newButton = GAMEPAD_BUTTON::A;
            break;
        case GAMEPAD_BUTTON::X:
            newButton = GAMEPAD_BUTTON::B;
            break;
        case GAMEPAD_BUTTON::LEFT_TRIGGER:
            newButton = GAMEPAD_BUTTON::LEFT_STICK;
            break;
        case GAMEPAD_BUTTON::RIGHT_TRIGGER:
            newButton = GAMEPAD_BUTTON::RIGHT_STICK;
            break;
    }

    return newButton;
}
