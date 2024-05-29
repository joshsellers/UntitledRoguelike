#ifndef _WORLD_H
#define _WORLD_H

#include <vector>
#include "Chunk.h"
#include "Player.h"
#include <mutex>
#include <memory>
#include "PerlinNoise.hpp"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "MobSpawnData.h"
#include "ShopKeep.h"

constexpr int CHUNK_LOAD_THRESHOLD = 400;
// original size 270
// full size 400

constexpr long long INACTIVE_ENEMY_REMOVAL_INTERVAL = 5000LL;

enum class TERRAIN_COLOR : sf::Uint32 {
    WATER_DEEP = 0x3370CC,
    WATER_MID = 0x4084E2,
    WATER_SHALLOW = 0x55AEF0,

    SAND = 0xF7E898,
    DIRT_LOW = 0x77C73A,
    DIRT_HIGH = 0x417D13,

    MOUNTAIN_LOW = 0x5B5E5C,
    MOUNTAIN_MID = 0x414545,
    MOUNTAIN_HIGH = 0xE2EDEC,

    TUNDRA = MOUNTAIN_HIGH,
    SAVANNA = 0x95A54F, //0xB5954F
    DESERT = 0xFDE898,

    FLESH = 0xEEC39A,

    FOREST = 0x77C73A
};

constexpr int MAX_ACTIVE_MOBS = 80;
constexpr int INITIAL_MAX_ACTIVE_ENEMIES = 5;
constexpr int MIN_ENEMY_SPAWN_COOLDOWN_TIME_MILLISECONDS = 1000 * 60 * 1;
constexpr int MAX_ENEMY_SPAWN_COOLDOWN_TIME_MILLISECONDS = 1000 * 60 * 2;

const BiomeMobSpawnData MOB_SPAWN_DATA[8] = {
    BiomeMobSpawnData(TERRAIN_TYPE::WATER, {}),

    BiomeMobSpawnData(TERRAIN_TYPE::GRASS, {
        MobSpawnData(MOB_TYPE::TURTLE, 1, 1, 1),
        MobSpawnData(MOB_TYPE::PLANT_MAN, 0, 2, 8),
        MobSpawnData(MOB_TYPE::FROG, 5, 1, 3)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::TUNDRA, {
        MobSpawnData(MOB_TYPE::PENGUIN, 0, 4, 15),
        MobSpawnData(MOB_TYPE::SNOW_MAN, 0, 2, 6),
        MobSpawnData(MOB_TYPE::YETI, 1, 4, 8)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::DESERT, {
        MobSpawnData(MOB_TYPE::CACTOID, 1, 1, 1)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::SAVANNA, {
        MobSpawnData(MOB_TYPE::PLANT_MAN, 0, 4, 10)
    }),

    BiomeMobSpawnData(TERRAIN_TYPE::FLESH, {}),

    BiomeMobSpawnData(TERRAIN_TYPE::GRASS_FOREST, {}),

    BiomeMobSpawnData(TERRAIN_TYPE::RIVER, {})
};

class World {
public:
    World(std::shared_ptr<Player> player, bool& showDebug);

    void init(unsigned int seed);

    void loadChunksAroundPlayer();

    void update();

    void draw(sf::RenderTexture& surface);

    int getActiveChunkCount();

    TERRAIN_TYPE getTerrainDataAt(sf::Vector2f pos);
    TERRAIN_TYPE getTerrainDataAt(float x, float y);

    Chunk* getCurrentChunk();
    TERRAIN_TYPE getTerrainDataAt(Chunk* chunk, sf::Vector2f pos);

    void loadSpriteSheet(std::shared_ptr<sf::Texture> spriteSheet);
    std::shared_ptr<sf::Texture> getSpriteSheet() const;

    unsigned int getSeed();

    void addEntity(std::shared_ptr<Entity> entity);
    std::vector<std::shared_ptr<Entity>> getEntities() const;
    std::vector<std::shared_ptr<Entity>> getEnemies() const;

    int getMobCount() const;
    int getEnemyCount() const;
    bool onEnemySpawnCooldown() const;
    long long getEnemySpawnCooldownTimeMilliseconds() const;
    long long getTimeUntilNextEnemyWave() const;
    void resetEnemySpawnCooldown();
    int getMaxActiveEnemies() const;
    void setMaxActiveEnemies(int maxActiveEnemies);
    void incrementEnemySpawnCooldownTimeWhilePaused();

    std::shared_ptr<Player> getPlayer() const;

    bool showDebug() const;

    void propDestroyedAt(sf::Vector2f pos);

    void reseed(const unsigned int seed);

    bool drawChunkOutline = false;
    bool disableMobSpawning = false;
    bool disablePropGeneration = false;

    void resetChunks();

    void enterShop(sf::Vector2f shopPos);
    void exitShop();
    bool playerIsInShop() const;

    void setShopKeep(std::shared_ptr<ShopKeep> shopKeep);

    friend class Game;
    friend class SaveManager;

private:
    std::shared_ptr<sf::Texture> _spriteSheet;

    std::mutex _mutex;

    std::vector<Chunk> _chunks;
    std::vector<sf::Vector2i> _loadingChunks;
    Chunk* _currentChunk = nullptr;
    
    std::shared_ptr<Player> _player;
    std::shared_ptr<ShopKeep> _shopKeep;

    std::vector<std::shared_ptr<Entity>> _entities;
    std::vector<std::shared_ptr<Entity>> _entityBuffer;

    std::vector<std::shared_ptr<Entity>> _enemies;

    std::vector<sf::Vector2f> _destroyedProps;
    bool isPropDestroyedAt(sf::Vector2f pos) const;

    std::vector<sf::Vector2f> _seenShops;
    bool shopHasBeenSeenAt(sf::Vector2f pos) const;
    void shopSeenAt(sf::Vector2f pos);

    void spawnMobs();
    int getRandMobType(const BiomeMobSpawnData& mobSpawnData);
    sf::Clock _mobSpawnClock;
    boost::random::mt19937 _mobGen = boost::random::mt19937();
    long long _cooldownStartTime = 0;
    bool _maxEnemiesReached = false;
    bool _cooldownActive = false;
    long long _enemySpawnCooldownTimeMilliseconds;
    int _maxActiveEnemies = INITIAL_MAX_ACTIVE_ENEMIES;
    int _enemiesSpawnedThisRound = 0;
    int _waveCounter = 0;
    int _currentWaveNumber = 1;

    void purgeEntityBuffer();
    void updateEntities();
    void eraseChunks(int pX, int pY);
    void findCurrentChunk(int pX, int pY);
    void loadNewChunks(int pX, int pY);

    void removeInactiveEnemies();
    long long _lastEnemyRemovalTime = 0;

    void loadChunk(sf::Vector2f pos);
    void buildChunk(sf::Vector2f pos);
    bool chunkContains(Chunk& chunk, sf::Vector2f pos);

    sf::Image generateChunkTerrain(Chunk& chunk);
    void generateChunkEntities(Chunk& chunk);

    sf::Font _font; // TEMP

    unsigned int _seed;

    boost::random::mt19937 gen = boost::random::mt19937();

    void sortEntities();

    bool& _showDebug;

    bool _isPlayerInShop = false;
};

#endif
