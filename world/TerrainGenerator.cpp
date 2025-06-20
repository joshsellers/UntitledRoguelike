#include "TerrainGenerator.h"
#include "../core/Util.h"
#include "../core/MessageManager.h"
#include "../core/Tutorial.h"
#include "../statistics/LocalAchievementManager.h"
#include "TerrainGenParameters.h"
#include "TerrainColor.h"
#include "FastNoise/FastNoise.h"

void TerrainGenerator::setRealm(REALM realm) {
    _realm = realm;
}

REALM TerrainGenerator::getRealm() const {
    return _realm;
}

sf::Image TerrainGenerator::generateChunkTerrain(Chunk& chunk, unsigned int seed) {
    long long startTime = 0;
    long long endTime = 0;
    if (BENCHMARK_TERRAIN_AND_BIOME_GEN) startTime = currentTimeMillis();

    sf::Image image;
    image.create(CHUNK_SIZE, CHUNK_SIZE);
    
    switch (_realm) {
        case OVERWORLD:
            generateOverworld(image, chunk, seed);
            break;
        case SEA:
            generateSea(image, chunk, seed);
            break;
    }

    if (BENCHMARK_TERRAIN_AND_BIOME_GEN) {
        endTime = currentTimeMillis();
        MessageManager::displayMessage("Terrain for chunk at (" + std::to_string(chunk.pos.x) + ", " + std::to_string(chunk.pos.y)
            + ") generated in " + std::to_string(endTime - startTime) + "ms", 2, DEBUG);
    }

    return image;
}

void TerrainGenerator::generateOverworld(sf::Image& image, Chunk& chunk, unsigned int seed) {
    const float TERRAIN_SCALE = TerrainGenInitializer::getParameters()->terrainScale;
    const float SCALE_COEFFICIENT = std::pow(10, TERRAIN_SCALE);

    sf::Vector2f pos = chunk.pos;

    // Generator properties
    const int octaves = TerrainGenInitializer::getParameters()->octaves;
    const double warpSize = TerrainGenInitializer::getParameters()->warpSize;
    const double warpStrength = TerrainGenInitializer::getParameters()->warpStrength;
    const double sampleRate = TerrainGenInitializer::getParameters()->sampleRate * SCALE_COEFFICIENT;

    // Terrain levels
    const double seaLevel = TerrainGenInitializer::getParameters()->seaLevel;
    const double oceanMidRange = TerrainGenInitializer::getParameters()->oceanMidRange;
    const double oceanShallowRange = TerrainGenInitializer::getParameters()->oceanShallowRange;
    const double sandRange = TerrainGenInitializer::getParameters()->sandRange;
    const double dirtLowRange = TerrainGenInitializer::getParameters()->dirtLowRange;
    const double dirtHighRange = TerrainGenInitializer::getParameters()->dirtHighRange;
    const double mountainLowRange = TerrainGenInitializer::getParameters()->mountainLowRange;
    const double mountainMidRange = TerrainGenInitializer::getParameters()->mountainMidRange;
    const double mountainHighRange = TerrainGenInitializer::getParameters()->mountainHighRange;

    int chX = pos.x;
    int chY = pos.y;

    std::vector<TERRAIN_TYPE> data(CHUNK_SIZE * CHUNK_SIZE);

    const auto perlin = FastNoise::New<FastNoise::Perlin>();
    const auto fractal = FastNoise::New<FastNoise::FractalFBm>();
    fractal->SetSource(perlin);
    fractal->SetOctaveCount(octaves);

    for (int y = chY; y < chY + CHUNK_SIZE; y++) {
        for (int x = chX; x < chX + CHUNK_SIZE; x++) {
            double warpNoise = fractal->GenSingle2D(warpSize * ((float)x * sampleRate * 2), warpSize * ((float)y * sampleRate * 2), seed);

            double warpNoise2 = fractal->GenSingle2D(warpSize * ((float)x * sampleRate * 4), warpSize * ((float)y * sampleRate * 4), seed);

            double val = fractal->GenSingle3D((x)*sampleRate, (y)*sampleRate, warpStrength * warpNoise * (warpStrength / 2) * warpNoise2, seed);

            sf::Uint32 rgb = 0x00;

            int dX = x - chX;
            int dY = y - chY;

            if (val < seaLevel) {
                rgb = (sf::Uint32)TERRAIN_COLOR::WATER_DEEP;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::WATER;
            } else if (val < oceanMidRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::WATER_MID;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::WATER;
            } else if (val < oceanShallowRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::WATER_SHALLOW;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::WATER;
            } else if (val < sandRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::SAND;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::SAND;
            } else if (val < dirtLowRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::DIRT_LOW;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::GRASS;
            } else if (val < dirtHighRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::DIRT_HIGH;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::GRASS;
            } else if (val < mountainLowRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::MOUNTAIN_LOW;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::MOUNTAIN_LOW;
            } else if (val < mountainMidRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::MOUNTAIN_MID;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::MOUNTAIN_MID;
            } else {
                rgb = (sf::Uint32)TERRAIN_COLOR::MOUNTAIN_HIGH;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::MOUNTAIN_HIGH;
            }

            // biomes
            const double xOffset = TerrainGenInitializer::getParameters()->biomeXOffset / SCALE_COEFFICIENT;
            const double yOffset = TerrainGenInitializer::getParameters()->biomeYOffset / SCALE_COEFFICIENT;
            const int biomeOctaves = TerrainGenInitializer::getParameters()->biomeOctaves;
            const double biomeSampleRate = TerrainGenInitializer::getParameters()->biomeSampleRate * SCALE_COEFFICIENT;;

            double temperatureNoise = perlin->GenSingle2D((x + xOffset) * biomeSampleRate, (y + yOffset) * biomeSampleRate, seed + 10);
            double precipitationNoise = perlin->GenSingle2D((x + xOffset) * biomeSampleRate, (y + yOffset) * biomeSampleRate, seed + 40);
            temperatureNoise = norm_0_1(temperatureNoise, -1, 1);
            precipitationNoise = norm_0_1(precipitationNoise, -1, 1);

            const float biomeEdgeMixing = TerrainGenInitializer::getParameters()->biomeEdgeMixing;
            temperatureNoise += ((float)randomInt(-(int)biomeEdgeMixing, (int)biomeEdgeMixing)) / 100000.;
            precipitationNoise += ((float)randomInt(-(int)biomeEdgeMixing, (int)biomeEdgeMixing)) / 100000.;

            const sf::Vector2f tundraTemp = TerrainGenInitializer::getParameters()->tundraTemp;
            const sf::Vector2f tundraPrec = TerrainGenInitializer::getParameters()->tundraPrec;

            const sf::Vector2f desertTemp = TerrainGenInitializer::getParameters()->desertTemp;
            const sf::Vector2f desertPrec = TerrainGenInitializer::getParameters()->desertPrec;

            const sf::Vector2f savannaTemp = TerrainGenInitializer::getParameters()->savannaTemp;
            const sf::Vector2f savannaPrec = TerrainGenInitializer::getParameters()->savannaPrec;

            const sf::Vector2f forestTemp = TerrainGenInitializer::getParameters()->forestTemp;
            const sf::Vector2f forestPrec = TerrainGenInitializer::getParameters()->forestPrec;

            bool tundra = temperatureNoise > tundraTemp.x && temperatureNoise < tundraTemp.y&& precipitationNoise > tundraPrec.x && precipitationNoise < tundraPrec.y;
            bool desert = temperatureNoise > desertTemp.x && temperatureNoise < desertTemp.y&& precipitationNoise > desertPrec.x && precipitationNoise < desertPrec.y;
            bool savanna = temperatureNoise > savannaTemp.x && temperatureNoise < savannaTemp.y&& precipitationNoise > savannaPrec.x && precipitationNoise < savannaPrec.y;
            bool forest = temperatureNoise > forestTemp.x && temperatureNoise < forestTemp.y&& precipitationNoise > forestPrec.x && precipitationNoise < forestPrec.y;

            if (!Tutorial::isCompleted()) {
                desert = false;
                tundra = false;
                forest = false;
            }

            // rare biomes 
            const double rareBiomeSampleRate = biomeSampleRate / 2;

            double rareBiomeTemp = perlin->GenSingle2D((x + xOffset) * rareBiomeSampleRate, (y + yOffset) * rareBiomeSampleRate, seed + 8);
            double rareBiomePrec = perlin->GenSingle2D((x + xOffset) * rareBiomeSampleRate, (y + yOffset) * rareBiomeSampleRate, seed + 34);
            rareBiomeTemp = norm_0_1(rareBiomeTemp, -1, 1);
            rareBiomePrec = norm_0_1(rareBiomePrec, -1, 1);

            rareBiomeTemp += ((float)randomInt(-(int)biomeEdgeMixing, (int)biomeEdgeMixing)) / 100000.;
            rareBiomePrec += ((float)randomInt(-(int)biomeEdgeMixing, (int)biomeEdgeMixing)) / 100000.;

            const sf::Vector2f fleshTemp = TerrainGenInitializer::getParameters()->fleshTemp;
            const sf::Vector2f fleshPrec = TerrainGenInitializer::getParameters()->fleshPrec;

            const sf::Vector2f fungusTemp = TerrainGenInitializer::getParameters()->fungusTemp;
            const sf::Vector2f fungusPrec = TerrainGenInitializer::getParameters()->fungusPrec;

            bool flesh = rareBiomeTemp > fleshTemp.x && rareBiomeTemp < fleshTemp.y&& rareBiomePrec > fleshPrec.x && rareBiomePrec < fleshPrec.y;
            bool fungus = rareBiomeTemp > fungusTemp.x && rareBiomeTemp < fungusTemp.y&& rareBiomePrec > fungusPrec.x && rareBiomePrec < fungusPrec.y
                && LocalAchievementManager::isUnlocked(ACHIEVEMENT::DEFEAT_SHROOMBOSS);

            if (seed == 124959026) flesh = true;
            else if (seed == 4134632056) fungus = true;
            if (flesh && seed != 124959026) {
                AchievementManager::unlock(FLESHY);
            } else if (fungus && seed != 4134632056) {
                AchievementManager::unlock(SHROOMY);
            }

            TERRAIN_TYPE terrainType = data[dX + dY * CHUNK_SIZE];

            if (terrainType == TERRAIN_TYPE::GRASS) {
                if (tundra) {
                    data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::TUNDRA;
                    rgb = (sf::Uint32)TERRAIN_COLOR::TUNDRA;
                } else if (desert) {
                    data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::DESERT;
                    rgb = (sf::Uint32)TERRAIN_COLOR::DESERT;
                } else if (savanna) {
                    data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::SAVANNA;
                    rgb = (sf::Uint32)TERRAIN_COLOR::SAVANNA;
                } else if (forest) {
                    data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::GRASS_FOREST;
                    rgb = (sf::Uint32)TERRAIN_COLOR::FOREST;
                }

                if (flesh) {
                    data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::FLESH;
                    rgb = (sf::Uint32)TERRAIN_COLOR::FLESH;
                } else if (fungus) {
                    data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::FUNGUS;
                    rgb = (sf::Uint32)TERRAIN_COLOR::FUNGUS;
                }
            }
            terrainType = data[dX + dY * CHUNK_SIZE];

            sf::Uint32 r = (rgb >> 16) & 0xFF;
            sf::Uint32 g = (rgb >> 8) & 0xFF;
            sf::Uint32 b = rgb & 0xFF;

            if (terrainType != TERRAIN_TYPE::SAND && terrainType != TERRAIN_TYPE::DESERT) {
                r += randomInt(0, 10);
                g += randomInt(0, 10);
                if (terrainType != TERRAIN_TYPE::TUNDRA
                    && terrainType != TERRAIN_TYPE::MOUNTAIN_LOW
                    && terrainType != TERRAIN_TYPE::MOUNTAIN_MID
                    && terrainType != TERRAIN_TYPE::MOUNTAIN_HIGH) b += randomInt(0, 10);

                if ((terrainType == TERRAIN_TYPE::GRASS && rgb == (sf::Uint32)TERRAIN_COLOR::DIRT_LOW)
                    || terrainType == TERRAIN_TYPE::SAVANNA || terrainType == TERRAIN_TYPE::FLESH) {
                    int ar = (int)(0x55 * (val)) | r;
                    int ag = (int)(0x55 * (val)) | g;
                    int ab = (int)(0x55 * (val)) | b;
                    r = ar;
                    g = ag;
                    b = ab;
                } else if (terrainType == TERRAIN_TYPE::GRASS && rgb == (sf::Uint32)TERRAIN_COLOR::DIRT_HIGH) {
                    r *= (val + 0.2) * 2;
                    g *= (val + 0.2) * 2;
                    b *= (val + 0.2) * 2;
                }

                rgb = r;
                rgb = (rgb << 8) + (g);
                rgb = (rgb << 8) + (b);
            } else {
                rgb = r;
                rgb = (rgb << 8) + (g + randomInt(0, 10));
                rgb = (rgb << 8) + (b + randomInt(0, 10));
            }

            image.setPixel(x - chX, y - chY, sf::Color(((rgb) << 8) + 0xFF));
        }
    }

    chunk.terrainData = data;

    //if (!disablePropGeneration) generateChunkScatters(chunk);
}

void TerrainGenerator::generateSea(sf::Image& image, Chunk& chunk, unsigned int seed) {
    int chX = chunk.pos.x;
    int chY = chunk.pos.y;

    std::vector<TERRAIN_TYPE> data(CHUNK_SIZE * CHUNK_SIZE);

    for (int y = chY; y < chY + CHUNK_SIZE; y++) {
        for (int x = chX; x < chX + CHUNK_SIZE; x++) {
            int dX = x - chX;
            int dY = y - chY;

            data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::SAND;
            image.setPixel(dX, dY, sf::Color(((sf::Uint32)TERRAIN_COLOR::DESERT << 8) + 0xFF));
        }
    }

    chunk.terrainData = data;
}
