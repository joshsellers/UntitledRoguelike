#include "MiniMapGenerator.h"
#include "../core/MessageManager.h"

void MiniMapGenerator::blitChunk(Chunk& chunk) {
    const int centerX = ((float)CHUNK_SIZE_SCALED * (float)MAP_SIZE_DEFAULT_CHUNKS) / 2.f;
    const int centerY = centerX;

    const float scale = ((float)CHUNK_SIZE_SCALED / (float)CHUNK_SIZE);
    const int chunkXScaled = chunk.pos.x * scale + centerX;
    const int chunkYScaled = chunk.pos.y * scale + centerY;

    for (int y = 0; y < CHUNK_SIZE_SCALED; y++) {
        for (int x = 0; x < CHUNK_SIZE_SCALED; x++) {
            const int sectionSize = (CHUNK_SIZE / CHUNK_SIZE_SCALED);
            constexpr int biomeCount = 14;
            int terrainTypeCounts[biomeCount]{};
            for (int ya = sectionSize * y; ya < sectionSize * y + sectionSize; ya++) {
                for (int xa = sectionSize * x; xa < sectionSize * x + sectionSize; xa++) {
                    const TERRAIN_TYPE biome = chunk.terrainData[xa + ya * CHUNK_SIZE];
                    terrainTypeCounts[(int)biome]++;
                }
            }

            int mostCommonBiome = 0;
            int highestCount = 0;
            for (int i = 0; i < biomeCount; i++) {
                if (terrainTypeCounts[i] > terrainTypeCounts[mostCommonBiome]) {
                    mostCommonBiome = i;
                }
            }

            const unsigned int index = (chunkXScaled + x) + (chunkYScaled + y) * (CHUNK_SIZE_SCALED * MAP_SIZE_DEFAULT_CHUNKS);
            if (index >= 0 && index < CHUNK_SIZE_SCALED * MAP_SIZE_DEFAULT_CHUNKS * CHUNK_SIZE_SCALED * MAP_SIZE_DEFAULT_CHUNKS) {
                _data[index] = (TERRAIN_TYPE)mostCommonBiome;
            }
        }
    }
}

void MiniMapGenerator::markPoi(sf::Vector2f worldPos) {
    const int centerX = ((float)CHUNK_SIZE_SCALED * (float)MAP_SIZE_DEFAULT_CHUNKS) / 2.f;
    const int centerY = centerX;

    const float scale = ((float)CHUNK_SIZE_SCALED / (float)CHUNK_SIZE);

    const sf::Vector2i mapLocation(worldPos.x * scale + centerX, worldPos.y * scale + centerY);
    _poiLocations.push_back(mapLocation);
}

void MiniMapGenerator::dropPin(sf::Vector2f worldPos) {
    const int centerX = ((float)CHUNK_SIZE_SCALED * (float)MAP_SIZE_DEFAULT_CHUNKS) / 2.f;
    const int centerY = centerX;

    const float scale = ((float)CHUNK_SIZE_SCALED / (float)CHUNK_SIZE);

    const sf::Vector2i mapLocation(worldPos.x * scale + centerX, worldPos.y * scale + centerY);
    bool alreadyMarked = false;
    bool* alreadyMarkedP = &alreadyMarked;
    _markerLocations.erase(std::remove_if(_markerLocations.begin(), _markerLocations.end(),
        [mapLocation, alreadyMarkedP](sf::Vector2i location) {
            if (location == mapLocation) {
                *alreadyMarkedP = true;
                return true;
            }
            return false; 
        }), _markerLocations.end());

    if (alreadyMarked) return;

    _markerLocations.push_back(mapLocation);
}

TERRAIN_TYPE MiniMapGenerator::getData(int index) {
    if (index < CHUNK_SIZE_SCALED * MAP_SIZE_DEFAULT_CHUNKS * CHUNK_SIZE_SCALED * MAP_SIZE_DEFAULT_CHUNKS && index >= 0) {
        return _data[index];
    } else return TERRAIN_TYPE::EMPTY;
}

std::vector<sf::Vector2i>& MiniMapGenerator::getPoiLocations() {
    return _poiLocations;
}

std::vector<sf::Vector2i>& MiniMapGenerator::getPinLocations() {
    return _markerLocations;
}

void MiniMapGenerator::reset() {
    _markerLocations.clear();
    _poiLocations.clear();

    for (int i = 0; i < CHUNK_SIZE_SCALED * MAP_SIZE_DEFAULT_CHUNKS * CHUNK_SIZE_SCALED * MAP_SIZE_DEFAULT_CHUNKS; i++) {
        _data[i] = TERRAIN_TYPE::EMPTY;
    }
}
