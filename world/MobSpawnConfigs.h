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
constexpr int PROJ_ENEMIES_WAVE = 10;
constexpr int BOMB_BOY_WAVE = 20;
constexpr int MEGA_BOMB_BOY_WAVE = 32;
constexpr int BOMB_ENEMY_CHANCE = 16;

const BiomeMobSpawnData ENEMY_SPAWN_DATA[8] = {
    BiomeMobSpawnData(TERRAIN_TYPE::WATER, {}),

    BiomeMobSpawnData(TERRAIN_TYPE::GRASS, {
        MobSpawnData(MOB_TYPE::PLANT_MAN, 0, 2, 8, 0),
        //MobSpawnData(MOB_TYPE::CHEESE_BOSS, CHEESE_BOSS_CHANCE, 1, 1, 50),
        MobSpawnData(MOB_TYPE::TULIP_MONSTER, 3, 1, 2, PROJ_ENEMIES_WAVE),
        MobSpawnData(MOB_TYPE::BOMB_BOY, BOMB_ENEMY_CHANCE, 4, 8, BOMB_BOY_WAVE),
        MobSpawnData(MOB_TYPE::MEGA_BOMB_BOY, BOMB_ENEMY_CHANCE, 1, 4, MEGA_BOMB_BOY_WAVE)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::TUNDRA, {
        MobSpawnData(MOB_TYPE::SNOW_MAN, 0, 2, 6, 0),
        MobSpawnData(MOB_TYPE::YETI, 1, 4, 8, 0),
        //MobSpawnData(MOB_TYPE::CHEESE_BOSS, CHEESE_BOSS_CHANCE, 1, 1, 50),
        MobSpawnData(MOB_TYPE::BOULDER_BEAST, 3, 1, 2, PROJ_ENEMIES_WAVE),
        MobSpawnData(MOB_TYPE::BOMB_BOY, BOMB_ENEMY_CHANCE, 4, 8, BOMB_BOY_WAVE),
        MobSpawnData(MOB_TYPE::MEGA_BOMB_BOY, BOMB_ENEMY_CHANCE, 1, 4, MEGA_BOMB_BOY_WAVE)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::DESERT, {
        MobSpawnData(MOB_TYPE::SKELETON, 0, 5, 12, 0),
        //MobSpawnData(MOB_TYPE::CHEESE_BOSS, CHEESE_BOSS_CHANCE, 1, 1, 50),
        MobSpawnData(MOB_TYPE::BOULDER_BEAST, 3, 1, 2, PROJ_ENEMIES_WAVE),
        MobSpawnData(MOB_TYPE::BOMB_BOY, BOMB_ENEMY_CHANCE, 4, 8, BOMB_BOY_WAVE),
        MobSpawnData(MOB_TYPE::MEGA_BOMB_BOY, BOMB_ENEMY_CHANCE, 1, 4, MEGA_BOMB_BOY_WAVE)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::SAVANNA, {
        MobSpawnData(MOB_TYPE::PLANT_MAN, 0, 4, 10, 0),
        //MobSpawnData(MOB_TYPE::CHEESE_BOSS, CHEESE_BOSS_CHANCE, 1, 1, 50),
        MobSpawnData(MOB_TYPE::LOG_MONSTER, 3, 1, 2, PROJ_ENEMIES_WAVE),
        MobSpawnData(MOB_TYPE::BOMB_BOY, BOMB_ENEMY_CHANCE, 4, 8, BOMB_BOY_WAVE),
        MobSpawnData(MOB_TYPE::MEGA_BOMB_BOY, BOMB_ENEMY_CHANCE, 1, 4, MEGA_BOMB_BOY_WAVE)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::FLESH, {
        MobSpawnData(MOB_TYPE::CYCLOPS, 3, 4, 5, 8),
        MobSpawnData(MOB_TYPE::FLESH_CHICKEN, 0, 2, 5, 0),
        //MobSpawnData(MOB_TYPE::CHEESE_BOSS, CHEESE_BOSS_CHANCE, 1, 1, 50),
        MobSpawnData(MOB_TYPE::BOMB_BOY, BOMB_ENEMY_CHANCE, 4, 8, BOMB_BOY_WAVE),
        MobSpawnData(MOB_TYPE::MEGA_BOMB_BOY, BOMB_ENEMY_CHANCE, 1, 4, MEGA_BOMB_BOY_WAVE)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::GRASS_FOREST, {
        MobSpawnData(MOB_TYPE::SKELETON, 2, 3, 6, 0),
        MobSpawnData(MOB_TYPE::PLANT_MAN, 0, 5, 10, 0),
        //MobSpawnData(MOB_TYPE::CHEESE_BOSS, CHEESE_BOSS_CHANCE, 1, 1, 50),
        MobSpawnData(MOB_TYPE::LOG_MONSTER, 3, 1, 2, PROJ_ENEMIES_WAVE),
        MobSpawnData(MOB_TYPE::BOMB_BOY, BOMB_ENEMY_CHANCE, 4, 8, BOMB_BOY_WAVE),
        MobSpawnData(MOB_TYPE::MEGA_BOMB_BOY, BOMB_ENEMY_CHANCE, 1, 4, MEGA_BOMB_BOY_WAVE)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::RIVER, {})
};

#endif