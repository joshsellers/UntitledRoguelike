#ifndef _CHUNK_H
#define _CHUNK_H

#include <SFML/Graphics.hpp>
#include <memory>

constexpr int CHUNK_SIZE = 550;
// full size 550

enum class TERRAIN_TYPE : unsigned int {
    NOT_WATER = 0,
    WATER     = 1
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
