#ifndef _STAT_MANAGER_H
#define _STAT_MANAGER_H

#include "Statistic.h"

class StatManager {
public:
    inline const static unsigned int NUM_STATS = 14;

    static float getOverallStat(STATISTIC stat);
    static float getStatThisSave(STATISTIC stat);

    static void increaseStat(STATISTIC stat, float amt);

    static void loadOverallStats();
    static void saveOverallStats();

    static void setStatThisSave(STATISTIC stat, float val);
    static void resetStatsForThisSave();

private:
    inline static float _STATS_OVERALL[NUM_STATS] = {};
    inline static float _STATS_THIS_SAVE[NUM_STATS] = {};
};

#endif