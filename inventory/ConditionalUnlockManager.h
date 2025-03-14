#ifndef _CONDITIONAL_UNLOCK_MANAGER_H
#define _CONDITIONAL_UNLOCK_MANAGER_H

#include <vector>
#include <string>
#include <map>

struct UnlockProgressTracker {
    UnlockProgressTracker(const float valueToUnlock) : valueToUnlock(valueToUnlock) {}

    const float valueToUnlock;
    float progress = 0.f;
};

class ConditionalUnlockManager {
public:
    static const bool isUnlocked(std::string itemName);
    static void increaseUnlockProgress(std::string itemName, float progress);

    static void loadUnlockedItems();
    static void saveUnlockedItems();

    static void resetUnlocks();

    static void catItemUsed(unsigned int itemId);

    friend class Game;
    friend class SaveManager;
private:
    static inline std::vector<std::string> _unlockedItems;

    static void unlockItem(std::string itemName);

    static std::map<std::string, UnlockProgressTracker> _unlockProgress;

    static inline unsigned int _catItems[3] = { 0, 0, 0 };
};

#endif