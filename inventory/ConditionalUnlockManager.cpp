#include "ConditionalUnlockManager.h"
#include "Item.h"
#include "../core/MessageManager.h"
#include "../core/Util.h"
#include "../statistics/StatManager.h"

std::map<std::string, UnlockProgressTracker> ConditionalUnlockManager::_unlockProgress = {
    {"Cyclops Eye", 100},
    {"Chef's Hat", 5}
};

const bool ConditionalUnlockManager::isUnlocked(std::string itemName) {
    for (const auto& unlockedItemName : _unlockedItems) {
        if (unlockedItemName == itemName) return true;
    }
    return false;
}

void ConditionalUnlockManager::unlockItem(std::string itemName) {
    if (isUnlocked(itemName)) return;

    for (const auto& item : Item::ITEMS) {
        if (item->getName() == itemName) {
            _unlockedItems.push_back(itemName);
            StatManager::increaseStat(ITEMS_UNLOCKED, 1);
            saveUnlockedItems();
            MessageManager::displayMessage("You've unlocked a new item!", 5, SPECIAL);
            return;
        }
    }

    MessageManager::displayMessage("No item named \"" + itemName + "\"", 5, WARN);
}

void ConditionalUnlockManager::increaseUnlockProgress(std::string itemName, float progress) {
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
    std::string path = getLocalLowPath() + "\\unlocks.config";
    std::ifstream in(path);

    if (!in.good()) {
        MessageManager::displayMessage("Could not find unlocks file", 5, DEBUG);
        in.close();
    } else {
        std::string line;
        while (getline(in, line)) {
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
    std::string path = getLocalLowPath() + "\\unlocks.config";

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
    } catch (std::exception ex) {
        MessageManager::displayMessage("Error saving unlocks: " + (std::string)ex.what(), 5, ERR);
    }
}

void ConditionalUnlockManager::resetUnlocks() {
    _unlockedItems.clear();
    for (auto& unlockProgress : _unlockProgress) {
        unlockProgress.second.progress = 0.f;
    }
    saveUnlockedItems();
}
