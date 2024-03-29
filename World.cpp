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
#include "Penguin.h"
#include "Turtle.h"
#include "PlantMan.h"
#include "Globals.h"
#include "Cactoid.h"
#include "MessageManager.h"
#include "ShopInterior.h"
#include "ShopCounter.h"
#include "ShopExterior.h"
#include "Frog.h"
#include "ShopKeep.h"

World::World(std::shared_ptr<Player> player, bool& showDebug) : _showDebug(showDebug) {
    _player = player;
    _player->setWorld(this);

    _entities.push_back(_player);

    if (!_font.loadFromFile("res/font.ttf")) {
        MessageManager::displayMessage("Failed to load font!", 10, WARN);
    }
}

void World::init(unsigned int seed) {
    _seed = seed;
    srand(_seed);
    gen.seed(_seed);

    loadChunksAroundPlayer();
}

void World::loadChunksAroundPlayer() {
    if (getActiveChunkCount() == 0 && _loadingChunks.size() == 0) {
        MessageManager::displayMessage("loading chunks around player", 5, DEBUG);

        int pX = _player->getPosition().x + PLAYER_WIDTH / 2;
        int pY = _player->getPosition().y + PLAYER_HEIGHT;

        int chX = 0;
        int chY = 0;

        for (int y = pY - CHUNK_SIZE / 2; y < pY + CHUNK_SIZE / 2; y++) {
            for (int x = pX - CHUNK_SIZE / 2; x < pX + CHUNK_SIZE / 2; x++) {
                if (y % CHUNK_SIZE == 0 && x % CHUNK_SIZE == 0) {
                    chX = x;
                    chY = y;
                }
            }
        }

        int dist = 1;
        int count = 0;
        for (int y = chY - (CHUNK_SIZE * dist); y < chY + (CHUNK_SIZE * dist); y += CHUNK_SIZE) {
            for (int x = chX - (CHUNK_SIZE * dist); x < chX + (CHUNK_SIZE * dist); x += CHUNK_SIZE) {
                loadChunk(sf::Vector2f(x, y));
            }
        }
    } else if (getActiveChunkCount() != 0) {
        MessageManager::displayMessage("loadChunksAroundPlayer was called while there were active chunks", 10, WARN);
    } else if (_loadingChunks.size() != 0) {
        MessageManager::displayMessage("loadChunksAroundPlayer was called while chunks were already loading", 10, WARN);
    }
}

void World::update() {
    if (!_isPlayerInShop) {
        if (getActiveChunkCount() == 0 && _loadingChunks.size() == 0) loadChunksAroundPlayer();

        if (!disableMobSpawning) spawnMobs();

        purgeEntityBuffer();

        updateEntities();

        int pX = ((int)_player->getPosition().x + PLAYER_WIDTH / 2);
        int pY = ((int)_player->getPosition().y + PLAYER_HEIGHT);

        eraseChunks(pX, pY);

        findCurrentChunk(pX, pY);

        loadNewChunks(pX, pY);

        if (_maxEnemiesReached && !_cooldownActive && getEnemyCount() == 0) {
            _cooldownActive = true;
            _maxEnemiesReached = false;
            _cooldownStartTime = currentTimeMillis();
        } else if (_cooldownActive && currentTimeMillis() - _cooldownStartTime >= _enemySpawnCooldownTimeMilliseconds) {
            _cooldownActive = false;
        }
    } else {
        _player->update();
        for (auto& entity : getEntities()) {
            if (entity->getEntityType() == "shopint" || entity->getEntityType() == "shopcounter" || entity->getEntityType() == "shopext") entity->update();
        }
    }
}

void World::draw(sf::RenderTexture& surface) {
    sortEntities();

    if (!_isPlayerInShop) {
        for (Chunk& chunk : _chunks) {
            surface.draw(chunk.sprite);
        }
    }

    if (drawChunkOutline) {
        for (Chunk& chunk : _chunks) {
            sf::RectangleShape chunkoutline(sf::Vector2f(CHUNK_SIZE - 1, CHUNK_SIZE - 1));

            chunkoutline.setFillColor(sf::Color::Transparent);
            chunkoutline.setOutlineColor(sf::Color(0xffffffff));
            chunkoutline.setOutlineThickness(2);
            chunkoutline.setPosition(chunk.pos);
            surface.draw(chunkoutline);

            sf::Text idlabel;
            idlabel.setFont(_font);
            idlabel.setCharacterSize(10);
            idlabel.setString(std::to_string(chunk.id));
            idlabel.setPosition(chunk.pos.x, chunk.pos.y - 4);
            surface.draw(idlabel);
        }
    }

    for (auto& entity : _entities) {
        if (!entity->isDormant() && !_isPlayerInShop 
            || (_isPlayerInShop && entity->getEntityType() == "shopint" 
                || entity->getEntityType() == "player" 
                || entity->getEntityType() == "shopcounter" 
                || entity->getEntityType() == "shopkeep")) entity->draw(surface);
        
        if (showDebug() && entity->isDamageable()) {
            sf::RectangleShape hitBox;
            sf::FloatRect box = entity->getHitBox();
            hitBox.setPosition(box.left, box.top);
            hitBox.setSize(sf::Vector2f(box.width, box.height));
            hitBox.setFillColor(sf::Color::Transparent);
            hitBox.setOutlineColor(sf::Color(0xFF0000FF));
            hitBox.setOutlineThickness(1.f);
            surface.draw(hitBox);

            if (entity->hasColliders()) {
                for (auto& collider : entity->getColliders()) {
                    sf::RectangleShape colliderBox;
                    colliderBox.setPosition(collider.left, collider.top);
                    colliderBox.setSize(sf::Vector2f(collider.width, collider.height));
                    colliderBox.setFillColor(sf::Color::Transparent);
                    colliderBox.setOutlineColor(sf::Color(0xFFFF00FF));
                    colliderBox.setOutlineThickness(1.f);
                    surface.draw(colliderBox);
                }
            }

            if (entity->isDormant()) {
                sf::CircleShape dormantMarker;
                dormantMarker.setPosition(entity->getPosition());
                dormantMarker.setRadius(25);
                dormantMarker.setFillColor(sf::Color(0x00FF00FF));
                surface.draw(dormantMarker);
            }
        }
    }
}

void World::spawnMobs() {
    constexpr float MOB_SPAWN_RATE_SECONDS = 0.5f;
    constexpr int MOB_SPAWN_CHANCE = 5;
    constexpr float MIN_DIST = 250.f;
    constexpr int PACK_SPREAD = 20;

    if (_mobSpawnClock.getElapsedTime().asSeconds() >= MOB_SPAWN_RATE_SECONDS) {
        _mobSpawnClock.restart();
        boost::random::uniform_int_distribution<> skipChunk(0, MOB_SPAWN_CHANCE);
        for (auto& chunk : _chunks) {
            if (skipChunk(_mobGen) == 0) {
                boost::random::uniform_int_distribution<> randX(chunk.pos.x, chunk.pos.x + CHUNK_SIZE);
                boost::random::uniform_int_distribution<> randY(chunk.pos.y, chunk.pos.y + CHUNK_SIZE);
                float x = randX(_mobGen);
                float y = randY(_mobGen);

                if (std::abs(x - _player->getPosition().x) > MIN_DIST || std::abs(y - _player->getPosition().y) > MIN_DIST) {
                    TERRAIN_TYPE terrainType = getTerrainDataAt(&chunk, sf::Vector2f(x, y));
                    if ((unsigned int)terrainType < (unsigned int)TERRAIN_TYPE::WATER) continue;

                    unsigned int biomeIndex = (unsigned int)terrainType - (unsigned int)TERRAIN_TYPE::WATER;
                    const BiomeMobSpawnData& biomeMobSpawnData = MOB_SPAWN_DATA[biomeIndex];
                    if (biomeMobSpawnData.mobData.size() == 0) continue;

                    const MobSpawnData& mobData = biomeMobSpawnData.mobData.at(getRandMobType(biomeMobSpawnData));
                    boost::random::uniform_int_distribution<> individualSpawnChance(0, mobData.spawnChance);
                    if (individualSpawnChance(_mobGen) != 0) continue;

                    if ((int)mobData.mobType > (int)MOB_TYPE::CACTOID && (_cooldownActive || _maxEnemiesReached)) continue;
                    else if ((int)mobData.mobType <= (int)MOB_TYPE::CACTOID && getMobCount() > MAX_ACTIVE_MOBS) continue;

                    boost::random::uniform_int_distribution<> randPackAmount(mobData.minPackSize, mobData.maxPackSize);
                    int packAmount = randPackAmount(_mobGen);

                    boost::random::uniform_int_distribution<> randXi(x - PACK_SPREAD, x + PACK_SPREAD);
                    boost::random::uniform_int_distribution<> randYi(y - PACK_SPREAD, y + PACK_SPREAD);
                    for (int i = 0; i < packAmount; i++) {
                        int xi = randXi(_mobGen);
                        int yi = randYi(_mobGen);
                        std::shared_ptr<Entity> mob = nullptr;
                        
                        switch (mobData.mobType) {
                            case MOB_TYPE::PENGUIN:
                                mob = std::shared_ptr<Penguin>(new Penguin(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::TURTLE:
                                mob = std::shared_ptr<Turtle>(new Turtle(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::PLANT_MAN:
                                mob = std::shared_ptr<PlantMan>(new PlantMan(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::CACTOID:
                                mob = std::shared_ptr<Cactoid>(new Cactoid(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::FROG:
                                mob = std::shared_ptr<Frog>(new Frog(sf::Vector2f(xi, yi)));
                                break;
                            default:
                                return;
                        }

                        mob->loadSprite(_spriteSheet);
                        mob->setWorld(this);
                        addEntity(mob);

                        if ((int)mobData.mobType > (int)MOB_TYPE::CACTOID
                            && getEnemyCount() >= _maxActiveEnemies) {
                            _maxEnemiesReached = true;
                            _enemySpawnCooldownTimeMilliseconds = randomInt(MIN_ENEMY_SPAWN_COOLDOWN_TIME_MILLISECONDS, MAX_ENEMY_SPAWN_COOLDOWN_TIME_MILLISECONDS);
                            _maxActiveEnemies = (int)((12.f * std::log(std::pow(PLAYER_SCORE, 2)) * std::log(PLAYER_SCORE / 2) + 5) * 0.5f);
                            if (_maxActiveEnemies == 2) _maxActiveEnemies = 6;
                            PLAYER_SCORE += 1.f;
                            break;
                        }
                    }
                }
            } else continue;
        }
    }
}

int World::getRandMobType(const BiomeMobSpawnData& mobSpawnData) {
    boost::random::uniform_int_distribution<> randMobType(0, mobSpawnData.mobData.size()-1);
    return randMobType(_mobGen);
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

    for (int i = 0; i < _entities.size(); i++) {
        auto& entity = _entities.at(i);
        if (!entity->isActive()) _entities.erase(_entities.begin() + i);
    }

    for (int j = 0; j < _entities.size(); j++) {
        auto& entity = _entities.at(j);

        if (!entity->isProp() && entity->isActive() && !entity->isMob()) {
            entity->update();
            continue;
        } else if (!entity->isActive()) {
            // this crashes the game if there's a bunch of projectiles 
            //_entities.erase(_entities.begin() + j);
        } else if (entity->isDormant()) {
            if (entity->getDormancyTime() >= entity->getDormancyTimeout()) {
                entity->deactivate();
                continue;
            }

            entity->incrementDormancyTimer();
        }

        int notInChunkCount = 0;
        for (auto& chunk : _chunks) {
            if (!chunkContains(chunk, entity->getPosition())) {
                notInChunkCount++;
            }
        }

        if (notInChunkCount == _chunks.size() && entity->isProp()) _entities.erase(_entities.begin() + j);
        else if (notInChunkCount == _chunks.size() && entity->isMob() && !entity->isDormant()) {
            if (entity->getTimeOutOfChunk() >= entity->getMaxTimeOutOfChunk()) {
                entity->setDormant(true);
                continue;
            }

            entity->incrementOutOfChunkTimer();
            entity->update();
        } else if (notInChunkCount < _chunks.size() && entity->isMob() && entity->isDormant()) {
            entity->setDormant(false);
            entity->resetDormancyTimer();
        } else if (notInChunkCount < _chunks.size() && entity->isMob()) {
            entity->resetOutOfChunkTimer();
            entity->update();
        }
        else if (!entity->isDormant()) entity->update();
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
        MessageManager::displayMessage("currentChunk was nullptr", 2, DEBUG);
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

void World::generateChunkEntities(Chunk& chunk) {
    int chX = chunk.pos.x;
    int chY = chunk.pos.y;

    const int shopSpawnRate = 5000000;
    const int grassSpawnRate = 5000;
    const int smallTreeSpawnRate = 37500;
    const int cactusSpawnRate = 200000;
    const int smallSavannaTreeSpawnRate = 200000;
    const int largeSavannaTreeSpawnRate = 250000;
    const int smallTundraTreeSpawnRate = 300000;

    srand(chX + chY * _seed);
    gen.seed(chX + chY * _seed);
    for (int y = chY; y < chY + CHUNK_SIZE; y++) {
        for (int x = chX; x < chX + CHUNK_SIZE; x++) {
            int dX = x - chX;
            int dY = y - chY;

            TERRAIN_TYPE terrainType = chunk.terrainData[dX + dY * CHUNK_SIZE];
            if (terrainType != TERRAIN_TYPE::WATER && terrainType != TERRAIN_TYPE::EMPTY) {
                boost::random::uniform_int_distribution<> shopDist(0, shopSpawnRate);
                if (shopDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<ShopExterior> shop = std::shared_ptr<ShopExterior>(new ShopExterior(sf::Vector2f(x, y), _spriteSheet));
                    shop->setWorld(this);
                    _entityBuffer.push_back(shop);

                    MessageManager::displayMessage("There's a shop around here somewhere!", 5);
                }
            }

            if (terrainType == TERRAIN_TYPE::GRASS) {
                boost::random::uniform_int_distribution<> grassDist(0, grassSpawnRate);
                boost::random::uniform_int_distribution<> treeDist(0, smallTreeSpawnRate);
                if (grassDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<TallGrass> grass = std::shared_ptr<TallGrass>(new TallGrass(sf::Vector2f(x, y), _spriteSheet));
                    _entityBuffer.push_back(grass);
                }

                if (treeDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<SmallTree> tree = std::shared_ptr<SmallTree>(new SmallTree(sf::Vector2f(x, y), _spriteSheet));
                    tree->setWorld(this);
                    _entityBuffer.push_back(tree);
                }
            } else if (terrainType == TERRAIN_TYPE::DESERT) {
                boost::random::uniform_int_distribution<> cactusDist(0, cactusSpawnRate);
                if (cactusDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<Cactus> cactus = std::shared_ptr<Cactus>(new Cactus(sf::Vector2f(x, y), _spriteSheet));
                    cactus->setWorld(this);
                    _entityBuffer.push_back(cactus);
                }
            } else if (terrainType == TERRAIN_TYPE::SAVANNA) {
                boost::random::uniform_int_distribution<> smallTreeDist(0, smallSavannaTreeSpawnRate);
                boost::random::uniform_int_distribution<> largeTreeDist(0, largeSavannaTreeSpawnRate);
                if (smallTreeDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<SmallSavannaTree> tree = std::shared_ptr<SmallSavannaTree>(new SmallSavannaTree(sf::Vector2f(x, y), _spriteSheet));
                    tree->setWorld(this);
                    _entityBuffer.push_back(tree);
                }

                if (largeTreeDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<LargeSavannaTree> tree = std::shared_ptr<LargeSavannaTree>(new LargeSavannaTree(sf::Vector2f(x, y), _spriteSheet));
                    tree->setWorld(this);
                    _entityBuffer.push_back(tree);
                }
            } else if (terrainType == TERRAIN_TYPE::TUNDRA) {
                boost::random::uniform_int_distribution<> smallTreeDist(0, smallTundraTreeSpawnRate);
                if (smallTreeDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<SmallTundraTree> tree = std::shared_ptr<SmallTundraTree>(new SmallTundraTree(sf::Vector2f(x, y), _spriteSheet));
                    tree->setWorld(this);
                    _entityBuffer.push_back(tree);
                }
            }
        }
    }
}

sf::Image World::generateChunkTerrain(Chunk& chunk) {
    long long startTime = 0;
    long long endTime = 0;
    if (BENCHMARK_TERRAIN_AND_BIOME_GEN) startTime = currentTimeMillis();

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
            double xOffset = 20000.;
            double yOffset = 20000.;
            int biomeOctaves = 2;
            double biomeSampleRate = 0.00001;
            double temperatureNoise = perlin.normalizedOctave3D_01((x + xOffset) * biomeSampleRate, (y + yOffset) * biomeSampleRate, 10, biomeOctaves);
            double precipitationNoise = perlin.normalizedOctave3D_01((x + xOffset) * biomeSampleRate, (y + yOffset) * biomeSampleRate, 40, biomeOctaves);

            /*float tundraTemp = 0.460 + 0.0075;
            float tundraPrecLow = 0.240 - 0.0075;
            float tundraPrecHigh = 0.500 + 0.0075;

            float desertTemp = 0.540 - 0.0075;
            float desertPrec = 0.460 + 0.0075;
            
            float savannaTemp = 0.540 - 0.0075;
            float savannaPrecLow = 0.315 - 0.0075;
            float savannaPrecHigh = 0.660 + 0.0075;

            bool tundra = temperatureNoise < tundraTemp && precipitationNoise >= tundraPrecLow && precipitationNoise < tundraPrecHigh;
            bool desert = temperatureNoise > desertTemp && precipitationNoise < desertPrec;
            bool savanna = temperatureNoise > savannaTemp && precipitationNoise >= savannaPrecLow && precipitationNoise < savannaPrecHigh;*/

            sf::Vector2f tundraTemp(0.0, 0.4);
            sf::Vector2f tundraPrec(0.1, 0.9);
            
            sf::Vector2f desertTemp(0.5, 0.6);
            sf::Vector2f desertPrec(0.0, 0.5);

            sf::Vector2f savannaTemp(0.5, 0.6);
            sf::Vector2f savannaPrec(0.5, 0.7);

            bool tundra = temperatureNoise > tundraTemp.x && temperatureNoise < tundraTemp.y && precipitationNoise > tundraPrec.x && precipitationNoise < tundraPrec.y;
            bool desert = temperatureNoise > desertTemp.x && temperatureNoise < desertTemp.y && precipitationNoise > desertPrec.x && precipitationNoise < desertPrec.y;
            bool savanna = temperatureNoise > savannaTemp.x && temperatureNoise < savannaTemp.y && precipitationNoise > savannaPrec.x && precipitationNoise < savannaPrec.y;

            // rare biomes 
            double rareBiomeSampleRate = biomeSampleRate / 2.;
            double rareBiomeTemp = perlin.noise3D_01((x + xOffset) * rareBiomeSampleRate, (y + yOffset) * rareBiomeSampleRate, 8);
            double rareBiomePrec = perlin.noise3D_01((x + xOffset) * rareBiomeSampleRate, (y + yOffset) * rareBiomeSampleRate, 34);

            sf::Vector2f fleshTemp(0.0005, 0.3);
            sf::Vector2f fleshPrec(0.04, 0.7);

            bool flesh = rareBiomeTemp > fleshTemp.x && rareBiomeTemp < fleshTemp.y && rareBiomePrec > fleshPrec.x && rareBiomePrec < fleshPrec.y;



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
                }
                
                if (flesh) {
                    data[dX + dY * CHUNK_SIZE] = TERRAIN_TYPE::FLESH;
                    rgb = (sf::Uint32)TERRAIN_COLOR::FLESH;
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

    if (BENCHMARK_TERRAIN_AND_BIOME_GEN) {
        endTime = currentTimeMillis();
        MessageManager::displayMessage("Terrain for chunk at (" + std::to_string(chunk.pos.x) + ", " + std::to_string(chunk.pos.y)
            + ") generated in " + std::to_string(endTime - startTime) + "ms", 2, DEBUG);
    }

    if (!disablePropGeneration) generateChunkEntities(chunk);

    return image;
}

int World::getActiveChunkCount() {
    return _chunks.size();
}

Chunk* World::getCurrentChunk() {
    return _currentChunk;
}

TERRAIN_TYPE World::getTerrainDataAt(Chunk* chunk, sf::Vector2f pos) {
    if (chunk == nullptr || chunk->terrainData.size() == 0) return TERRAIN_TYPE::EMPTY;

    int x = (int)pos.x - (int)chunk->pos.x;
    int y = (int)pos.y - (int)chunk->pos.y;

    if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE) {
        return chunk->terrainData[x + y * CHUNK_SIZE];
    } else return TERRAIN_TYPE::EMPTY;
}

TERRAIN_TYPE World::getTerrainDataAt(sf::Vector2f pos) {
    for (Chunk& chunk : _chunks) {
        if (chunkContains(chunk, pos)) return getTerrainDataAt(&chunk, pos);
    }
    return TERRAIN_TYPE::EMPTY;
}

TERRAIN_TYPE World::getTerrainDataAt(float x, float y) {
    return getTerrainDataAt(sf::Vector2f(x, y));
}

void World::loadSpriteSheet(std::shared_ptr<sf::Texture> spriteSheet) {
    _spriteSheet = spriteSheet;
}

std::shared_ptr<sf::Texture> World::getSpriteSheet() const {
    return _spriteSheet;
}

unsigned int World::getSeed() {
    return _seed;
}

void World::addEntity(std::shared_ptr<Entity> entity) {
    _entities.push_back(entity);
}

bool World::showDebug() const {
    return _showDebug;
}

std::shared_ptr<Player> World::getPlayer() const {
    return _player;
}

int World::getMobCount() const {
    int count = 0;
    for (auto& entity : getEntities())
        if (entity->isMob() && (!entity->isEnemy() || entity->getEntityType() == "cactoid") && entity->isActive()) count++;
    return count;
}

int World::getEnemyCount() const {
    int count = 0;
    for (auto& entity : getEntities())
        if (entity->isEnemy() && entity->getEntityType() != "cactoid" && entity->isActive()) count++;
    return count;
}

bool World::onEnemySpawnCooldown() const {
    return _cooldownActive;
}

long long World::getEnemySpawnCooldownTimeMilliseconds() const {
    return _enemySpawnCooldownTimeMilliseconds;
}

long long World::getTimeUntilNextEnemyWave() const {
    return getEnemySpawnCooldownTimeMilliseconds() - (currentTimeMillis() - _cooldownStartTime);
}

void World::resetEnemySpawnCooldown() {
    _cooldownStartTime = 0;
}

int World::getMaxActiveEnemies() const {
    return _maxActiveEnemies;
}

void World::setMaxActiveEnemies(int maxActiveEnemies) {
    _maxActiveEnemies = maxActiveEnemies;
}

void World::incrementEnemySpawnCooldownTimeWhilePaused() {
    if (onEnemySpawnCooldown()) _cooldownStartTime += 16LL;
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

void World::propDestroyedAt(sf::Vector2f pos) {
    _destroyedProps.push_back(pos);
}

bool World::isPropDestroyedAt(sf::Vector2f pos) const {
    for (auto& prop : _destroyedProps)
        if (prop.x == pos.x && prop.y == pos.y) return true;
    return false;
}

void World::reseed(const unsigned int seed) {
    _seed = seed;
    srand(_seed);
    gen.seed(_seed);
}

void World::resetChunks() {
    if (_loadingChunks.size() == 0) {
        _chunks.clear();
        _currentChunk = nullptr;
        _entityBuffer.clear();
    } else MessageManager::displayMessage("Tried to reset chunks while chunks were loading", 10, WARN);
}

void World::enterShop(sf::Vector2f shopPos) {
    _isPlayerInShop = true;
    std::shared_ptr<ShopInterior> shopInterior = std::shared_ptr<ShopInterior>(new ShopInterior(shopPos, getSpriteSheet()));
    shopInterior->setWorld(this);
    addEntity(shopInterior);

    std::shared_ptr<ShopCounter> shopCounter = std::shared_ptr<ShopCounter>(new ShopCounter(sf::Vector2f(shopPos.x, shopPos.y + 80), getSpriteSheet()));
    shopCounter->setWorld(this);
    addEntity(shopCounter);

    _shopKeep->setPosition(sf::Vector2f(shopPos.x + 32, shopPos.y + 80 - 12));
    _shopKeep->initInventory();
    _shopKeep->activate();
    addEntity(_shopKeep);

    _player->_pos.x = shopCounter->getPosition().x + 90 - 16;
    _player->_pos.y = shopCounter->getPosition().y + 46;

    MessageManager::displayMessage("Approach the shopkeep and press E to see what he's got", 6);
}

void World::exitShop() {
    _isPlayerInShop = false;
}

bool World::playerIsInShop() const {
    return _isPlayerInShop;
}

void World::setShopKeep(std::shared_ptr<ShopKeep> shopKeep) {
    _shopKeep = shopKeep;
    _shopKeep->setWorld(this);
}
