#include "World.h"
#include <iostream>
#include "PerlinNoise.hpp"
#include <thread>

World::World(Player* player) {
    _player = player;

    int pX = _player->getPosition().x;
    int pY = _player->getPosition().y;
    loadChunk(sf::Vector2f(pX - CHUNK_SIZE / 2, pY - CHUNK_SIZE / 2));

    if (!_font.loadFromFile("font.ttf")) {
        std::cout << "Failed to load font!" << std::endl;
    }
}

void World::update() {
    Chunk* currentChunk = nullptr;
    int pX = _player->getPosition().x + 5;
    int pY = _player->getPosition().y + 5;

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
            std::cout << "erasing chunk " << chunk.id << std::endl;
            _chunks.erase(_chunks.begin() + i);
        }
    }

    for (Chunk& chunk : _chunks) {
        if (chunkContains(chunk, sf::Vector2f(pX, pY))) {
            currentChunk = &chunk;
            break;
        }
    }

    if (currentChunk != nullptr) {
        int chX = currentChunk->pos.x;
        int chY = currentChunk->pos.y;

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
    } else {
        std::cout << "currentChunk was nullptr" << std::endl;
    }
}

void World::draw(sf::RenderTexture& surface) {
    for (Chunk& chunk : _chunks) {
        surface.draw(chunk.sprite);
        sf::RectangleShape chunkOutline(sf::Vector2f(CHUNK_SIZE-1, CHUNK_SIZE-1));
        chunkOutline.setFillColor(sf::Color::Transparent);
        chunkOutline.setOutlineColor(sf::Color(0xFFFFFFFF));
        chunkOutline.setOutlineThickness(1);
        chunkOutline.setPosition(chunk.pos);
        surface.draw(chunkOutline);

        sf::Text idLabel;
        idLabel.setFont(_font);
        idLabel.setCharacterSize(10);
        idLabel.setString(std::to_string(chunk.id));
        idLabel.setPosition(chunk.pos.x, chunk.pos.y - 4);
        surface.draw(idLabel);
    }
}

void World::loadChunk(sf::Vector2f pos) {
    for (Chunk& chunk : _chunks) {
        if (chunk.pos == pos) {
            std::cout << "chunk " << chunk.id << " already exists, did not load" << std::endl;
            return;
        }
    }
    std::thread buildThread(&World::buildChunk, this, pos);
    buildThread.detach();
}

void World::buildChunk(sf::Vector2f pos) {
    Chunk chunk(pos);
    std::cout << "loading chunk " << chunk.id << std::endl;
    chunk.texture->create(CHUNK_SIZE, CHUNK_SIZE);
    chunk.texture->update(generateChunkTerrain(chunk.pos));
    chunk.sprite.setTexture(*chunk.texture);
    chunk.sprite.setPosition(chunk.pos);
    _chunks.push_back(chunk);
}

bool World::chunkContains(Chunk& chunk, sf::Vector2f pos) {
    int pX = pos.x;
    int pY = pos.y;
    int chX = chunk.pos.x;
    int chY = chunk.pos.y;
    return pX >= chX && pY >= chY && pX < chX + CHUNK_SIZE && pY < chY + CHUNK_SIZE;
}

sf::Image World::generateChunkTerrain(sf::Vector2f pos) {
    // Generator properties
    int intOctaves = 4; 
    double warpSize = 4;
    double warpStrength = 0.8;
    double sampleRate = 0.005;

    // Terrain levels
    double seaLevel = 0.5;
    double oceanMidRange = 0.045;
    double oceanShallowRange = 0.01;
    double sandRange = 0.02;
    double dirtHighRange = 0.05;
    double mountainLowRange = 0.1;
    double mountainMidRange = 0.1;
    double mountainHighRange = 0.2;

    int chX = pos.x;
    int chY = pos.y;

    //std::vector<double> data(CHUNK_SIZE * CHUNK_SIZE);

    const siv::PerlinNoise perlin{ (siv::PerlinNoise::seed_type)_seed };

    sf::Image image;
    image.create(CHUNK_SIZE, CHUNK_SIZE, sf::Color::White);

    for (int y = chY; y < chY + CHUNK_SIZE; y++) {
        for (int x = chX; x < chX + CHUNK_SIZE; x++) {
            double warpNoise = perlin.octave2D_01(
                warpSize * ((double)x * sampleRate),
                warpSize * ((double)y * sampleRate), intOctaves
            );
            double val = perlin.octave3D_01(
                (x)*sampleRate, (y)*sampleRate,
                warpStrength * warpNoise, intOctaves
            );

            //data[x + y * CHUNK_SIZE] = noise;

            sf::Uint32 rgb = 0x00;

            if (val >= seaLevel + mountainMidRange + mountainHighRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::MOUNTAIN_HIGH;
            } else if (val >= seaLevel + mountainLowRange + mountainMidRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::MOUNTAIN_MID;
            } else if (val >= seaLevel + dirtHighRange + mountainLowRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::MOUNTAIN_LOW;
            } else if (val >= seaLevel + sandRange + dirtHighRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::DIRT_HIGH;
            } else if (val >= seaLevel + sandRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::DIRT_LOW;
            } else if (val > seaLevel && val < seaLevel + sandRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::SAND;
            } else if (val > seaLevel - oceanShallowRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::WATER_SHALLOW;
            } else if (val > seaLevel - oceanMidRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::WATER_MID;
            } else if (val < seaLevel) {
                rgb = (sf::Uint32)TERRAIN_COLOR::WATER_DEEP;
            }

            image.setPixel(x - chX, y - chY, sf::Color((rgb << 8) + 0xFF));
        }
    }

    return image;
}

int World::getActiveChunkCount() {
    return _chunks.size();
}
