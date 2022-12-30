#ifndef _CHUNK_H
#define _CHUNK_H

#include <SFML/Graphics.hpp>
#include <memory>

constexpr int CHUNK_SIZE = 550;
// full size 550

enum class TERRAIN_TYPE : unsigned int {
    VOID          = 0,
    WATER         = 1,
    GRASS_LOW     = 2,
    GRASS_HIGH    = 3,
    MOUNTAIN_LOW  = 4,
    MOUNTAIN_MID  = 5,
    MOUNTAIN_HIGH = 6,
    SAND          = 7,
    TUNDRA        = 8,
    DESERT        = 9,
    SAVANNA       = 10
};

struct Chunk {
    Chunk(sf::Vector2f pos) {
        this->pos = pos;
        id = pos.x * pos.y;
    }

    int id;
    sf::Vector2f pos;

    std::shared_ptr<sf::Texture> texture = std::shared_ptr<sf::Texture>(new sf::Texture());
    sf::Sprite sprite;

    std::vector<TERRAIN_TYPE> terrainData;
};

#endif 
