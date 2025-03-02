#ifndef _MINI_MAP_GENERATOR_H
#define _MINI_MAP_GENERATOR_H

#include "Chunk.h"

class MiniMapGenerator {
public:
    const static int CHUNK_SIZE_SCALED = 8;
    const static int MAP_SIZE_DEFAULT_CHUNKS = 500;

    static void blitChunk(Chunk& chunk);
    static void markPoi(sf::Vector2f worldPos);

    static TERRAIN_TYPE getData(int index);
    static std::vector<sf::Vector2i>& getPoiLocations();

    static void reset();
private:
    static inline TERRAIN_TYPE _data[CHUNK_SIZE_SCALED * MAP_SIZE_DEFAULT_CHUNKS * CHUNK_SIZE_SCALED * MAP_SIZE_DEFAULT_CHUNKS];

    static inline std::vector<sf::Vector2i> _poiLocations;
    static inline std::vector<sf::Vector2i> _markerLocations;
};

#endif