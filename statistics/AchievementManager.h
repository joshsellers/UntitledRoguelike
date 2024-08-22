#ifndef _ACHIEVEMENT_MANAGER_H
#define _ACHIEVEMENT_MANAGER_H

#include "../../SteamworksHeaders/steam_api.h"
#include <vector>
#include <string>
#include "Statistic.h"

constexpr int NUM_ACHIEVEMENTS = 12;
enum ACHIEVEMENT {
    MILLIONAIRE,
    DEFEAT_CHEESEBOSS,
    DEFEAT_CANNONBOSS,
    TRIATHLON,
    MARATHON,
    AROUND_THE_WORLD,
    SURVIVOR,
    BUSINESSPERSON,
    TRIAL_AND_ERROR,
    EXTERMINATOR,
    UNSTOPPABLE,
    TRIGGER_HAPPY
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
        "ACH_MILLIONAIRE",
        "ACH_CHEESEBOSS",
        "ACH_CANNONBOSS",
        "ACH_TRIATHLON",
        "ACH_MARATHON",
        "ACH_AROUND_THE_WORLD",
        "ACH_SURVIVOR",
        "ACH_BUSINESSPERSON",
        "ACH_TRIAL_AND_ERROR",
        "ACH_EXTERMINATOR",
        "ACH_UNSTOPPABLE",
        "ACH_TRIGGER_HAPPY"
    };

    static void start() {
        getInstance().start();
    }

    static void unlock(ACHIEVEMENT achievement) {
        getInstance().unlock(achievement);
    }

    static void resetAchievements() {
        getInstance().resetAchievements();
    }

    static void checkAchievementsOnStatIncrease(STATISTIC stat, float valueThisSave);

private:
    inline static AchievementManagerInstance _instance;

    inline static int _wavesWithoutDamage = 0;
};

#endif
