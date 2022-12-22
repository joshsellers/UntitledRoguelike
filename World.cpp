#include "World.h"
#include <iostream>

World::World(Player* player) {
    _player = player;

    int pX = _player->getPosition().x;
    int pY = _player->getPosition().y;
    Chunk chunk(sf::Vector2f(pX - CHUNK_SIZE / 2, pY - CHUNK_SIZE / 2));
    _chunks.push_back(chunk);
}

void World::update() {
    Chunk* currentChunk = nullptr;
    int pX = _player->getPosition().x + 5;
    int pY = _player->getPosition().y + 5;

    for (int i = 0; i < _chunks.size(); i++) {
        Chunk& chunk = _chunks.at(i);

        int chX = chunk.pos.x;
        int chY = chunk.pos.y;


        bool left = std::abs(pX - chX) < CHUNK_LOAD_THRESHOLD;
        bool top = std::abs(pY - chY) < CHUNK_LOAD_THRESHOLD;
        bool right = std::abs(pX - (chX + (int)CHUNK_SIZE)) < CHUNK_LOAD_THRESHOLD;
        bool bottom = std::abs(pY - (chY + (int)CHUNK_SIZE)) < CHUNK_LOAD_THRESHOLD;

        if ((!left || !right) && (!top || !bottom) && !chunkContains(chunk, sf::Vector2f(pX, pY))) {
            _chunks.erase(_chunks.begin() + i);
        }
    }

    for (Chunk& chunk : _chunks) {
        int chX = chunk.pos.x;
        int chY = chunk.pos.y;

        if (chunkContains(chunk, sf::Vector2f(pX, pY))) {
            currentChunk = &chunk;
            break;
        }
    }

    int chX = currentChunk->pos.x;
    int chY = currentChunk->pos.y;

    bool left = std::abs(pX - chX) < CHUNK_LOAD_THRESHOLD;
    bool top = std::abs(pY - chY) < CHUNK_LOAD_THRESHOLD;
    bool right = std::abs(pX - (chX + (int)CHUNK_SIZE)) < CHUNK_LOAD_THRESHOLD;
    bool bottom = std::abs(pY - (chY + (int)CHUNK_SIZE)) < CHUNK_LOAD_THRESHOLD;

    // left
    if (left) {
        loadChunk(sf::Vector2f(currentChunk->pos.x - CHUNK_SIZE, currentChunk->pos.y));
    }
    // top
    if (top) {
        loadChunk(sf::Vector2f(currentChunk->pos.x, currentChunk->pos.y - CHUNK_SIZE));
    }
    // top left corner
    if (top && left) {
        loadChunk(sf::Vector2f(currentChunk->pos.x - CHUNK_SIZE, currentChunk->pos.y - CHUNK_SIZE));
    }
    // right
    if (right) {
        loadChunk(sf::Vector2f(currentChunk->pos.x + CHUNK_SIZE, currentChunk->pos.y));
    }
    // bottom
    if (bottom) {
        loadChunk(sf::Vector2f(currentChunk->pos.x, currentChunk->pos.y + CHUNK_SIZE));
    }
    // bottom right corner 
    if (bottom && right) {
        loadChunk(sf::Vector2f(currentChunk->pos.x + CHUNK_SIZE, currentChunk->pos.y + CHUNK_SIZE));
    }
    // bottom left corner
    if (bottom && left) {
        loadChunk(sf::Vector2f(currentChunk->pos.x - CHUNK_SIZE, currentChunk->pos.y + CHUNK_SIZE));
    }
    // top right corner 
    if (top && right) {
        loadChunk(sf::Vector2f(currentChunk->pos.x + CHUNK_SIZE, currentChunk->pos.y - CHUNK_SIZE));
    }
}

void World::draw(sf::RenderTexture& surface) {
    for (Chunk& chunk : _chunks) {
        sf::RectangleShape chunkOutline(sf::Vector2f(CHUNK_SIZE, CHUNK_SIZE));
        chunkOutline.setFillColor(sf::Color::Transparent);
        //chunkOutline.setOutlineColor(sf::Color(0xFFFFFFFF));
        chunkOutline.setOutlineThickness(1);
        chunkOutline.setPosition(chunk.pos);
        surface.draw(chunkOutline);
    }
}

void World::loadChunk(sf::Vector2f pos) {
    for (Chunk& chunk : _chunks) {
        if (chunk.pos == pos) {
            std::cout << "chunk " << chunk.id << " already exists, did not load" << std::endl;
            return;
        }
    }
    std::cout << "loaded a chunk" << std::endl;
    _chunks.push_back(Chunk(pos));
}

bool World::chunkContains(Chunk& chunk, sf::Vector2f pos) {
    int pX = pos.x;
    int pY = pos.y;
    int chX = chunk.pos.x;
    int chY = chunk.pos.y;
    return pX > chX && pY > chY && pX < chX + CHUNK_SIZE && pY < chY + CHUNK_SIZE;
}
