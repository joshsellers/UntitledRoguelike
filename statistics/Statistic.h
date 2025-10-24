#ifndef _STATISTIC_H
#define _STATISTIC_H

#include <map>
#include <string>
#include <vector>

enum STATISTIC {
    DIST_TRAVELLED,
    DIST_SWAM,
    DIST_SAILED,
    ENEMIES_DEFEATED,
    BOSSES_DEFEATED,
    DAMAGE_TAKEN,
    DAMAGE_DEALT,
    TIMES_DIED,
    WAVES_CLEARED,
    SHOTS_FIRED,
    PENNIES_COLLECTED,
    TIMES_ROLLED,
    ITEMS_PURCHASED,
    ITEMS_SOLD,
    HIGHEST_WAVE_REACHED,
    ITEMS_UNLOCKED,
    ATM_AMOUNT,
    SHOPS_BLOWN_UP,
    WAVES_WITHOUT_DAMAGE,
    TOOK_DAMAGE_THIS_WAVE
};

inline std::map<STATISTIC, std::string> STAT_NAMES = {
    {DIST_TRAVELLED,        "Distance travelled"},
    {DIST_SWAM,             "Distance swam"},
    {DIST_SAILED,           "Distance sailed"},
    {ENEMIES_DEFEATED,      "Enemies defeated"},
    {BOSSES_DEFEATED,       "Bosses defeated"},
    {DAMAGE_TAKEN,          "Damage taken"},
    {DAMAGE_DEALT,          "Damage dealt"},
    {TIMES_DIED,            "Deaths"},
    {WAVES_CLEARED,         "Waves cleared"},
    {SHOTS_FIRED,           "Shots fired"},
    {PENNIES_COLLECTED,     "Pennies looted"},
    {TIMES_ROLLED,          "Rolls"},
    {ITEMS_PURCHASED,       "Items bought"},
    {ITEMS_SOLD,            "Items sold"},
    {HIGHEST_WAVE_REACHED,  "Highest wave reached"},
    {ITEMS_UNLOCKED,        "Items unlocked"},
    {ATM_AMOUNT,            "ATM Balance"},
    {SHOPS_BLOWN_UP,        "Shop doors blown up"},
    {WAVES_WITHOUT_DAMAGE,  "Waves cleared without taking damage"},
    {TOOK_DAMAGE_THIS_WAVE, "Took damage this wave?"}
};

inline std::vector<STATISTIC> HIDDEN_STATS = {
    SHOPS_BLOWN_UP,
    WAVES_WITHOUT_DAMAGE,
    TOOK_DAMAGE_THIS_WAVE
};

#endif