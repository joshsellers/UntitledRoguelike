#ifndef _TERRAIN_GENERATOR_H
#define _TERRAIN_GENERATOR_H

#include "Chunk.h"

enum REALM {
    OVERWORLD,
    SEA,
    UNDERWORLD
};

class TerrainGenerator {
public:
    void setRealm(REALM realm);
    REALM getRealm() const;

    sf::Image generateChunkTerrain(Chunk& chunk, unsigned int seed);
private:
    REALM _realm = OVERWORLD;

    void generateOverworld(sf::Image& image, Chunk& chunk, unsigned int seed);
    void generateSea(sf::Image& image, Chunk& chunk, unsigned int seed);
};

#endif