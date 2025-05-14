#ifndef _WORLD_H
#define _WORLD_H

#include <vector>
#include "Chunk.h"
#include "entities/Player.h"
#include <mutex>
#include <memory>
#include "PerlinNoise.hpp"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "MobSpawnData.h"
#include "entities/ShopKeep.h"

constexpr int CHUNK_LOAD_THRESHOLD = 400;
// original size 270
// full size 400

constexpr long long INACTIVE_ENEMY_REMOVAL_INTERVAL = 5000LL;


constexpr int MAX_ACTIVE_MOBS = 80;
constexpr int INITIAL_MAX_ACTIVE_ENEMIES = 3;
constexpr int MIN_ENEMY_SPAWN_COOLDOWN_TIME_MILLISECONDS = 1000 * 60 * 1;
constexpr int MAX_ENEMY_SPAWN_COOLDOWN_TIME_MILLISECONDS = 1000 * 60 * 2;

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

    void addEntity(std::shared_ptr<Entity> entity, bool defer = true);
    std::vector<std::shared_ptr<Entity>> getEntities() const;
    std::vector<std::shared_ptr<Entity>> getEnemies() const;
    std::vector<std::shared_ptr<Entity>> getCollectorMobs() const;

    int getMobCount() const;
    int getEnemyCount() const;
    bool onEnemySpawnCooldown() const;
    long long getEnemySpawnCooldownTimeMilliseconds() const;
    long long getTimeUntilNextEnemyWave() const;
    void resetEnemySpawnCooldown();
    int getMaxActiveEnemies() const;
    void setMaxActiveEnemies(int maxActiveEnemies);
    void incrementEnemySpawnCooldownTimeWhilePaused();

    unsigned int getCurrentWaveNumber() const;

    std::shared_ptr<Player> getPlayer() const;

    bool showDebug() const;

    void propDestroyedAt(sf::Vector2f pos);

    void reseed(const unsigned int seed);

    bool drawChunkOutline = false;
    bool disableMobSpawning = false;
    bool disableEnemySpawning = false;
    bool disablePropGeneration = false;

    void resetChunks();

    void enterBuilding(std::string buildingID, sf::Vector2f buildingPos);
    void exitBuilding();
    bool playerIsInShop() const;

    void shopKeepKilled(unsigned int shopSeed);
    bool isShopKeepDead(unsigned int shopSeed) const;

    void setShopKeep(std::shared_ptr<ShopKeep> shopKeep);

    void startNewGameCooldown();

    void setDisplayedWaveNumber(int waveNumber);

    bool bossIsActive() const;
    void bossDefeated();
    std::shared_ptr<Entity> getCurrentBoss() const;

    void altarActivatedAt(sf::Vector2f pos);
    bool altarHasBeenActivatedAt(sf::Vector2f pos) const;

    friend class Game;
    friend class SaveManager;
    friend class UICommandPrompt;

private:
    std::shared_ptr<sf::Texture> _spriteSheet;

    std::mutex _mutex;

    std::vector<Chunk> _chunks;
    std::vector<sf::Vector2i> _loadingChunks;
    Chunk* _currentChunk = nullptr;
    
    std::shared_ptr<Player> _player;
    std::shared_ptr<ShopKeep> _shopKeep;

    std::vector<std::shared_ptr<Entity>> _entities;
    std::vector<std::shared_ptr<Entity>> _scatterBuffer;
    std::vector<std::shared_ptr<Entity>> _entityBuffer;

    std::vector<std::shared_ptr<Entity>> _enemies;
    std::vector<std::shared_ptr<Entity>> _collectorMobs;
    std::vector<std::shared_ptr<Entity>> _orbiters;

    std::vector<sf::Vector2f> _destroyedProps;
    bool isPropDestroyedAt(sf::Vector2f pos) const;

    std::vector<sf::Vector2f> _seenShops;
    bool shopHasBeenSeenAt(sf::Vector2f pos) const;
    void shopSeenAt(sf::Vector2f pos);
    std::vector<bool> _visitedShops;

    std::vector<unsigned int> _deadShopKeeps;

    std::vector<sf::Vector2f> _activatedAltars;

    void spawnMobs();
    void spawnEnemies();
    int getRandMobType(const BiomeMobSpawnData& mobSpawnData);
    sf::Clock _mobSpawnClock;
    boost::random::mt19937 _mobGen = boost::random::mt19937();
    sf::Clock _enemySpawnClock;
    boost::random::mt19937 _enemyGen = boost::random::mt19937();
    long long _cooldownStartTime = 0;
    bool _maxEnemiesReached = false;
    bool _cooldownActive = false;
    long long _enemySpawnCooldownTimeMilliseconds;
    int _maxActiveEnemies = INITIAL_MAX_ACTIVE_ENEMIES;
    int _enemiesSpawnedThisRound = 0;
    int _waveCounter = 0;
    int _currentWaveNumber = 1;
    int _highestPlayerHp = 100;

    bool _newGameCooldown = false;
    long long _newGameCooldownStartTime = 0;
    long long _newGameCooldownLength = 15000LL;

    void purgeEntityBuffer();
    void purgeScatterBuffer();
    void updateEntities();
    void eraseChunks(int pX, int pY);
    void findCurrentChunk(int pX, int pY);
    void loadNewChunks(int pX, int pY);
    void dumpChunkBuffer();

    void removeInactiveEntitiesFromSubgroups();
    long long _lastEntityRemovalTime = 0;

    void manageCurrentWave();
    void onWaveCleared();
    void checkAltarSpawn();

    void loadChunk(sf::Vector2f pos);
    void buildChunk(sf::Vector2f pos);
    bool chunkContains(const Chunk& chunk, sf::Vector2f pos) const;

    sf::Image generateChunkTerrain(Chunk& chunk);
    std::queue<Chunk> _chunkBuffer;
    void generateChunkScatters(Chunk& chunk);
    bool _loadingScatters = false;

    unsigned int _seed;

    boost::random::mt19937 gen = boost::random::mt19937();

    void sortEntities();

    bool& _showDebug;
    bool _showHitBoxes = false;
    void toggleShowHitBoxes();

    bool _isPlayerInShop = false;
    long long _shopEntranceTimeMillis = 0LL;

    bool _bossIsActive = false;
    std::shared_ptr<Entity> _currentBoss = nullptr;
    void spawnBoss(int currentWaveNumber);

    // this should be in a different class but oh well
    // also might not work right if done from a different class tho
    void givePlayerDefaultAbilities() const;
};

#endif
