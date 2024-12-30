#ifndef _CONDITIONAL_UNLOCK_MANAGER_H
#define _CONDITIONAL_UNLOCK_MANAGER_H

#include <vector>
#include <string>

class ConditionalUnlockManager {
public:
    static const bool isUnlocked(std::string itemName);
    static const void unlockItem(std::string itemName);

    static void loadUnlockedItems();
    static void saveUnlockedItems();

    static void resetUnlocks();
private:
    static inline std::vector<std::string> _unlockedItems;
};

#endif