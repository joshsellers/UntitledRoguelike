#ifndef _MOB_SPAWN_DATA_H
#define _MOB_SPAWN_DATA_H

#include "Chunk.h"

enum class MOB_TYPE : int {
    PENGUIN = 0,
    TURTLE = 1,
    PLANT_MAN = 2
};

struct MobSpawnData {
    MobSpawnData(const MOB_TYPE mobType, const int spawnChance, const int minPackSize, const int maxPackSize)
        : mobType(mobType), spawnChance(spawnChance), minPackSize(minPackSize), maxPackSize(maxPackSize)
    {}
    
    const MOB_TYPE mobType;
    const int spawnChance;
    const int minPackSize;
    const int maxPackSize;
};

struct BiomeMobSpawnData {
    BiomeMobSpawnData(const TERRAIN_TYPE biome, const std::vector<MobSpawnData> mobData)
        : biome(biome), mobData(mobData)
    {}

    const TERRAIN_TYPE biome;
    const std::vector<MobSpawnData> mobData;
};

#endif