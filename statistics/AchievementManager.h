#ifndef _ACHIEVEMENT_MANAGER_H
#define _ACHIEVEMENT_MANAGER_H

#include "../../SteamworksHeaders/steam_api.h"
#include <vector>
#include <string>
#include "Statistic.h"

constexpr int NUM_ACHIEVEMENTS = 49;
enum ACHIEVEMENT {
    MILLIONAIRE,
    DEFEAT_CHEESEBOSS,
    HARDMODE_DEFEAT_CHEESEBOSS,
    DEFEAT_CANNONBOSS,
    HARDMODE_DEFEAT_CANNONBOSS,
    TRIATHLON,
    MARATHON,
    AROUND_THE_WORLD,
    SURVIVOR,
    BUSINESSPERSON,
    TRIAL_AND_ERROR,
    EXTERMINATOR,
    UNSTOPPABLE,
    TRIGGER_HAPPY,
    TENACIOUS,
    SLIPPERY,
    UNTOUCHABLE,
    FLESHY,
    DEFEAT_TREEBOSS,
    HARDMODE_DEFEAT_TREEBOSS,
    DEFEAT_CREAMBOSS,
    HARDMODE_DEFEAT_CREAMBOSS,
    DEFEAT_CHEFBOSS,
    HARDMODE_DEFEAT_CHEFBOSS,
    HEARTBREAKER,
    HUMAN_BOULDER,
    HARDMODE_UNSTOPPABLE,
    HARDMODE_SLIPPERY,
    HARDMODE_UNTOUCHABLE,
    DEFEAT_BABYBOSS,
    HARDMODE_DEFEAT_BABYBOSS,
    MASOCHIST,
    DEFEAT_TEETHBOSS,
    HARDMODE_DEFEAT_TEETHBOSS,
    DEFEAT_SHROOMBOSS,
    HARDMODE_DEFEAT_SHROOMBOSS,
    ECONOMICAL,
    SHROOMY,
    SLAUGHTERER,
    BREAKING_AND_ENTERING,
    DEFEAT_FROGBOSS,
    HARDMODE_DEFEAT_FROGBOSS,
    DEFEAT_OCTOPUSBOSS,
    HARDMODE_DEFEAT_OCTOPUSBOSS,
    TREECKSTER,
    DEFEAT_PENGUINBOSS,
    HARDMODE_DEFEAT_PENGUINBOSS,
    DEFEAT_DEVBOSS,
    HARDMODE_DEFEAT_DEVBOSS
};

class AchievementManagerInstance {
public:
    void start();

    void unlock(ACHIEVEMENT achievement, bool callLocalManager = true);

    void resetAchievements();

    bool achievementsReady() const;

    void migrateAchievements();

private:
    bool isUnlocked(ACHIEVEMENT achievement);

    STEAM_CALLBACK(AchievementManagerInstance, onUserStatsReceived, UserStatsReceived_t);
    STEAM_CALLBACK(AchievementManagerInstance, onStatStored, UserStatsStored_t);
    STEAM_CALLBACK(AchievementManagerInstance, onAchievementStored, UserAchievementStored_t);

    bool _achievementsReady = false;

    inline static bool _achievementStats[NUM_ACHIEVEMENTS];

    bool _needToMigrateAchivements = false;
};

class AchievementManager {
public:
    static AchievementManagerInstance& getInstance() {
        return _instance;
    }

    inline static const std::vector<std::string> achievementSteamIds = {
        "ACH_MILLIONAIRE",
        "ACH_CHEESEBOSS",
        "ACH_HARDMODE_CHEESEBOSS",
        "ACH_CANNONBOSS",
        "ACH_HARDMODE_CANNONBOSS",
        "ACH_TRIATHLON",
        "ACH_MARATHON",
        "ACH_AROUND_THE_WORLD",
        "ACH_SURVIVOR",
        "ACH_BUSINESSPERSON",
        "ACH_TRIAL_AND_ERROR",
        "ACH_EXTERMINATOR",
        "ACH_UNSTOPPABLE",
        "ACH_TRIGGER_HAPPY",
        "ACH_TENACIOUS",
        "ACH_SLIPPERY",
        "ACH_UNTOUCHABLE",
        "ACH_FLESHY",
        "ACH_TREEBOSS",
        "ACH_HARDMODE_TREEBOSS",
        "ACH_CREAMBOSS",
        "ACH_HARDMODE_CREAMBOSS",
        "ACH_CHEFBOSS",
        "ACH_HARDMODE_CHEFBOSS",
        "ACH_HEARTBREAKER",
        "ACH_HUMAN_BOULDER",
        "ACH_HARDMODE_UNSTOPPABLE",
        "ACH_HARDMODE_SLIPPERY",
        "ACH_HARDMODE_UNTOUCHABLE",
        "ACH_BABYBOSS",
        "ACH_HARDMODE_BABYBOSS",
        "ACH_MASOCHIST",
        "ACH_TEETHBOSS",
        "ACH_HARDMODE_TEETHBOSS",
        "ACH_SHROOMBOSS",
        "ACH_HARDMODE_SHROOMBOSS",
        "ACH_ECONOMICAL",
        "ACH_SHROOMY",
        "ACH_SLAUGHTERER",
        "ACH_BREAKING_AND_ENTERING",
        "ACH_FROGBOSS",
        "ACH_HARDMODE_FROGBOSS",
        "ACH_OCTOPUSBOSS",
        "ACH_HARDMODE_OCTOPUSBOSS",
        "ACH_TREECKSTER",
        "ACH_PENGUINBOSS",
        "ACH_HARDMODE_PENGUINBOSS",
        "ACH_DEVBOSS",
        "ACH_HARDMODE_DEVBOSS"
    };

    static void start() {
        getInstance().start();
    }

    static void unlock(ACHIEVEMENT achievement, bool callLocalManager = true) {
        getInstance().unlock(achievement, callLocalManager);
    }

    static void resetAchievements() {
        getInstance().resetAchievements();
    }

    static bool achievementsReady() {
        return getInstance().achievementsReady();
    }

    static void checkAchievementsOnStatIncrease(STATISTIC stat, float valueThisSave);

    static inline bool bossIsActive = false;

private:
    inline static AchievementManagerInstance _instance;
};

#endif
