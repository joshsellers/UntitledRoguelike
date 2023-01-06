#include "World.h"
#include <iostream>
#include <thread>
#include "Util.h"
#include "TallGrass.h"
#include "SmallTree.h"
#include "Cactus.h"
#include "SmallSavannaTree.h"
#include "LargeSavannaTree.h"
#include "SmallTundraTree.h"

World::World(std::shared_ptr<Player> player) {
    _player = player;
    _player->setWorld(this);

    _entities.push_back(_player);

    _seed = currentTimeMillis();
    srand(_seed);
    gen.seed(_seed);

    int pX = _player->getPosition().x + PLAYER_WIDTH / 2;
    int pY = _player->getPosition().y + PLAYER_HEIGHT;
    loadChunk(sf::Vector2f(pX - CHUNK_SIZE / 2, pY - CHUNK_SIZE / 2));

    if (!_font.loadFromFile("font.ttf")) {
        std::cout << "Failed to load font!" << std::endl;
    }
}

void World::update() {
    purgeEntityBuffer();

    updateEntities();

    int pX = ((int)_player->getPosition().x + PLAYER_WIDTH / 2);
    int pY = ((int)_player->getPosition().y + PLAYER_HEIGHT);

    eraseChunks(pX, pY);

    findCurrentChunk(pX, pY);

    loadNewChunks(pX, pY);
}

void World::draw(sf::RenderTexture& surface) {
    sortEntities();

    for (Chunk& chunk : _chunks) {
        surface.draw(chunk.sprite);
        /*sf::RectangleShape chunkoutline(sf::Vector2f(CHUNK_SIZE - 1, CHUNK_SIZE - 1));

        chunkoutline.setFillColor(sf::Color::Transparent);
        chunkoutline.setOutlineColor(sf::Color(0xffffffff));
        chunkoutline.setOutlineThickness(1);
        chunkoutline.setPosition(chunk.pos);
        surface.draw(chunkoutline);

        sf::Text idlabel;
        idlabel.setFont(_font);
        idlabel.setCharacterSize(10);
        idlabel.setString(std::to_string(chunk.id));
        idlabel.setPosition(chunk.pos.x, chunk.pos.y - 4);
        surface.draw(idlabel);*/
    }

    for (auto& entity : _entities) {
        entity->draw(surface);
    }
}

void World::purgeEntityBuffer() {
    if (_entityBuffer.size() != 0 && _loadingChunks.size() == 0) {
        for (auto& entity : _entityBuffer) {
            _entities.push_back(entity);
        }
        _entityBuffer.clear();
    }
}

void World::updateEntities() {
    // rewrite this so that entities that are not in any 
    // chunks are marked as do not render

    for (int j = 0; j < _entities.size(); j++) {
        auto& entity = _entities.at(j);

        if (!entity->isProp() && entity->isActive()) {
            entity->update();
            continue;
        } else if (!entity->isActive()) {
            _entities.erase(_entities.begin() + j);
        }

        int notInChunkCount = 0;
        for (auto& chunk : _chunks) {
            if (!chunkContains(chunk, entity->getPosition())) {
                notInChunkCount++;
            }
        }

        if (notInChunkCount == _chunks.size()) _entities.erase(_entities.begin() + j);
        else entity->update();
    }
}

void World::eraseChunks(int pX, int pY) {
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

            _chunks.erase(_chunks.begin() + i);
        }
    }
}

void World::findCurrentChunk(int pX, int pY) {
    for (Chunk& chunk : _chunks) {
        if (chunkContains(chunk, sf::Vector2f(pX, pY))) {
            _currentChunk = &chunk;
            break;
        }
    }
}

void World::loadNewChunks(int pX, int pY) {
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

void World::loadChunk(sf::Vector2f pos) {
    for (sf::Vector2i loadingChunk : _loadingChunks) {
        if (loadingChunk.x == (int)pos.x && loadingChunk.y == (int)pos.y) return;
    }
    for (Chunk& chunk : _chunks) {
        if (chunk.pos == pos) return;
    }
    _loadingChunks.push_back(sf::Vector2i((int)pos.x, (int)pos.y));
    std::thread buildThread(&World::buildChunk, this, pos);
    buildThread.detach();
}

void World::buildChunk(sf::Vector2f pos) {
    _mutex.lock();

    Chunk chunk(pos);

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

void World::generateChunkProps(Chunk& chunk) {
    int chX = chunk.pos.x;
    int chY = chunk.pos.y;

    int grassSpawnRate = 5000;
    int smallTreeSpawnRate = 50000;
    int cactusSpawnRate = 200000;
    int smallSavannaTreeSpawnRate = 200000;
    int largeSavannaTreeSpawnRate = 250000;
    int smallTundraTreeSpawnRate = 300000;

    srand(chX + chY * _seed);
    gen.seed(chX + chY * _seed);
    for (int y = chY; y < chY + CHUNK_SIZE; y++) {
        for (int x = chX; x < chX + CHUNK_SIZE; x++) {
            int dX = x - chX;
            int dY = y - chY;

            // TODO store prop sprite sizes somewhere and set their spawn position to x - w / 2, y - h
            TERRAIN_TYPE terrainType = chunk.terrainData[dX + dY * CHUNK_SIZE];
            if (terrainType == TERRAIN_TYPE::GRASS_LOW || terrainType == TERRAIN_TYPE::GRASS_HIGH) {
                boost::random::uniform_int_distribution<> grassDist(0, grassSpawnRate);
                boost::random::uniform_int_distribution<> treeDist(0, smallTreeSpawnRate);
                if (grassDist(gen) == 0) {
                    std::shared_ptr<TallGrass> grass = std::shared_ptr<TallGrass>(new TallGrass(sf::Vector2f(x, y), _spriteSheet));
                    _entityBuffer.push_back(grass);
                }

                if (treeDist(gen) == 0) {
                    std::shared_ptr<SmallTree> tree = std::shared_ptr<SmallTree>(new SmallTree(sf::Vector2f(x, y), _spriteSheet));
                    _entityBuffer.push_back(tree);
                }
            } else if (terrainType == TERRAIN_TYPE::DESERT) {
                boost::random::uniform_int_distribution<> cactusDist(0, cactusSpawnRate);
                if (cactusDist(gen) == 0) {
                    std::shared_ptr<Cactus> cactus = std::shared_ptr<Cactus>(new Cactus(sf::Vector2f(x, y), _spriteSheet));
                    _entityBuffer.push_back(cactus);
                }
            } else if (terrainType == TERRAIN_TYPE::SAVANNA) {
                boost::random::uniform_int_distribution<> smallTreeDist(0, smallSavannaTreeSpawnRate);
                boost::random::uniform_int_distribution<> largeTreeDist(0, largeSavannaTreeSpawnRate);
                if (smallTreeDist(gen) == 0) {
                    std::shared_ptr<SmallSavannaTree> tree = std::shared_ptr<SmallSavannaTree>(new SmallSavannaTree(sf::Vector2f(x, y), _spriteSheet));
                    _entityBuffer.push_back(tree);
                }

                if (largeTreeDist(gen) == 0) {
                    std::shared_ptr<LargeSavannaTree> tree = std::shared_ptr<LargeSavannaTree>(new LargeSavannaTree(sf::Vector2f(x, y), _spriteSheet));
                    _entityBuffer.push_back(tree);
                }
            } else if (terrainType == TERRAIN_TYPE::TUNDRA) {
                boost::random::uniform_int_distribution<> smallTreeDist(0, smallTundraTreeSpawnRate);
                if (smallTreeDist(gen) == 0) {
                    std::shared_ptr<SmallTundraTree> tree = std::shared_ptr<SmallTundraTree>(new SmallTundraTree(sf::Vector2f(x, y), _spriteSheet));
                    _entityBuffer.push_back(tree);
                }
            }
        }
    }
}

sf::Image World::generateChunkTerrain(Chunk& chunk) {
    sf::Vector2f pos = chunk.pos;

    // Generator properties
    int octaves = 4;
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
                warpSize * ((double)y * sampleRate * 2), octaves
            );
            double warpNoise2 = perlin.octave2D_11(
                warpSize * ((double)x * sampleRate * 4),
                warpSize * ((double)y * sampleRate * 4), octaves
            );

            double val = perlin.octave3D_11(
                (x)*sampleRate, (y)*sampleRate,
                warpStrength * warpNoise * (warpStrength / 2) * warpNoise2, octaves
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
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::SAND;
            } else if (val < dirtLowRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::DIRT_LOW;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::GRASS_LOW;
            } else if (val < dirtHighRange) {
                rgb = (sf::Uint32)TERRAIN_COLOR::DIRT_HIGH;
                data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::GRASS_HIGH;
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
            int biomeOctaves = 2;
            float biomeSampleRate = 0.00001;
            double temperatureNoise = perlin.normalizedOctave3D_01(x * biomeSampleRate, y * biomeSampleRate, 10, biomeOctaves);
            double precipitationNoise = perlin.normalizedOctave3D_01(x * biomeSampleRate, y * biomeSampleRate, 40, biomeOctaves);

            bool tundra = temperatureNoise < 0.4 && precipitationNoise >= 0.3 && precipitationNoise < 0.6;
            bool desert = temperatureNoise > 0.6 && precipitationNoise < 0.4;
            bool savanna = temperatureNoise > 0.6 && precipitationNoise >= 0.375 && precipitationNoise < 0.6;

            TERRAIN_TYPE terrainType = data[dX + dY * CHUNK_SIZE];
            if (terrainType == TERRAIN_TYPE::GRASS_LOW || terrainType == TERRAIN_TYPE::GRASS_HIGH) {
                bool high = terrainType == TERRAIN_TYPE::GRASS_HIGH;
                if (tundra) {
                    data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::TUNDRA;
                    rgb = (sf::Uint32)TERRAIN_COLOR::TUNDRA;
                } else if (desert) {
                    data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::DESERT;
                    rgb = (sf::Uint32)TERRAIN_COLOR::DESERT;
                } else if (savanna) {
                    data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::SAVANNA;
                    rgb = (sf::Uint32)TERRAIN_COLOR::SAVANNA;
                }
            }
            terrainType = data[dX + dY * CHUNK_SIZE];

            sf::Uint32 r = (rgb >> 16) & 0xFF;
            sf::Uint32 g = (rgb >> 8) & 0xFF;
            sf::Uint32 b = rgb & 0xFF;

            if (terrainType != TERRAIN_TYPE::SAND && terrainType != TERRAIN_TYPE::DESERT) {
                r += randomInt(0, 10);
                g += randomInt(0, 10);
                b += randomInt(0, 10);

                if (terrainType == TERRAIN_TYPE::GRASS_LOW || terrainType == TERRAIN_TYPE::SAVANNA) {
                    int ar = (int)(0x55 * (val)) | r;
                    int ag = (int)(0x55 * (val)) | g;
                    int ab = (int)(0x55 * (val)) | b;
                    r = ar;
                    g = ag;
                    b = ab;
                } else if (terrainType == TERRAIN_TYPE::GRASS_HIGH) {
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

    generateChunkProps(chunk);

    return image;
}

int World::getActiveChunkCount() {
    return _chunks.size();
}

Chunk* World::getCurrentChunk() {
    return _currentChunk;
}

TERRAIN_TYPE World::getTerrainDataAt(Chunk* chunk, sf::Vector2f pos) {
    if (chunk == nullptr || chunk->terrainData.size() == 0) return TERRAIN_TYPE::VOID;

    int x = (int)pos.x - (int)chunk->pos.x;
    int y = (int)pos.y - (int)chunk->pos.y;

    if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE) {
        return chunk->terrainData[x + y * CHUNK_SIZE];
    } else return TERRAIN_TYPE::VOID;
}

void World::loadSpriteSheet(std::shared_ptr<sf::Texture> spriteSheet) {
    _spriteSheet = spriteSheet;
}

std::shared_ptr<sf::Texture> World::getSpriteSheet() const {
    return _spriteSheet;
}

int World::getSeed() {
    return _seed;
}

void World::addEntity(std::shared_ptr<Entity> entity) {
    _entities.push_back(entity);
}

std::vector<std::shared_ptr<Entity>> World::getEntities() const {
    return _entities;
}

void World::sortEntities() {
    int n = _entities.size();

    for (int i = 0; i < n - 1; i++) {
        int min = i;
        for (int j = i + 1; j < n; j++) {
            std::shared_ptr<Entity> minEntity = _entities.at(min);
            std::shared_ptr<Entity> jEntity = _entities.at(j);
            if (minEntity->getPosition().y + minEntity->getSprite().getGlobalBounds().height
                > jEntity->getPosition().y + jEntity->getSprite().getGlobalBounds().height) {
                min = j;
            }
        }

        std::shared_ptr<Entity> key = _entities.at(min);
        while (min > i) {
            _entities[min] = _entities[min - 1];
            min--;
        }
        _entities[i] = key;
    }
}