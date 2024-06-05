#ifndef _GAMEPAD_BUTTONS_H
#define _GAMEPAD_BUTTONS_H

enum class GAMEPAD_BUTTON : unsigned int {
    A = 0,
    B = 1,
    Y = 3,
    X = 2,

    START = 7,
    SELECT = 6,

    LEFT_STICK = 8,
    RIGHT_STICK = 9,

    LEFT_BUMPER = 4,
    RIGHT_BUMPER = 5,

    // these are not actually buttons but i'm doing some hacky stuff to make them act like buttons
    LEFT_TRIGGER = 10,
    RIGHT_TRIGGER = 11,

    DPAD_UP = 12,
    DPAD_DOWN = 13,
    DPAD_LEFT = 14,
    DPAD_RIGHT = 15
};

enum class GAMEPAD_AXIS : unsigned int {
    LEFT_STICK_Y = 0,
    LEFT_STICK_X = 1,

    TRIGGERS = 2,

    UNKNOWN_AXIS = 3, // might be accelerometer?

    RIGHT_STICK_Y = 4,
    RIGHT_STICK_X = 5,

    DPAD_Y = 7,
    DPAD_X = 6
};

#endif