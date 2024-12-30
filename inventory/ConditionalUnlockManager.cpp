#include "ConditionalUnlockManager.h"
#include "Item.h"
#include "../core/MessageManager.h"
#include "../core/Util.h"

const bool ConditionalUnlockManager::isUnlocked(std::string itemName) {
    for (const auto& unlockedItemName : _unlockedItems) {
        if (unlockedItemName == itemName) return true;
    }
    return false;
}

const void ConditionalUnlockManager::unlockItem(std::string itemName) {
    if (isUnlocked(itemName)) return;

    for (const auto& item : Item::ITEMS) {
        if (item->getName() == itemName) {
            _unlockedItems.push_back(itemName);
            saveUnlockedItems();
            MessageManager::displayMessage("You've unlocked a new item!", 5);
            return;
        }
    }

    MessageManager::displayMessage("No item named \"" + itemName + "\"", 5, WARN);
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
            _unlockedItems.push_back(line);
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
        for (const auto& unlockedItemName : _unlockedItems) {
            out << unlockedItemName << std::endl;
        }
        out.close();
    } catch (std::exception ex) {
        MessageManager::displayMessage("Error saving unlocks: " + (std::string)ex.what(), 5, ERR);
    }
}

void ConditionalUnlockManager::resetUnlocks() {
    _unlockedItems.clear();
    saveUnlockedItems();
}
