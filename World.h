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

constexpr int CHUNK_LOAD_THRESHOLD = 270;
// full size 270

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
    DESERT = 0xFDE898
};

enum class MOB_TYPE : int {
    PENGUIN = 0,
    TURTLE = 1,
    PLANT_MAN = 2
};

constexpr int TUNDRA_MOB_COUNT = 1;
constexpr MOB_TYPE TUNDRA_MOBS[TUNDRA_MOB_COUNT] = { MOB_TYPE::PENGUIN };

constexpr int GRASS_MOB_COUNT = 7;
constexpr MOB_TYPE GRASS_MOBS[GRASS_MOB_COUNT] = { 
    MOB_TYPE::TURTLE, MOB_TYPE::TURTLE, MOB_TYPE::TURTLE, 
    MOB_TYPE::TURTLE, MOB_TYPE::TURTLE, MOB_TYPE::TURTLE, 
    MOB_TYPE::PLANT_MAN 
};

constexpr int SAVANNA_MOB_COUNT = 1;
constexpr MOB_TYPE SAVANNA_MOBS[SAVANNA_MOB_COUNT] = { MOB_TYPE::PLANT_MAN };


constexpr int MAX_ACTIVE_MOBS = 15;

class World {
public:
    World(std::shared_ptr<Player> player, bool& showDebug);

    void update();

    void draw(sf::RenderTexture& surface);

    int getActiveChunkCount();

    TERRAIN_TYPE getTerrainDataAt(sf::Vector2f pos);
    TERRAIN_TYPE getTerrainDataAt(float x, float y);

    Chunk* getCurrentChunk();
    TERRAIN_TYPE getTerrainDataAt(Chunk* chunk, sf::Vector2f pos);

    void loadSpriteSheet(std::shared_ptr<sf::Texture> spriteSheet);
    std::shared_ptr<sf::Texture> getSpriteSheet() const;

    int getSeed();

    void addEntity(std::shared_ptr<Entity> entity);
    std::vector<std::shared_ptr<Entity>> getEntities() const;

    int getMobCount() const;

    std::shared_ptr<Player> getPlayer() const;

    bool showDebug() const;

    void propDestroyedAt(sf::Vector2f pos);

    void reseed(const unsigned int seed);

private:
    std::shared_ptr<sf::Texture> _spriteSheet;

    std::mutex _mutex;

    std::vector<Chunk> _chunks;
    std::vector<sf::Vector2i> _loadingChunks;
    Chunk* _currentChunk = nullptr;
    
    std::shared_ptr<Player> _player;

    std::vector<std::shared_ptr<Entity>> _entities;
    std::vector<std::shared_ptr<Entity>> _entityBuffer;

    std::vector<sf::Vector2f> _destroyedProps;
    bool isPropDestroyedAt(sf::Vector2f pos) const;

    void spawnMobs();
    int getRandMobType(int mobListSize);
    sf::Clock _mobSpawnClock;
    boost::random::mt19937 _mobGen = boost::random::mt19937();

    void purgeEntityBuffer();
    void updateEntities();
    void eraseChunks(int pX, int pY);
    void findCurrentChunk(int pX, int pY);
    void loadNewChunks(int pX, int pY);

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
};

#endif
