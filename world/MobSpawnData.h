#ifndef _MOB_SPAWN_DATA_H
#define _MOB_SPAWN_DATA_H

#include "Chunk.h"

// put enemies after cactoid, non enemies before cactoid
enum class MOB_TYPE : int {
    PENGUIN,
    TURTLE,
    FROG,
    DOG,
    SHARK,
    CACTOID,
    PLANT_MAN,
    SNOW_MAN,
    YETI,
    SKELETON,
    CYCLOPS,
    FLESH_CHICKEN,
    CHEESE_BOSS,
    CANNON_BOSS,
    LOG_MONSTER
};

struct MobSpawnData {
    MobSpawnData(const MOB_TYPE mobType, const int spawnChance, const int minPackSize, const int maxPackSize, const int waveNumber)
        : mobType(mobType), spawnChance(spawnChance), minPackSize(minPackSize), maxPackSize(maxPackSize), waveNumber(waveNumber)
    {}
    
    const MOB_TYPE mobType;
    const int spawnChance;
    const int minPackSize;
    const int maxPackSize;
    const int waveNumber;
};

struct BiomeMobSpawnData {
    BiomeMobSpawnData(const TERRAIN_TYPE biome, const std::vector<MobSpawnData> mobData)
        : biome(biome), mobData(mobData)
    {}

    const TERRAIN_TYPE biome;
    const std::vector<MobSpawnData> mobData;
};

#endif