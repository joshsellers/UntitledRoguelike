#include "StatManager.h"
#include "../core/Util.h"
#include <fstream>
#include "../core/MessageManager.h"
#include "AchievementManager.h"

float StatManager::getOverallStat(STATISTIC stat) {
    return _STATS_OVERALL[stat];
}

float StatManager::getStatThisSave(STATISTIC stat) {
    return _STATS_THIS_SAVE[stat];
}

void StatManager::increaseStat(STATISTIC stat, float amt) {
    _STATS_OVERALL[stat] += amt;
    _STATS_THIS_SAVE[stat] += amt;

    if (stat == PENNIES_COLLECTED && getStatThisSave(PENNIES_COLLECTED) >= 1000000) {
        AchievementManager::unlock(MILLIONAIRE);
    }
}

void StatManager::loadOverallStats() {
    std::string path = getLocalLowPath() + "\\stats.config";
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
    std::string path = getLocalLowPath() + "\\stats.config";

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

void StatManager::setStatThisSave(STATISTIC stat, float val) {
    _STATS_THIS_SAVE[stat] = val;
}

void StatManager::resetStatsForThisSave() {
    for (int i = 0; i < NUM_STATS; i++) {
        setStatThisSave((STATISTIC)i, 0);
    }
}
