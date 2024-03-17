#ifndef _MOB_SPAWN_DATA_H
#define _MOB_SPAWN_DATA_H

#include "Chunk.h"

// put enemies after cactoid, non enemies before cactoid
enum class MOB_TYPE : int {
    PENGUIN,
    TURTLE,
    FROG,
    CACTOID,
    PLANT_MAN
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