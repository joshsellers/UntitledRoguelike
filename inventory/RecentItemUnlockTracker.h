#ifndef _RECENT_ITEM_UNLOCK_TRACKER_H
#define _RECENT_ITEM_UNLOCK_TRACKER_H

#include <vector>

class RecentItemUnlockTracker {
public:
    static void itemUnlocked(unsigned int itemId);
    static bool isRecentUnlock(unsigned int itemId);

    static void itemSeen(unsigned int itemId);

    static void onWaveCleared();

    static void reset();

    friend class SaveManager;
private:
    // itemId, hasBeenSeen
    static inline std::vector<std::pair<unsigned int, bool>> _recentItemIds;
};

#endif