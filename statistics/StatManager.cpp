#include "StatManager.h"
#include "../core/Util.h"
#include <fstream>
#include "../core/MessageManager.h"
#include "AchievementManager.h"

float StatManager::getOverallStat(STATISTIC stat) {
    return _STATS_OVERALL[stat];
}

float StatManager::getStatThisRun(STATISTIC stat) {
    return _STATS_THIS_RUN[stat];
}

void StatManager::increaseStat(STATISTIC stat, float amt) {
    if (DISABLE_STATS) return;

    _STATS_OVERALL[stat] += amt;
    _STATS_THIS_RUN[stat] += amt;

    AchievementManager::checkAchievementsOnStatIncrease(stat, getStatThisRun(stat));
}

void StatManager::loadOverallStats() {
    if (SELECTED_SAVE_FILE == SAVE_FILE_NOT_SELECTED) {
        MessageManager::displayMessage("Tried to load stats before save file was selected", 5, WARN);
        return;
    }

    std::string path = getLocalLowPath() + "\\save" + std::to_string(SELECTED_SAVE_FILE);
    if (!std::filesystem::is_directory(path + "\\")) {
        std::filesystem::create_directory(path);
    }

    path += "\\stats.config";
    std::ifstream in(path);

    if (!in.good()) {
        MessageManager::displayMessage("Could not find stats file", 5, DEBUG);
        in.close();
    } else {
        std::string line;
        int lineNumber = 0;
        while (getline(in, line)) {
            try {
                _STATS_OVERALL[lineNumber] = std::stof(line);
            } catch (std::exception ex) {
                MessageManager::displayMessage("Error reading stats.config: " + (std::string)ex.what(), 5, ERR);
            }

            lineNumber++;
        }

        in.close();
    }
}

void StatManager::saveOverallStats() {
    if (DISABLE_STATS) return;

    if (SELECTED_SAVE_FILE == SAVE_FILE_NOT_SELECTED) {
        MessageManager::displayMessage("Tried to save stats before save file was selected", 5, WARN);
        return;
    }

    std::string path = getLocalLowPath() + "\\save" + std::to_string(SELECTED_SAVE_FILE);
    if (!std::filesystem::is_directory(path + "\\")) {
        std::filesystem::create_directory(path);
    }

    path += "\\stats.config";

    try {
        if (!std::filesystem::remove(path)) {
            MessageManager::displayMessage("Could not replace stats file", 5, DEBUG);
        }
    } catch (const std::filesystem::filesystem_error& err) {
        MessageManager::displayMessage("Could not replace stats file: " + (std::string)err.what(), 5, ERR);
    }

    try {
        std::ofstream out(path);
        for (int i = 0; i < NUM_STATS; i++) {
            out << std::to_string(getOverallStat((STATISTIC)i)) << std::endl;
        }
        out.close();
    } catch (std::exception ex) {
        MessageManager::displayMessage("Error saving stats: " + (std::string)ex.what(), 5, ERR);
    }
}

void StatManager::setStatThisRun(STATISTIC stat, float val) {
    if (DISABLE_STATS) return;

    _STATS_THIS_RUN[stat] = val;
}

void StatManager::setOverallStat(STATISTIC stat, float val) {
    if (DISABLE_STATS) return;

    _STATS_OVERALL[stat] = val;
}

void StatManager::resetStatsForThisRun() {
    for (int i = 0; i < NUM_STATS; i++) {
        setStatThisRun((STATISTIC)i, 0);
    }
}

void StatManager::resetOverallStats() {
    for (int i = 0; i < NUM_STATS; i++) {
        setOverallStat((STATISTIC)i, 0);
    }
}
