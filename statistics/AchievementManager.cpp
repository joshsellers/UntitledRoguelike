#include "AchievementManager.h"
#include "../core/MessageManager.h"

void AchievementManagerInstance::start() {
    SteamUserStats()->RequestCurrentStats();
}

void AchievementManagerInstance::unlock(ACHIEVEMENT achievement) {
    if (!STEAMAPI_INITIATED || !_achievementsReady) return;
    else if (isUnlocked(achievement)) {
        MessageManager::displayMessage("Achievement " + AchievementManager::achievementNames.at(achievement) + " already unlocked", 2, DEBUG);
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
    MessageManager::displayMessage("Achievment set: " + std::string(pCallback->m_rgchAchievementName), 5, DEBUG);
}