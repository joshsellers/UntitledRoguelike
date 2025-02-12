#include "AchievementManager.h"
#include "../core/MessageManager.h"
#include "StatManager.h"
#include "../core/Tutorial.h"
#include "../inventory/ConditionalUnlockManager.h"

void AchievementManagerInstance::start() {
    if (!STEAMAPI_INITIATED) return;
    SteamUserStats()->RequestCurrentStats();
}

void AchievementManagerInstance::unlock(ACHIEVEMENT achievement) {
    if (!STEAMAPI_INITIATED || !_achievementsReady) return;
    else if (isUnlocked(achievement)) {
        return;
    }

    if (!SteamUserStats()->SetAchievement(AchievementManager::achievementNames.at(achievement).c_str())) {
        MessageManager::displayMessage("Failed to set achievement: " + AchievementManager::achievementNames.at(achievement), 5, WARN);
    } else if (!SteamUserStats()->StoreStats()) {
        MessageManager::displayMessage("Failed to store achievements", 5, WARN);
    } else {
        _achievementStats[achievement] = true;
    }
}

void AchievementManagerInstance::resetAchievements() {
    if (!STEAMAPI_INITIATED || !_achievementsReady) return;

    if (!SteamUserStats()->ResetAllStats(true)) {
        MessageManager::displayMessage("Failed to reset achievements", 5, WARN);
    } else {
        MessageManager::displayMessage("Reset achievments", 5, DEBUG);
        start();
    }
}

bool AchievementManagerInstance::isUnlocked(ACHIEVEMENT achievement) {
    if (!STEAMAPI_INITIATED || !_achievementsReady) return false;

    return _achievementStats[achievement];
}

void AchievementManagerInstance::onUserStatsReceived(UserStatsReceived_t* pCallback) {
    MessageManager::displayMessage("Recieved achievement data: " + std::to_string(pCallback->m_eResult), 2, DEBUG);
    _achievementsReady = pCallback->m_eResult == k_EResultOK;

    if (_achievementsReady) {
        for (int i = 0; i < NUM_ACHIEVEMENTS; i++) {
            bool temp = false;
            bool* achieved = &temp;
            if (!SteamUserStats()->GetAchievement(AchievementManager::achievementNames.at(i).c_str(), achieved)) {
                MessageManager::displayMessage("Failed to get achievement status", 5, WARN);
            }

            _achievementStats[i] = *achieved;
        }

        MessageManager::displayMessage("Achievements ready", 2, DEBUG);
    }
}

void AchievementManagerInstance::onStatStored(UserStatsStored_t* pCallback) {
    MessageManager::displayMessage("StoreStats returned " + std::to_string(pCallback->m_eResult), 2, DEBUG);
}

void AchievementManagerInstance::onAchievementStored(UserAchievementStored_t* pCallback) {
    MessageManager::displayMessage("Achievement set: " + std::string(pCallback->m_rgchAchievementName), 5, DEBUG);
}

void AchievementManager::checkAchievementsOnStatIncrease(STATISTIC stat, float valueThisSave) {
    if (stat == PENNIES_COLLECTED && valueThisSave >= 1000000) {
        unlock(MILLIONAIRE);
        ConditionalUnlockManager::increaseUnlockProgress("Penny Cannon", 1);
    } else if ((stat == DIST_TRAVELLED || stat == DIST_SWAM || stat == DIST_SAILED)
        && StatManager::getOverallStat(DIST_TRAVELLED) >= 20000
        && StatManager::getOverallStat(DIST_SWAM) >= 20000
        && StatManager::getOverallStat(DIST_SAILED) >= 20000) {
        unlock(TRIATHLON);
    } else if (stat == DIST_TRAVELLED && StatManager::getOverallStat(DIST_TRAVELLED) >= 40000000) {
        unlock(AROUND_THE_WORLD);
    } else if (stat == DIST_TRAVELLED && StatManager::getOverallStat(DIST_TRAVELLED) >= 42000) {
        unlock(MARATHON);
    } else if ((stat == ITEMS_PURCHASED || stat == ITEMS_SOLD)
        && StatManager::getOverallStat(ITEMS_PURCHASED) >= 1000 && StatManager::getOverallStat(ITEMS_SOLD) >= 1000) {
        unlock(BUSINESSPERSON);
        ConditionalUnlockManager::increaseUnlockProgress("Order Form", 1);
    } else if (stat == TIMES_DIED && !Tutorial::isCompleted() && valueThisSave == 10) {
        unlock(TENACIOUS);
    } else if (stat == TIMES_DIED && StatManager::getOverallStat(TIMES_DIED) == 100) {
        unlock(TRIAL_AND_ERROR);
    } else if (stat == ENEMIES_DEFEATED && valueThisSave == 5000) {
        unlock(EXTERMINATOR);
        ConditionalUnlockManager::increaseUnlockProgress("Cassidy's Tail", 1);
    } else if (stat == SHOTS_FIRED && valueThisSave == 1000) {
        unlock(TRIGGER_HAPPY);
        ConditionalUnlockManager::increaseUnlockProgress("Quantum Visor", 1);
    } else if (stat == DAMAGE_TAKEN) {
        _wavesWithoutDamage = 0;
        _tookDamageThisWave = true;
        if (StatManager::getOverallStat(DAMAGE_TAKEN) >= 50000) unlock(MASOCHIST);
    } else if (stat == WAVES_CLEARED) {
        if (!_tookDamageThisWave) {
            _wavesWithoutDamage++;
        }
        _tookDamageThisWave = false;

        if (_wavesWithoutDamage == 5) {
            unlock(SLIPPERY);
            if (HARD_MODE_ENABLED) unlock(HARDMODE_SLIPPERY);
        } else if (_wavesWithoutDamage == 10) {
            unlock(UNTOUCHABLE);
            if (HARD_MODE_ENABLED) {
                unlock(HARDMODE_UNTOUCHABLE);
                ConditionalUnlockManager::increaseUnlockProgress("Dev's Blessing", 1);
            }
        }
    } else if (stat == TIMES_ROLLED && StatManager::getOverallStat(TIMES_ROLLED) >= 20000) {
        unlock(HUMAN_BOULDER);
    }
}
