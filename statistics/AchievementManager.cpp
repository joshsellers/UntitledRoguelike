#include "AchievementManager.h"
#include "../core/MessageManager.h"
#include "StatManager.h"

void AchievementManagerInstance::start() {
    if (!STEAMAPI_INITIATED) return;
    SteamUserStats()->RequestCurrentStats();
}

void AchievementManagerInstance::unlock(ACHIEVEMENT achievement) {
    if (!STEAMAPI_INITIATED || !_achievementsReady) return;
    else if (isUnlocked(achievement)) {
        MessageManager::displayMessage("Achievement " + AchievementManager::achievementNames.at(achievement) + " already unlocked", 1, DEBUG);
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
    } else if ((stat == DIST_TRAVELLED || stat == DIST_SWAM || stat == DIST_SAILED)
        && StatManager::getStatThisSave(DIST_TRAVELLED) >= 20000
        && StatManager::getStatThisSave(DIST_SWAM) >= 20000
        && StatManager::getStatThisSave(DIST_SAILED) >= 20000) {
        unlock(TRIATHLON);
    } else if (stat == DIST_TRAVELLED && valueThisSave >= 40000000) {
        unlock(AROUND_THE_WORLD);
    } else if (stat == DIST_TRAVELLED && valueThisSave >= 42000) {
        unlock(MARATHON);
    } else if ((stat == ITEMS_PURCHASED || stat == ITEMS_SOLD)
        && StatManager::getStatThisSave(ITEMS_PURCHASED) >= 1000 && StatManager::getStatThisSave(ITEMS_SOLD) >= 1000) {
        unlock(BUSINESSPERSON);
    } else if (stat == TIMES_DIED && StatManager::getOverallStat(TIMES_DIED) == 100) {
        unlock(TRIAL_AND_ERROR);
    } else if (stat == ENEMIES_DEFEATED && valueThisSave == 5000) {
        unlock(EXTERMINATOR);
    } else if (stat == SHOTS_FIRED && valueThisSave == 1000) {
        unlock(TRIGGER_HAPPY);
    }
}
