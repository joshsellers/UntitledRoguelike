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
    BUY_AXE,
    CUT_DOWN_TREE,
    SELL_WOOD,
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
        {TUTORIAL_STEP::CLEAR_WAVE_1, "You beat the first wave!\nNow you should find a shop and buy an axe."},
        {TUTORIAL_STEP::BUY_AXE, "Excellent. Now equip the axe, head back outside,\ngo up to a tree, and spin around in circles\nto cut it down."},
        {TUTORIAL_STEP::CUT_DOWN_TREE, "You got some wood! Go back to the shop (or find a different one)\nand sell your wood."},
        {TUTORIAL_STEP::SELL_WOOD, "That's about it! You might wanna save up some money so you can buy some weapons and\nother items.\nNo respawning anymore!"}
    };

    inline static std::map<TUTORIAL_STEP, std::string> _messagesGamepad = {
        {TUTORIAL_STEP::START, "Pick up that green orb."},
        {TUTORIAL_STEP::PICK_UP_SLIMEBALL, "Now press select to open the inventory, and\nuse the d-pad to select the slime ball, and press A to\nequip it."},
        {TUTORIAL_STEP::EQUIP_SLIMEBALL, "Great! The slime ball will shoot at enemies.\nThe first wave will start soon.\nMake sure you pick up the coins dropped by enemies"},
        {TUTORIAL_STEP::CLEAR_WAVE_1, "You beat the first wave!\nNow you should find a shop and buy an axe."},
        {TUTORIAL_STEP::BUY_AXE, "Excellent. Now equip the axe, head back outside,\ngo up to a tree, and spin around in circles\nto cut it down."},
        {TUTORIAL_STEP::CUT_DOWN_TREE, "You got some wood! Go back to the shop (or find a different one)\nand sell your wood.\nUse the bumpers to switch between buying\nand selling"},
        {TUTORIAL_STEP::SELL_WOOD, "That's about it! You might wanna save up some money so you can buy some weapons and\nother items.\nNo respawning anymore!"}
    };

    inline static TUTORIAL_STEP _currentStep = TUTORIAL_STEP::START;
    inline static bool _tutorialCompleted = false;
};

#endif