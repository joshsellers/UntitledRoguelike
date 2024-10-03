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
    PICK_UP_SLIMEBALL,
    EQUIP_SLIMEBALL,
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
            if (GamePad::isConnected()) {
                MessageManager::displayMessage(_messagesGamepad[_currentStep], 10);
            } else {
                MessageManager::displayMessage(_messagesKeyboard[_currentStep], 10);
            }

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
    const inline static long long tutorialAutoDisableTime = 5 * 60 * 1000;
    
private:
    inline static std::map<TUTORIAL_STEP, std::string> _messagesKeyboard = {
        {TUTORIAL_STEP::START, "Pick up that green orb."},
        {TUTORIAL_STEP::PICK_UP_SLIMEBALL, "Now press tab to open the inventory, and\nclick the slime ball to equip it."},
        {TUTORIAL_STEP::EQUIP_SLIMEBALL, "Great! The slime ball will shoot at enemies.\nThe first wave will start soon.\nMake sure you pick up the coins dropped by enemies."},
        {TUTORIAL_STEP::CLEAR_WAVE_1, "You beat the first wave!\nNow you should find a shop and buy a bow."},
        {TUTORIAL_STEP::BUY_BOW, "Excellent. Now open your inventory and equip the bow."},
        {TUTORIAL_STEP::EQUIP_BOW, "Great! Press R to load your weapon."},
        {TUTORIAL_STEP::RELOAD_BOW, "That's about it! Get ready for the next wave.\nYou can hold B to skip the cooldown.\nNo respawning anymore!"}
    };

    inline static std::map<TUTORIAL_STEP, std::string> _messagesGamepad = {
        {TUTORIAL_STEP::START, "Pick up that green orb."},
        {TUTORIAL_STEP::PICK_UP_SLIMEBALL, "Now press select to open the inventory,\nuse the d-pad to select the slime ball, and press A to\nequip it."},
        {TUTORIAL_STEP::EQUIP_SLIMEBALL, "Great! The slime ball will shoot at enemies.\nThe first wave will start soon.\nMake sure you pick up the coins dropped by enemies"},
        {TUTORIAL_STEP::CLEAR_WAVE_1, "You beat the first wave!\nNow you should find a shop and buy a bow."},
        {TUTORIAL_STEP::BUY_BOW, "Excellent. Now open your inventory and equip the bow."},
        {TUTORIAL_STEP::EQUIP_BOW, "Great! Press X to load your weapon."},
        {TUTORIAL_STEP::RELOAD_BOW, "That's about it! Get ready for the next wave.\nYou can hold B to skip the cooldown.\nNo respawning anymore!"}
    };

    inline static TUTORIAL_STEP _currentStep = TUTORIAL_STEP::START;
    inline static bool _tutorialCompleted = false;
};

#endif