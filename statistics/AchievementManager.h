#ifndef _ACHIEVEMENT_MANAGER_H
#define _ACHIEVEMENT_MANAGER_H

#include "../../SteamworksHeaders/steam_api.h"
#include <vector>
#include <string>

constexpr int NUM_ACHIEVEMENTS = 1;
enum ACHIEVEMENT {
    MILLIONAIRE
};

class AchievementManagerInstance {
public:
    void start();

    void unlock(ACHIEVEMENT achievement);

    void resetAchievements();
private:
    bool isUnlocked(ACHIEVEMENT achievement);

    STEAM_CALLBACK(AchievementManagerInstance, onUserStatsReceived, UserStatsReceived_t);
    STEAM_CALLBACK(AchievementManagerInstance, onStatStored, UserStatsStored_t);
    STEAM_CALLBACK(AchievementManagerInstance, onAchievementStored, UserAchievementStored_t);

    bool _achievementsReady = false;

    inline static bool _achievementStats[NUM_ACHIEVEMENTS];
};

class AchievementManager {
public:
    static AchievementManagerInstance& getInstance() {
        return _instance;
    }

    inline static const std::vector<std::string> achievementNames = {
        "ACH_MILLIONAIRE"
    };

private:
    inline static AchievementManagerInstance _instance;
};

#endif
