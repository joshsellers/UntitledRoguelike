#include "AchievementManager.h"
#include "../core/MessageManager.h"
#include "StatManager.h"
#include "../core/Tutorial.h"
#include "../inventory/ConditionalUnlockManager.h"
#include "LocalAchievementManager.h"
#include "../core/SoundManager.h"

void AchievementManagerInstance::start() {
    if (!STEAMAPI_INITIATED) return;
    SteamUserStats()->RequestCurrentStats();
}

void AchievementManagerInstance::unlock(ACHIEVEMENT achievement, bool callLocalManager) {
    if (callLocalManager) LocalAchievementManager::unlock(achievement);

    if (!STEAMAPI_INITIATED || !_achievementsReady || DISABLE_ACHIEVEMENTS) return;
    else if (isUnlocked(achievement)) {
        return;
    }

    if (!SteamUserStats()->SetAchievement(AchievementManager::achievementSteamIds.at(achievement).c_str())) {
        MessageManager::displayMessage("Failed to set achievement: " + AchievementManager::achievementSteamIds.at(achievement), 5, WARN);
    } else if (!SteamUserStats()->StoreStats()) {
        MessageManager::displayMessage("Failed to store achievements", 5, WARN);
    } else {
        _achievementStats[achievement] = true;
    }
}

void AchievementManagerInstance::resetAchievements() {
    if (!STEAMAPI_INITIATED || !_achievementsReady || DISABLE_ACHIEVEMENTS) return;

    if (!SteamUserStats()->ResetAllStats(true)) {
        MessageManager::displayMessage("Failed to reset achievements", 5, WARN);
    } else {
        MessageManager::displayMessage("Reset achievments", 5, DEBUG);
        start();
    }
}

bool AchievementManagerInstance::achievementsReady() const {
    return _achievementsReady;
}

void AchievementManagerInstance::migrateAchievements() {
    _needToMigrateAchivements = true;
}

bool AchievementManagerInstance::isUnlocked(ACHIEVEMENT achievement) {
    if (!STEAMAPI_INITIATED || !_achievementsReady || DISABLE_ACHIEVEMENTS) return false;

    return _achievementStats[achievement];
}

void AchievementManagerInstance::onUserStatsReceived(UserStatsReceived_t* pCallback) {
    MessageManager::displayMessage("Recieved achievement data: " + std::to_string(pCallback->m_eResult), 2, DEBUG);
    _achievementsReady = pCallback->m_eResult == k_EResultOK;

    if (_achievementsReady) {
        for (int i = 0; i < NUM_ACHIEVEMENTS; i++) {
            bool temp = false;
            bool* achieved = &temp;
            if (!SteamUserStats()->GetAchievement(AchievementManager::achievementSteamIds.at(i).c_str(), achieved)) {
                MessageManager::displayMessage("Failed to get achievement status", 5, WARN);
            }

            _achievementStats[i] = *achieved;
        }

        MessageManager::displayMessage("Achievements ready", 2, DEBUG);
    } 
    
    if (_needToMigrateAchivements) {
        if (!STEAMAPI_INITIATED || !_achievementsReady || DISABLE_ACHIEVEMENTS) {
            MessageManager::displayMessage("Failed to migrate achivements", 5, WARN);
            return;
        }

        for (int i = 0; i < NUM_ACHIEVEMENTS; i++) {
            LocalAchievementManager::_unlockedAchievements[i] = _achievementStats[i];
        }
        SELECTED_SAVE_FILE = 0;
        LocalAchievementManager::saveLocalAchievements();
        SELECTED_SAVE_FILE = SAVE_FILE_NOT_SELECTED;

        _needToMigrateAchivements = false;
    }
}

void AchievementManagerInstance::onStatStored(UserStatsStored_t* pCallback) {
    MessageManager::displayMessage("StoreStats returned " + std::to_string(pCallback->m_eResult), 2, DEBUG);
}

void AchievementManagerInstance::onAchievementStored(UserAchievementStored_t* pCallback) {
    MessageManager::displayMessage("Achievement set: " + std::string(pCallback->m_rgchAchievementName), 5, DEBUG);
}

void AchievementManager::checkAchievementsOnStatIncrease(STATISTIC stat, float valueThisSave) {
    if (stat == PENNIES_COLLECTED && StatManager::getOverallStat(PENNIES_COLLECTED) >= 1000000) {
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
    } else if (stat == TIMES_DIED && !Tutorial::isCompleted() && valueThisSave == 5) {
        unlock(TENACIOUS);
    } else if (stat == TIMES_DIED && StatManager::getOverallStat(TIMES_DIED) == 20) {
        unlock(TRIAL_AND_ERROR);
        ConditionalUnlockManager::increaseUnlockProgress("Reset Button", 1);
    } else if (stat == ENEMIES_DEFEATED && valueThisSave == 5000) {
        unlock(EXTERMINATOR);
        ConditionalUnlockManager::increaseUnlockProgress("Cassidy's Tail", 1);
        if (HARD_MODE_ENABLED) {
            unlock(SLAUGHTERER);
            ConditionalUnlockManager::increaseUnlockProgress("Rebound Jewel", 1);
        }
    } else if (stat == SHOTS_FIRED && valueThisSave == 1000) {
        unlock(TRIGGER_HAPPY);
        ConditionalUnlockManager::increaseUnlockProgress("Quantum Visor", 1);
    } else if (stat == DAMAGE_TAKEN) {
        if (!bossIsActive) {
            StatManager::setStatThisRun(WAVES_WITHOUT_DAMAGE, 0);
            StatManager::setStatThisRun(TOOK_DAMAGE_THIS_WAVE, true);
        }

        if (StatManager::getOverallStat(DAMAGE_TAKEN) >= 10000) {
            unlock(MASOCHIST);
            ConditionalUnlockManager::increaseUnlockProgress("Defibrillator Sandwich", 1);
        }
    } else if (stat == WAVES_CLEARED) {
        if (!StatManager::getStatThisRun(TOOK_DAMAGE_THIS_WAVE)) {
            StatManager::setStatThisRun(WAVES_WITHOUT_DAMAGE, StatManager::getStatThisRun(WAVES_WITHOUT_DAMAGE) + 1);
        }
        StatManager::setStatThisRun(TOOK_DAMAGE_THIS_WAVE, false);

        if (StatManager::getStatThisRun(WAVES_WITHOUT_DAMAGE) == 5) {
            unlock(SLIPPERY);
            if (HARD_MODE_ENABLED) unlock(HARDMODE_SLIPPERY);
        } else if (StatManager::getStatThisRun(WAVES_WITHOUT_DAMAGE) == 10) {
            unlock(UNTOUCHABLE);
            ConditionalUnlockManager::increaseUnlockProgress("Coupon", 1);
            if (HARD_MODE_ENABLED) {
                if (!LocalAchievementManager::isUnlocked(HARDMODE_UNTOUCHABLE)) {
                    MessageManager::displayMessage("You unlocked the Altar!", 8, SPECIAL);
                    SoundManager::playSound("itemunlock");
                }
                unlock(HARDMODE_UNTOUCHABLE);
            }
        }
    } else if (stat == TIMES_ROLLED && StatManager::getOverallStat(TIMES_ROLLED) >= 15000) {
        unlock(HUMAN_BOULDER);
        ConditionalUnlockManager::increaseUnlockProgress("Totem of Rolling", 1);
    } else if (stat == ATM_AMOUNT && StatManager::getOverallStat(ATM_AMOUNT) == 999) {
        unlock(ECONOMICAL);
        ConditionalUnlockManager::increaseUnlockProgress("Debit Card", 1);
    } else if (stat == SHOPS_BLOWN_UP && StatManager::getOverallStat(SHOPS_BLOWN_UP) == 10) {
        unlock(BREAKING_AND_ENTERING);
        ConditionalUnlockManager::increaseUnlockProgress("Ski Mask", 1);
        ConditionalUnlockManager::increaseUnlockProgress("Heavy Duty Boots", 1);
    }
}
