#ifndef _MOB_SPAWN_CONFIGS_H
#define _MOB_SPAWN_CONFIGS_H

#include "MobSpawnData.h"

const BiomeMobSpawnData MOB_SPAWN_DATA[8] = {
    BiomeMobSpawnData(TERRAIN_TYPE::WATER, {
        //MobSpawnData(MOB_TYPE::SHARK, 10, 1, 2)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::GRASS, {
        MobSpawnData(MOB_TYPE::TURTLE, 1, 1, 1, 0),
        MobSpawnData(MOB_TYPE::FROG, 5, 1, 3, 0),
        MobSpawnData(MOB_TYPE::DOG, 100, 1, 1, 0)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::TUNDRA, {
        MobSpawnData(MOB_TYPE::PENGUIN, 0, 4, 15, 0)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::DESERT, {
        MobSpawnData(MOB_TYPE::CACTOID, 10, 1, 1, 0)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::SAVANNA, {}),

    BiomeMobSpawnData(TERRAIN_TYPE::FLESH, {}),

    BiomeMobSpawnData(TERRAIN_TYPE::GRASS_FOREST, {
        MobSpawnData(MOB_TYPE::FROG, 6, 2, 4, 0)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::RIVER, {})
};

constexpr int CHEESE_BOSS_CHANCE = 18;
const BiomeMobSpawnData ENEMY_SPAWN_DATA[8] = {
    BiomeMobSpawnData(TERRAIN_TYPE::WATER, {}),

    BiomeMobSpawnData(TERRAIN_TYPE::GRASS, {
        MobSpawnData(MOB_TYPE::PLANT_MAN, 0, 2, 8, 0),
        MobSpawnData(MOB_TYPE::CHEESE_BOSS, CHEESE_BOSS_CHANCE, 1, 1, 50),
        MobSpawnData(MOB_TYPE::LOG_MONSTER, 4, 1, 1, 26)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::TUNDRA, {
        MobSpawnData(MOB_TYPE::SNOW_MAN, 0, 2, 6, 0),
        MobSpawnData(MOB_TYPE::YETI, 1, 4, 8, 0),
        MobSpawnData(MOB_TYPE::CHEESE_BOSS, CHEESE_BOSS_CHANCE, 1, 1, 50)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::DESERT, {
        MobSpawnData(MOB_TYPE::SKELETON, 0, 5, 12, 0),
        MobSpawnData(MOB_TYPE::CHEESE_BOSS, CHEESE_BOSS_CHANCE, 1, 1, 50)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::SAVANNA, {
        MobSpawnData(MOB_TYPE::PLANT_MAN, 0, 4, 10, 0),
        MobSpawnData(MOB_TYPE::CHEESE_BOSS, CHEESE_BOSS_CHANCE, 1, 1, 50),
        MobSpawnData(MOB_TYPE::LOG_MONSTER, 4, 1, 1, 26)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::FLESH, {
        MobSpawnData(MOB_TYPE::CYCLOPS, 0, 4, 5, 0),
        MobSpawnData(MOB_TYPE::FLESH_CHICKEN, 4, 1, 2, 0),
        MobSpawnData(MOB_TYPE::CHEESE_BOSS, CHEESE_BOSS_CHANCE, 1, 1, 50)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::GRASS_FOREST, {
        MobSpawnData(MOB_TYPE::SKELETON, 2, 3, 6, 0),
        MobSpawnData(MOB_TYPE::PLANT_MAN, 0, 5, 10, 0),
        MobSpawnData(MOB_TYPE::CHEESE_BOSS, CHEESE_BOSS_CHANCE, 1, 1, 50),
        MobSpawnData(MOB_TYPE::LOG_MONSTER, 4, 1, 1, 26)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::RIVER, {})
};

#endif