#include "ConditionalUnlockManager.h"
#include "Item.h"
#include "../core/MessageManager.h"
#include "../core/Util.h"
#include "../statistics/StatManager.h"
#include "../core/SoundManager.h"
#include "../core/FileIntegrityManager.h"

std::map<std::string, UnlockProgressTracker> ConditionalUnlockManager::_unlockProgress = {
    {"Cyclops Eye", 100},
    {"Chef's Hat", 1},
    {"Order Form", 1},
    {"Quantum Visor", 1},
    {"Cassidy's Tail", 1},
    {"Cassidy's Brain Cell", 1},
    {"Dev's Blessing", 1},
    {"Penny Cannon", 1},
    {"Debit Card", 1},
    {"Coupon", 1},
    {"Minigun", 1},
    {"Rebound Jewel", 1},
    {"Cassidy's Head", 1},
    {"Knife", 1},
    {"Ski Mask", 1},
    {"Heavy Duty Boots", 1},
    {"Airstrike", 1},
    {"Burst Jewel", 1},
    {"Radioactive Octopus", 1},
    {"Can of Soup", 1},
    {"Rocket Launcher", 1},
    {"Laser Pistol", 1},
    {"SMG", 1},
    {"Assault Rifle", 1},
    {"Railgun", 1},
    {"Autolaser", 1},
    {"Speed Pill", 1},
    {"Leaf Hat", 1},
    {"Bark Cuirass", 1},
    {"Bark Greaves", 2},
    {"Bark Sabatons", 3},
    {"Bloat Jewel", 1},
    {"Sharp Teeth", 1},
    {"Scythe", 1},
    {"Acorn", 1},
    {"Penguin Cannon", 1},
    {"Seek Jewel", 1}
};

const bool ConditionalUnlockManager::isUnlocked(std::string itemName) {
    for (const auto& unlockedItemName : _unlockedItems) {
        if (unlockedItemName == itemName) return true;
    }
    return false;
}

void ConditionalUnlockManager::unlockItem(std::string itemName) {
    if (DISABLE_UNLOCKS) return;

    if (isUnlocked(itemName)) return;

    for (const auto& item : Item::ITEMS) {
        if (item->getName() == itemName) {
            _unlockedItems.push_back(itemName);
            StatManager::increaseStat(ITEMS_UNLOCKED, 1);
            saveUnlockedItems();
            MessageManager::displayMessage("New item unlocked: " + itemName, 8, SPECIAL);
            SoundManager::playSound("itemunlock");
            return;
        }
    }

    MessageManager::displayMessage("No item named \"" + itemName + "\"", 5, WARN);
}

void ConditionalUnlockManager::increaseUnlockProgress(std::string itemName, float progress) {
    if (DISABLE_UNLOCKS) return;

    if (isUnlocked(itemName)) return;

    if (_unlockProgress.find(itemName) == _unlockProgress.end()) {
        MessageManager::displayMessage("No item progress stat for item named \"" + itemName + "\"", 5, WARN);
        return;
    }

    _unlockProgress.at(itemName).progress += progress;

    bool unlockedItem = false;
    if (_unlockProgress.at(itemName).progress >= _unlockProgress.at(itemName).valueToUnlock) {
        unlockItem(itemName);
        unlockedItem = true;
    }

    if (!unlockedItem) saveUnlockedItems();
}

void ConditionalUnlockManager::loadUnlockedItems() {
    if (SELECTED_SAVE_FILE == SAVE_FILE_NOT_SELECTED) {
        MessageManager::displayMessage("Tried to load unlocks before save file was selected", 5, WARN);
        return;
    }

    std::string path = getLocalLowPath() + "\\save" + std::to_string(SELECTED_SAVE_FILE);
    if (!std::filesystem::is_directory(path + "\\")) {
        std::filesystem::create_directory(path);
    }

    path += "\\unlocks.config";
    std::ifstream in(path);

    if (!in.good()) {
        MessageManager::displayMessage("Could not find unlocks file", 5, DEBUG);
        in.close();
    } else {
        std::string line;
        while (getline(in, line)) {
            if (stringStartsWith(line, "#")) continue;

            try {
                const std::vector<std::string> parsedEntry = splitString(line, ",");
                const std::string itemName = parsedEntry[0];
                const float progress = std::stof(parsedEntry[1]);

                if (_unlockProgress.find(itemName) == _unlockProgress.end()) {
                    MessageManager::displayMessage("Unkown item name for unlock progress: \"" + itemName + "\"", 5, WARN);
                    continue;
                }

                _unlockProgress.at(itemName).progress = progress;
                if (progress >= _unlockProgress.at(itemName).valueToUnlock) _unlockedItems.push_back(itemName);
            } catch (std::exception ex) {
                MessageManager::displayMessage("Error reading unlock.config: " + std::string(ex.what()), 5, ERR);
            }
        }

        in.close();
    }
}

void ConditionalUnlockManager::saveUnlockedItems() {
    if (DISABLE_UNLOCKS) return;

    if (SELECTED_SAVE_FILE == SAVE_FILE_NOT_SELECTED) {
        MessageManager::displayMessage("Tried to save unlocks before save file was selected", 5, WARN);
        return;
    }

    std::string path = getLocalLowPath() + "\\save" + std::to_string(SELECTED_SAVE_FILE);
    if (!std::filesystem::is_directory(path + "\\")) {
        std::filesystem::create_directory(path);
    }

    path += "\\unlocks.config";

    try {
        if (!std::filesystem::remove(path)) {
            MessageManager::displayMessage("Could not replace unlocks file", 5, DEBUG);
        }
    } catch (const std::filesystem::filesystem_error& err) {
        MessageManager::displayMessage("Could not replace unlocks file: " + (std::string)err.what(), 5, ERR);
    }

    try {
        std::ofstream out(path);
        for (const auto& unlockProgress : _unlockProgress) {
            out << unlockProgress.first << "," << unlockProgress.second.progress << std::endl;
        }
        out.close();

        FileIntegrityManager::signFile(path);
    } catch (std::exception ex) {
        MessageManager::displayMessage("Error saving unlocks: " + (std::string)ex.what(), 5, ERR);
    }
}

void ConditionalUnlockManager::hardResetUnlocks() {
    _unlockedItems.clear();
    for (auto& unlockProgress : _unlockProgress) {
        unlockProgress.second.progress = 0.f;
    }
    saveUnlockedItems();
}

void ConditionalUnlockManager::softResetUnlocks() {
    saveUnlockedItems();
    _unlockedItems.clear();
    for (auto& unlockProgress : _unlockProgress) {
        unlockProgress.second.progress = 0.f;
    }
}

void ConditionalUnlockManager::catItemUsed(unsigned int itemId) {
    if (DISABLE_UNLOCKS) return;

    for (int i = 0; i < 3; i++) {
        const unsigned int id = _catItems[i];
        if (id == itemId) return;

        if (id == 0) {
            _catItems[i] = itemId;
            if (i == 2) increaseUnlockProgress("Cassidy's Brain Cell", 1);
            return;
        }
    }
}
