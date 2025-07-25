#ifndef _TUTORIAL_H
#define _TUTORIAL_H

#include <vector>
#include <string>
#include "MessageManager.h"
#include "gamepad/GamePad.h"
#include <filesystem>
#include <fstream>

enum class TUTORIAL_STEP {
    START,
    PICK_UP_BEE,
    USE_BEE,
    CLEAR_WAVE_1,
    BUY_BOW,
    EQUIP_BOW,
    RELOAD_BOW,
    END
};

class Tutorial {
public:
    static void completeStep(TUTORIAL_STEP step) {
        if (step == TUTORIAL_STEP::END) {
            _tutorialCompleted = true;
            return;
        } else if (step == TUTORIAL_STEP::START && _currentStep == TUTORIAL_STEP::START) {
            tutorialStartTime = currentTimeMillis();
        }

        if (!isCompleted() && _currentStep == step) {
            int timeout = (int)_currentStep + 1;
            int messageType = TUTORIAL;
            if (_currentStep == TUTORIAL_STEP::RELOAD_BOW) {
                timeout = 10;
                messageType = NORMAL;
            }

            if (GamePad::isConnected()) {
                MessageManager::displayMessage(_messagesGamepad[_currentStep], timeout, messageType);
            } else {
                MessageManager::displayMessage(_messagesKeyboard[_currentStep], timeout, messageType);
            }

            MessageManager::clearTutorialMessage((int)_currentStep);
            _currentStep = (TUTORIAL_STEP)((int)_currentStep + 1);
            if (_currentStep == TUTORIAL_STEP::END) {
                _tutorialCompleted = true;
                updateSettingsFiles();
            }
        }
    }

    static TUTORIAL_STEP getCurrentStep() {
        return _currentStep;
    }

    static bool isCompleted() {
        return _tutorialCompleted;
    }

    static void reset() {
        _currentStep = TUTORIAL_STEP::START;
        _tutorialCompleted = false;
    }

    inline static long long tutorialStartTime = 0LL;
    const inline static long long tutorialAutoDisableTime = 8 * 60 * 1000;
    
private:
    inline static std::map<TUTORIAL_STEP, std::string> _messagesKeyboard = {
        {TUTORIAL_STEP::START, "Walk into that bee."},
        {TUTORIAL_STEP::PICK_UP_BEE, "Now press tab to open the inventory, and\nclick the bee to activate it.\n\nThis is the same way you will activate most items that\nyou buy from the shop."},
        {TUTORIAL_STEP::USE_BEE, "Great! The bee will attack enemies.\nThe first wave will start soon.\nMake sure you pick up the coins dropped by enemies.\n\nHold B to start the first wave when you're ready."},
        {TUTORIAL_STEP::CLEAR_WAVE_1, "You beat the first wave!\nNow you should find a shop and buy a bow."},
        {TUTORIAL_STEP::BUY_BOW, "Excellent. Now open your inventory and equip the bow."},
        {TUTORIAL_STEP::EQUIP_BOW, "Great! Press R to load your weapon."},
        {TUTORIAL_STEP::RELOAD_BOW, "That's about it! Get ready for the next wave.\nYou can hold B to skip the cooldown.\nNo respawning anymore!"}
    };

    inline static std::map<TUTORIAL_STEP, std::string> _messagesGamepad = {
        {TUTORIAL_STEP::START, "Walk into that bee."},
        {TUTORIAL_STEP::PICK_UP_BEE, "Now press select to open the inventory,\nuse the d-pad to select the bee, and press A to\nactivate it.\n\nThis is the same way you will activate most items that\nyou buy from the shop."},
        {TUTORIAL_STEP::USE_BEE, "Great! The bee will attack enemies.\nThe first wave will start soon.\nMake sure you pick up the coins dropped by enemies.\n\nHold B to start the first wave when you're ready."},
        {TUTORIAL_STEP::CLEAR_WAVE_1, "You beat the first wave!\nNow you should find a shop and buy a bow."},
        {TUTORIAL_STEP::BUY_BOW, "Excellent. Now open your inventory and equip the bow."},
        {TUTORIAL_STEP::EQUIP_BOW, "Great!\nYou can use the right joystick to aim, and\nthe right trigger to shoot.\nPress X to load your weapon."},
        {TUTORIAL_STEP::RELOAD_BOW, "That's about it! Get ready for the next wave.\nYou can hold B to skip the cooldown.\nNo respawning anymore!"}
    };

    inline static TUTORIAL_STEP _currentStep = TUTORIAL_STEP::START;
    inline static bool _tutorialCompleted = false;
};

#endif