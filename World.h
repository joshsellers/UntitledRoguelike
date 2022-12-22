#ifndef _WORLD_H
#define _WORLD H;

#include <vector>
#include "Chunk.h"
#include "Player.h"

constexpr int CHUNK_LOAD_THRESHOLD = 15;

class World {
public:
    World(Player* player);

    void update();

    void draw(sf::RenderTexture& surface);

private:
    std::vector<Chunk> _chunks;
    
    Player* _player;

    void loadChunk(sf::Vector2f pos);
    bool chunkContains(Chunk& chunk, sf::Vector2f pos);
};

#endif
