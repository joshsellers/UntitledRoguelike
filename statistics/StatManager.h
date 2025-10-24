#ifndef _STAT_MANAGER_H
#define _STAT_MANAGER_H

#include "Statistic.h"

class StatManager {
public:
    inline const static unsigned int NUM_STATS = 20;

    static float getOverallStat(STATISTIC stat);
    static float getStatThisRun(STATISTIC stat);

    static void increaseStat(STATISTIC stat, float amt);

    static void loadOverallStats();
    static void saveOverallStats();

    static void setStatThisRun(STATISTIC stat, float val);
    static void setOverallStat(STATISTIC stat, float val);
    static void resetStatsForThisRun();
    static void resetOverallStats();

private:
    inline static float _STATS_OVERALL[NUM_STATS] = {};
    inline static float _STATS_THIS_RUN[NUM_STATS] = {};
};

#endif