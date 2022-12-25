#ifndef _WORLD_H
#define _WORLD_H

#include <vector>
#include "Chunk.h"
#include "Player.h"
#include <mutex>

constexpr int CHUNK_LOAD_THRESHOLD = 270;
// full size 270

enum class TERRAIN_COLOR : sf::Uint32 {
    WATER_DEEP = 0x3370cc,
    WATER_MID = 0x4084e2,
    WATER_SHALLOW = 0x55aef0,

    SAND = 0xf7e898,
    DIRT_LOW = 0x77c73a,
    DIRT_HIGH = 0x417d13,

    MOUNTAIN_LOW = 0x5b5e5c,
    MOUNTAIN_MID = 0x414545,
    MOUNTAIN_HIGH = 0xe2edec
};

class World {
public:
    World(Player* player);

    void update();

    void draw(sf::RenderTexture& surface);

    int getActiveChunkCount();

    Chunk* getCurrentChunk();
    TERRAIN_TYPE getTerrainDataAt(Chunk* chunk, sf::Vector2f pos);

private:
    std::mutex _mutex;

    std::vector<Chunk> _chunks;
    std::vector<sf::Vector2i> _loadingChunks;
    
    Player* _player;

    Chunk* _currentChunk = nullptr;

    void loadChunk(sf::Vector2f pos);
    void buildChunk(sf::Vector2f pos);
    bool chunkContains(Chunk& chunk, sf::Vector2f pos);

    sf::Image generateChunkTerrain(Chunk& chunk);

    sf::Font _font; // TEMP

    int _seed;
};

#endif
