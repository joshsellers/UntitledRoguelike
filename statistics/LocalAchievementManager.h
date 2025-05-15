#ifndef _LOCAL_ACHIEVEMENT_MANAGER_H
#define _LOCAL_ACHIEVEMENT_MANAGER_H

#include "AchievementManager.h"

class LocalAchievementManager {
public:
    static void unlock(ACHIEVEMENT achievement);
    static bool isUnlocked(ACHIEVEMENT achievement);

    static void loadLocalAchievements();
    static void saveLocalAchievements();

    static void softReset();
    static void hardReset();

    static std::string getAchievementName(ACHIEVEMENT achievement);

    friend class AchievementManagerInstance;
private:
    static bool isReady();

    inline static bool _unlockedAchievements[NUM_ACHIEVEMENTS];

    inline static const std::vector<std::string> _achievementNames = {
        "Millionaire",
        "William Cheesequake",
        "William Cheesequake but harder",
        "Professor Hands",
        "Professor Hands but harder",
        "Triathlon",
        "Marathon",
        "Around The World",
        "Survivor",
        "Businessperson",
        "Trial and Error",
        "Exterminator",
        "Unstoppable",
        "Trigger Happy",
        "Tenacious",
        "Slippery",
        "Untouchable",
        "Fleshy",
        "Phillip McTree",
        "Phillip McTree but harder",
        "Arnold Creamston",
        "Arnold Creamston but harder",
        "Chef Pete",
        "Chef Pete but harder",
        "Heartbreaker",
        "Human Boulder",
        "Completely Unstoppable",
        "Very Slippery",
        "Completely Untouchable",
        "Lieutenant Timmy",
        "Lieutenant Timmy but harder",
        "Masochist",
        "Danny Smiles",
        "Danny Smiles but harder",
        "Anita",
        "Anita but harder",
        "Economical",
        "Shroomy",
        "Slaughterer",
        "Breaking and Entering"
    };
};

#endif