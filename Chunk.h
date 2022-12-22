#ifndef _CHUNK_H
#define _CHUNK_H

#include <SFML/Graphics.hpp>

constexpr int CHUNK_SIZE = 100;

struct Chunk {
    Chunk(sf::Vector2f pos) {
        this->pos = pos;

        id = pos.x * pos.y;
    }

    int id;

    sf::Vector2f pos;
};

#endif 
