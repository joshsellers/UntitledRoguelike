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
    WATER_DEEP = 0X3370CC,
    WATER_MID = 0X4084E2,
    WATER_SHALLOW = 0X55AEF0,

    SAND = 0XF7E898,
    DIRT_LOW = 0X77C73A,
    DIRT_HIGH = 0X417D13,

    MOUNTAIN_LOW = 0X5B5E5C,
    MOUNTAIN_MID = 0X414545,
    MOUNTAIN_HIGH = 0XE2EDEC,

    TUNDRA = MOUNTAIN_HIGH,
    SAVANNA = 0X95A54F, //0XB5954F
    DESERT = 0XFDE898
};

class World {
public:
    World(std::shared_ptr<Player> player);

    void update();

    void draw(sf::RenderTexture& surface);

    void purgeEntityBuffer();
    void updateEntities();
    void eraseChunks(int pX, int pY);
    void findCurrentChunk(int pX, int pY);
    void loadNewChunks(int pX, int pY);

    int getActiveChunkCount();

    Chunk* getCurrentChunk();
    TERRAIN_TYPE getTerrainDataAt(Chunk* chunk, sf::Vector2f pos);

    void loadSpriteSheet(std::shared_ptr<sf::Texture> spriteSheet);
    std::shared_ptr<sf::Texture> getSpriteSheet() const;

    int getSeed();

    void addEntity(std::shared_ptr<Entity> entity);
    std::vector<std::shared_ptr<Entity>> getEntities() const;

    std::shared_ptr<Player> getPlayer() const;

private:
    std::shared_ptr<sf::Texture> _spriteSheet;

    std::mutex _mutex;

    std::vector<Chunk> _chunks;
    std::vector<sf::Vector2i> _loadingChunks;
    
    std::shared_ptr<Player> _player;

    std::vector<std::shared_ptr<Entity>> _entities;
    std::vector<std::shared_ptr<Entity>> _entityBuffer;

    Chunk* _currentChunk = nullptr;

    void loadChunk(sf::Vector2f pos);
    void buildChunk(sf::Vector2f pos);
    bool chunkContains(Chunk& chunk, sf::Vector2f pos);

    sf::Image generateChunkTerrain(Chunk& chunk);
    void generateChunkProps(Chunk& chunk);

    sf::Font _font; // TEMP

    int _seed;

    boost::random::mt19937 gen = boost::random::mt19937();

    void sortEntities();
};

#endif
