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
                writeToSettingsFile();
            }
        }
    }

    static bool isCompleted() {
        return _tutorialCompleted;
    }
    
private:
    inline static std::map<TUTORIAL_STEP, std::string> _messagesKeyboard = {
        {TUTORIAL_STEP::START, "Pick up that green orb."},
        {TUTORIAL_STEP::PICK_UP_SLIMEBALL, "Now press tab to open the inventory, and\nclick the slime ball to equip it."},
        {TUTORIAL_STEP::EQUIP_SLIMEBALL, "Great! The slime ball will shoot at enemies.\nDefeat the first wave."},
        {TUTORIAL_STEP::CLEAR_WAVE_1, "You beat the first wave!\nNow you should find a shop and buy an axe."},
        {TUTORIAL_STEP::BUY_AXE, "Excellent. Now equip the axe, head back outside,\ngo up to a tree, and spin around in circles\nto cut it down."},
        {TUTORIAL_STEP::CUT_DOWN_TREE, "You got some wood! Go back to the shop (or find a different one)\nand sell your wood."},
        {TUTORIAL_STEP::SELL_WOOD, "That's about it! You might wanna save up some money so you can buy some weapons and\nother items."}
    };

    inline static std::map<TUTORIAL_STEP, std::string> _messagesGamepad = {
        {TUTORIAL_STEP::START, "Pick up that green orb."},
        {TUTORIAL_STEP::PICK_UP_SLIMEBALL, "Now press select to open the inventory, and\nuse the d-pad to select the slime ball, and press A to\nequip it."},
        {TUTORIAL_STEP::EQUIP_SLIMEBALL, "Great! The slime ball will shoot at enemies.\nDefeat the first wave."},
        {TUTORIAL_STEP::CLEAR_WAVE_1, "You beat the first wave!\nNow you should find a shop and buy an axe."},
        {TUTORIAL_STEP::BUY_AXE, "Excellent. Now equip the axe, head back outside,\ngo up to a tree, and spin around in circles\nto cut it down."},
        {TUTORIAL_STEP::CUT_DOWN_TREE, "You got some wood! Go back to the shop (or find a different one)\nand sell your wood.\nUse the bumpers to switch between buying\nand selling"},
        {TUTORIAL_STEP::SELL_WOOD, "That's about it! You might wanna save up some money so you can buy some weapons and\nother items."}
    };

    inline static TUTORIAL_STEP _currentStep = TUTORIAL_STEP::START;
    inline static bool _tutorialCompleted = false;

    static void writeToSettingsFile() {
        std::string fileName = "settings.config";
        try {
            if (!std::filesystem::remove(fileName))
                MessageManager::displayMessage("Could not replace settings file", 5, DEBUG);
        } catch (const std::filesystem::filesystem_error& err) {
            MessageManager::displayMessage("Could not replace settings file: " + (std::string)err.what(), 5, ERR);
        }

        try {
            std::ofstream out(fileName);
            int fullscreenSetting = FULLSCREEN ? 1 : 0;
            int tutorialCompleted = _tutorialCompleted ? 1 : 0;
            out << "fullscreen=" << std::to_string(fullscreenSetting) << std::endl;
            out << "tutorial=" << std::to_string(tutorialCompleted) << std::endl;
            out.close();
        } catch (std::exception ex) {
            MessageManager::displayMessage("Error writing to settings file: " + (std::string)ex.what(), 5, ERR);
        }
    }
};

#endif