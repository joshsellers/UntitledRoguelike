#include "World.h"
#include <iostream>
#include "PerlinNoise.hpp"
#include <thread>
#include "Util.h"

World::World(std::shared_ptr<Player> player) {
    _player = player;
    _player->setWorld(this);

    _entities.push_back(_player);

    _seed = randomInt(0, 999);

    int pX = _player->getPosition().x + PLAYER_WIDTH / 2;
    int pY = _player->getPosition().y + PLAYER_HEIGHT;
    loadChunk(sf::Vector2f(pX - CHUNK_SIZE / 2, pY - CHUNK_SIZE / 2));

    if (!_font.loadFromFile("font.ttf")) {
        std::cout << "Failed to load font!" << std::endl;
    }
}

void World::update() {
    for (auto& entity : _entities) {
        entity->update();
    }

    int pX = ((int)_player->getPosition().x + PLAYER_WIDTH / 2);
    int pY = ((int)_player->getPosition().y + PLAYER_HEIGHT);

    for (int i = 0; i < _chunks.size(); i++) {
        Chunk& chunk = _chunks.at(i);

        if (chunkContains(chunk, sf::Vector2f(pX, pY))) continue;

        int chX = chunk.pos.x;
        int chY = chunk.pos.y;

        bool left = std::abs(pX - chX) < CHUNK_LOAD_THRESHOLD;
        bool top = std::abs(pY - chY) < CHUNK_LOAD_THRESHOLD;
        bool right = std::abs(pX - (chX + CHUNK_SIZE)) < CHUNK_LOAD_THRESHOLD;
        bool bottom = std::abs(pY - (chY + CHUNK_SIZE)) < CHUNK_LOAD_THRESHOLD;

        bool inVerticleBounds = pX > chX && pX < chX + CHUNK_SIZE;
        bool inHorizontalBounds = pY > chY && pY < chY + CHUNK_SIZE;

        if ((!left && !right && !top && !bottom)
            || ((top || bottom) && (!inVerticleBounds && !left && !right))
            || ((left || right) && (!inHorizontalBounds && !top && !bottom))) {
            //std::cout << "erasing chunk " << chunk.id << std::endl;
            _chunks.erase(_chunks.begin() + i);
        }
    }

    for (Chunk& chunk : _chunks) {
        if (chunkContains(chunk, sf::Vector2f(pX, pY))) {
            _currentChunk = &chunk;
            break;
        }
    }

    if (_currentChunk != nullptr) {
        int chX = _currentChunk->pos.x;
        int chY = _currentChunk->pos.y;

        bool left = std::abs(pX - chX) < CHUNK_LOAD_THRESHOLD;
        bool top = std::abs(pY - chY) < CHUNK_LOAD_THRESHOLD;
        bool right = std::abs(pX - (chX + (int)CHUNK_SIZE)) < CHUNK_LOAD_THRESHOLD;
        bool bottom = std::abs(pY - (chY + (int)CHUNK_SIZE)) < CHUNK_LOAD_THRESHOLD;


        if (left) {
            loadChunk(sf::Vector2f(chX - CHUNK_SIZE, chY));
        } else if (right) {
            loadChunk(sf::Vector2f(chX + CHUNK_SIZE, chY));
        }

        if (top) {
            loadChunk(sf::Vector2f(chX, chY - CHUNK_SIZE));
        } else if (bottom) {
            loadChunk(sf::Vector2f(chX, chY + CHUNK_SIZE));
        }

        if (top && left) {
            loadChunk(sf::Vector2f(chX - CHUNK_SIZE, chY - CHUNK_SIZE));
        } else if (bottom && right) {
            loadChunk(sf::Vector2f(chX + CHUNK_SIZE, chY + CHUNK_SIZE));
        } else if (bottom && left) {
            loadChunk(sf::Vector2f(chX - CHUNK_SIZE, chY + CHUNK_SIZE));
        } else if (top && right) {
            loadChunk(sf::Vector2f(chX + CHUNK_SIZE, chY - CHUNK_SIZE));
        }
    } else if (_currentChunk == nullptr) {
        std::cout << "currentChunk was nullptr" << std::endl;
    }
}

void World::draw(sf::RenderTexture& surface) {
    for (Chunk& chunk : _chunks) {
        try {
            surface.draw(chunk.sprite);
            //sf::RectangleShape chunkOutline(sf::Vector2f(CHUNK_SIZE - 1, CHUNK_SIZE - 1));

            //chunkOutline.setFillColor(sf::Color::Transparent);
            //chunkOutline.setOutlineColor(sf::Color(0xFFFFFFFF));
            //chunkOutline.setOutlineThickness(1);
            //chunkOutline.setPosition(chunk.pos);
            //surface.draw(chunkOutline);

            //sf::Text idLabel;
            //.setFont(_font);
            //idLabel.setCharacterSize(10);
            //idLabel.setString(std::to_string(chunk.id));
            //idLabel.setPosition(chunk.pos.x, chunk.pos.y - 4);
            //surface.draw(idLabel);
        } catch (std::exception& e) {
            std::cout << "exception: " << e.what() << std::endl;
        }
    }

    for (auto& entity : _entities) {
        entity->draw(surface);
    }
}

void World::loadChunk(sf::Vector2f pos) {
    for (sf::Vector2i loadingChunk : _loadingChunks) {
        if (loadingChunk.x == (int)pos.x && loadingChunk.y == (int)pos.y) {
            //std::cout << "chunk at " << pos.x << ", " << pos.y << " is already loading, did not generate" << std::endl;
            return;
        }
    }
    for (Chunk& chunk : _chunks) {
        if (chunk.pos == pos) {
            //std::cout << "chunk " << chunk.id << " already exists, did not load" << std::endl;
            return;
        }
    }
    _loadingChunks.push_back(sf::Vector2i((int)pos.x, (int)pos.y));
    std::thread buildThread(&World::buildChunk, this, pos);
    buildThread.detach();
}

void World::buildChunk(sf::Vector2f pos) {
    _mutex.lock();

    Chunk chunk(pos);
    //std::cout << "loading chunk " << chunk.id << std::endl;
    chunk.texture->create(CHUNK_SIZE, CHUNK_SIZE);
    chunk.texture->update(generateChunkTerrain(chunk));
    chunk.sprite.setTexture(*chunk.texture);
    chunk.sprite.setPosition(chunk.pos);
    _chunks.push_back(chunk);

    _mutex.unlock();

    for (int i = 0; i < _loadingChunks.size(); i++) {
        sf::Vector2i loadingChunk = _loadingChunks.at(i);
        if (loadingChunk.x == (int)pos.x && loadingChunk.y == (int)pos.y) {
            _loadingChunks.erase(_loadingChunks.begin() + i);
            break;
        }
    }
}

bool World::chunkContains(Chunk& chunk, sf::Vector2f pos) {
    int pX = (int)pos.x;
    int pY = (int)pos.y;
    int chX = (int)chunk.pos.x;
    int chY = (int)chunk.pos.y;
    return pX >= chX && pY >= chY && pX < chX + CHUNK_SIZE && pY < chY + CHUNK_SIZE;
}

sf::Image World::generateChunkTerrain(Chunk& chunk) {
    sf::Vector2f pos = chunk.pos;

    // Generator properties
    int intOctaves = 4;
    double warpSize = 4;
    double warpStrength = 0.8;
    double sampleRate = 0.0001; // 0.0001

    // Terrain levels
    double seaLevel = -0.2;
    double oceanMidRange = -0.1;
    double oceanShallowRange = 0;
    double sandRange = 0.05;
    double dirtLowRange = 0.3;
    double dirtHighRange = 0.65;
    double mountainLowRange = 0.7;
    double mountainMidRange = 0.8;
    double mountainHighRange = 0.7;

    int chX = pos.x;
    int chY = pos.y;

    std::vector<TERRAIN_TYPE> data(CHUNK_SIZE * CHUNK_SIZE);

    const siv::PerlinNoise perlin{ (siv::PerlinNoise::seed_type)_seed };

    sf::Image image;
    image.create(CHUNK_SIZE, CHUNK_SIZE, sf::Color::White);

    for (int y = chY; y < chY + CHUNK_SIZE; y++) {
        for (int x = chX; x < chX + CHUNK_SIZE; x++) {
            double warpNoise = perlin.octave2D_11(
                warpSize * ((double)x * sampleRate * 2),
                warpSize * ((double)y * sampleRate * 2), intOctaves
            );
            double warpNoise2 = perlin.octave2D_11(
                warpSize * ((double)x * sampleRate * 4),
                warpSize * ((double)y * sampleRate * 4), intOctaves
            );

            double val = perlin.octave3D_11(
                (x)*sampleRate, (y)*sampleRate,
                warpStrength * warpNoise * (warpStrength / 2) * warpNoise2, intOctaves
            );

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
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::NOT_WATER;
            } else if (val < dirtLowRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::DIRT_LOW;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::NOT_WATER;
            } else if (val < dirtHighRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::DIRT_HIGH;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::NOT_WATER;
            } else if (val < mountainLowRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::MOUNTAIN_LOW;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::NOT_WATER;
            } else if (val < mountainMidRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::MOUNTAIN_MID;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::NOT_WATER;
            } else {
                rgb = (sf::Uint32)TERRAIN_COLOR::MOUNTAIN_HIGH;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::NOT_WATER;
            }

            sf::Uint32 r = (rgb >> 16) & 0xFF;
            sf::Uint32 g = (rgb >> 8) & 0xFF;
            sf::Uint32 b = rgb & 0xFF;

            if (val >= sandRange || val < oceanShallowRange) {
                r += randomInt(0, 10);
                g += randomInt(0, 10);
                b += randomInt(0, 10);

                if (val >= oceanShallowRange && val < dirtLowRange) {
                    int ar = (int)(0x55 * (val)) | r;
                    int ag = (int)(0x55 * (val)) | g;
                    int ab = (int)(0x55 * (val)) | b;
                    r = ar;
                    g = ag;
                    b = ab;
                } else if (val >= dirtLowRange && val < dirtHighRange) {
                    r *= (val + 0.2) * 2;
                    g *= (val + 0.2) * 2;
                    b *= (val + 0.2) * 2;
                }

                rgb = r;
                rgb = (rgb << 8) + (g);
                rgb = (rgb << 8) + (b);
            } else if (val < sandRange && val >= oceanShallowRange) {
                rgb = r;
                rgb = (rgb << 8) + (g + randomInt(0, 10));
                rgb = (rgb << 8) + (b + randomInt(0, 10));
            }

            image.setPixel(x - chX, y - chY, sf::Color((rgb << 8) + 0xFF));
        }
    }

    chunk.terrainData = data;

    return image;
}

int World::getActiveChunkCount() {
    return _chunks.size();
}

Chunk* World::getCurrentChunk() {
    return _currentChunk;
}

TERRAIN_TYPE World::getTerrainDataAt(Chunk* chunk, sf::Vector2f pos) {
    if (chunk == nullptr || chunk->terrainData.size() == 0) return TERRAIN_TYPE::NOT_WATER;

    int x = (int)pos.x - (int)chunk->pos.x;
    int y = (int)pos.y - (int)chunk->pos.y;

    if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE) {
        return chunk->terrainData[x + y * CHUNK_SIZE];
    } else return TERRAIN_TYPE::NOT_WATER;
}

void World::loadSpriteSheet(std::shared_ptr<sf::Texture> spriteSheet) {
    _spriteSheet = spriteSheet;
}