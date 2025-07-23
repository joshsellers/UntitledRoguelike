#include "LocalAchievementManager.h"
#include "../core/Globals.h"
#include "../core/Util.h"
#include <filesystem>
#include "../core/MessageManager.h"
#include "../core/SoundManager.h"
#include "../core/FileIntegrityManager.h"

void LocalAchievementManager::unlock(ACHIEVEMENT achievement) {
    if (!isReady() || isUnlocked(achievement)) return;
    _unlockedAchievements[achievement] = true;
    MessageManager::displayMessage("Achievement unlocked: " + getAchievementName(achievement), 5, SPECIAL);
    SoundManager::playSound("itemunlock");
    saveLocalAchievements();
}

void LocalAchievementManager::loadLocalAchievements() {
    if (!isReady()) return;

    std::string path = getLocalLowPath() + "\\save" + std::to_string(SELECTED_SAVE_FILE);
    if (!std::filesystem::is_directory(path + "\\")) {
        std::filesystem::create_directory(path);
    }

    path += "\\achievements.config";
    std::ifstream in(path);

    if (!in.good()) {
        MessageManager::displayMessage("Could not find achivements file", 5, DEBUG);
        in.close();
    } else {
        std::string line;
        while (getline(in, line)) {
            if (stringStartsWith(line, "#")) continue;

            try {
                const int achNumber = std::stoi(line);
                if (achNumber >= 0 && achNumber < NUM_ACHIEVEMENTS) {
                    _unlockedAchievements[achNumber] = true;
                    AchievementManager::unlock((ACHIEVEMENT)achNumber, false);
                } else {
                    MessageManager::displayMessage("Invalid achievement id: " + std::to_string(achNumber), 5, WARN);
                }
            } catch (std::exception ex) {
                MessageManager::displayMessage("Error reading achivements.config: " + (std::string)ex.what(), 5, ERR);
            }
        }

        in.close();
    }
}

void LocalAchievementManager::saveLocalAchievements() {
    if (!isReady()) return; 
    
    std::string path = getLocalLowPath() + "\\save" + std::to_string(SELECTED_SAVE_FILE);
    if (!std::filesystem::is_directory(path + "\\")) {
        std::filesystem::create_directory(path);
    }

    path += "\\achievements.config";

    try {
        if (!std::filesystem::remove(path)) {
            MessageManager::displayMessage("Could not replace achievements file", 5, DEBUG);
        }
    } catch (const std::filesystem::filesystem_error& err) {
        MessageManager::displayMessage("Could not replace achievements file: " + (std::string)err.what(), 5, ERR);
    }

    try {
        std::ofstream out(path);
        for (int i = 0; i < NUM_ACHIEVEMENTS; i++) {
            if (_unlockedAchievements[i]) {
                out << std::to_string(i) << std::endl;
            }
        }
        out.close();

        FileIntegrityManager::signFile(path);
    } catch (std::exception ex) {
        MessageManager::displayMessage("Error saving achievements: " + (std::string)ex.what(), 5, ERR);
    }
}

void LocalAchievementManager::softReset() {
    saveLocalAchievements();
    for (int i = 0; i < NUM_ACHIEVEMENTS; i++) {
        _unlockedAchievements[i] = false;
    }
}

void LocalAchievementManager::hardReset() {
    for (int i = 0; i < NUM_ACHIEVEMENTS; i++) {
        _unlockedAchievements[i] = false;
    }
    saveLocalAchievements();
}

std::string LocalAchievementManager::getAchievementName(ACHIEVEMENT achievement) {
    return _achievementNames.at(achievement);
}

bool LocalAchievementManager::isUnlocked(ACHIEVEMENT achievement) {
    return _unlockedAchievements[achievement];
}

bool LocalAchievementManager::isReady() {
    return SELECTED_SAVE_FILE != SAVE_FILE_NOT_SELECTED && !DISABLE_ACHIEVEMENTS;
}
