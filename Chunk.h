#ifndef _CHUNK_H
#define _CHUNK_H

#include <SFML/Graphics.hpp>
#include <memory>

constexpr int CHUNK_SIZE = 800;
// full size 550

enum class TERRAIN_TYPE : unsigned int {
    EMPTY         = 0,
    MOUNTAIN_LOW  = 1,
    MOUNTAIN_MID  = 2,
    MOUNTAIN_HIGH = 3,
    SAND          = 4,
    WATER         = 5,
    GRASS         = 6,
    TUNDRA        = 7,
    DESERT        = 8,
    SAVANNA       = 9,
    FLESH         = 10,
    GRASS_FOREST  = 11,
    RIVER         = 12
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
