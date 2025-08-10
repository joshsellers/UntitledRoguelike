#include "RecentItemUnlockTracker.h"

void RecentItemUnlockTracker::itemUnlocked(unsigned int itemId) {
    _recentItemIds.push_back({ itemId, false });
}

bool RecentItemUnlockTracker::isRecentUnlock(unsigned int itemId) {
    for (const auto& item : _recentItemIds) {
        if (item.first == itemId) return true;
    }

    return false;
}

void RecentItemUnlockTracker::itemSeen(unsigned int itemId) {
    for (auto& item : _recentItemIds) {
        if (item.first == itemId) {
            item.second = true;
        }
    }
}

void RecentItemUnlockTracker::onWaveCleared() {
    std::vector<unsigned int> unseenItems;
    for (const auto& item : _recentItemIds) {
        if (item.second == false) unseenItems.push_back(item.first);
    }

    _recentItemIds.clear();

    for (const auto unseenItem : unseenItems) _recentItemIds.push_back({ unseenItem, false });
}

void RecentItemUnlockTracker::reset() {
    _recentItemIds.clear();
}
