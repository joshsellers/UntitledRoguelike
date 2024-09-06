#include "World.h"
#include <iostream>
#include <thread>
#include "entities/Penguin.h"
#include "entities/Turtle.h"
#include "entities/PlantMan.h"
#include "entities/Cactoid.h"
#include "entities/Frog.h"
#include "entities/SnowMan.h"
#include "entities/Yeti.h"
#include "entities/ShopExterior.h"
#include "entities/TallGrass.h"
#include "entities/SmallTree.h"
#include "entities/Cactus.h"
#include "entities/SmallSavannaTree.h"
#include "entities/LargeSavannaTree.h"
#include "entities/SmallTundraTree.h"
#include "entities/ShopInterior.h"
#include "entities/ShopCounter.h"
#include "../core/Util.h"
#include "entities/Skeleton.h"
#include "entities/Dog.h"
#include "../core/Tutorial.h"
#include "entities/Shark.h"
#include "../core/ShaderManager.h"
#include "entities/FingerTree.h"
#include "entities/BarberInterior.h"
#include "entities/BarberCounter.h"
#include "entities/BarberChair.h"
#include "TerrainColor.h"
#include "entities/Cyclops.h"
#include "entities/CheeseBoss.h"
#include "../statistics/StatManager.h"
#include "entities/FleshChicken.h"
#include "entities/CannonBoss.h"
#include "../statistics/AchievementManager.h"
#include "../inventory/abilities/AbilityManager.h"
#include "../core/music/MusicManager.h"
#include "entities/LogMonster.h"
#include "MobSpawnConfigs.h"
#include "entities/projectiles/ProjectilePoolManager.h"
#include "../core/Viewport.h"
#include "entities/BoulderBeast.h"
#include "entities/TulipMonster.h"

World::World(std::shared_ptr<Player> player, bool& showDebug) : _showDebug(showDebug) {
    _player = player;
    _player->setWorld(this);

    _entities.push_back(_player);
}

void World::init(unsigned int seed) {
    _seed = seed;
    srand(_seed);
    gen.seed(_seed);

    _newGameCooldown = false;

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
        MessageManager::displayMessage("loadChunksAroundPlayer was called while there were active chunks", 10, DEBUG);
    } else if (_loadingChunks.size() != 0) {
        MessageManager::displayMessage("loadChunksAroundPlayer was called while chunks were already loading", 10, DEBUG);
    }
}

void World::update() {
    if (!_isPlayerInShop) {
        dumpChunkBuffer();

        if (getActiveChunkCount() == 0 && _loadingChunks.size() == 0) loadChunksAroundPlayer();

        if (!disableMobSpawning) {
            spawnMobs();
            if (!disableEnemySpawning && (!_newGameCooldown || currentTimeMillis() - _newGameCooldownStartTime > _newGameCooldownLength)) spawnEnemies();
        }

        purgeScatterBuffer();
        purgeEntityBuffer();

        ProjectilePoolManager::update();
        updateEntities();
        AbilityManager::updateAbilities(_player.get());

        removeInactiveEntitiesFromSubgroups();

        int pX = ((int)_player->getPosition().x + PLAYER_WIDTH / 2);
        int pY = ((int)_player->getPosition().y + PLAYER_HEIGHT);

        eraseChunks(pX, pY);

        findCurrentChunk(pX, pY);

        loadNewChunks(pX, pY);

        manageCurrentWave();
    } else {
        purgeEntityBuffer();
        _player->update();
        for (auto& entity : getEntities()) {
            if (entity->getEntityType() == "shopint" 
                || entity->getEntityType() == "shopcounter" 
                || entity->getEntityType() == "shopext"
                || entity->getEntityType() == "barberint"
                || entity->getEntityType() == "barberext"
                || entity->getEntityType() == "barbercounter"
                || entity->getEntityType() == "barberchair") entity->update();
        }
    }
}

void World::draw(sf::RenderTexture& surface) {
    sortEntities();

    if (!_isPlayerInShop) {
        for (Chunk& chunk : _chunks) {
            surface.draw(chunk.sprite, ShaderManager::getShader("waves_frag"));
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
        }
    }

    sf::FloatRect cameraBounds = Viewport::getBounds();
    for (const auto& entity : _entities) {
        if (!entity->isDormant() && !_isPlayerInShop && (cameraBounds.intersects(entity->getSprite().getGlobalBounds()))
            || (_isPlayerInShop && entity->getEntityType() == "shopint" 
                || entity->getEntityType() == "player" 
                || entity->getEntityType() == "shopcounter" 
                || entity->getEntityType() == "shopkeep"
                || entity->getEntityType() == "barberint"
                || entity->getEntityType() == "barbercounter"
                || entity->getEntityType() == "barberchair"
                || entity->getEntityType() == "barber")) entity->draw(surface);
        
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

    ProjectilePoolManager::draw(surface);

    if (!playerIsInShop()) AbilityManager::drawAbilities(_player.get(), surface);
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

                    if (getMobCount() > MAX_ACTIVE_MOBS) continue;

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
                            case MOB_TYPE::CACTOID:
                                mob = std::shared_ptr<Cactoid>(new Cactoid(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::FROG:
                                mob = std::shared_ptr<Frog>(new Frog(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::DOG:
                                mob = std::shared_ptr<Dog>(new Dog(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::SHARK:
                                mob = std::shared_ptr<Shark>(new Shark(sf::Vector2f(xi, yi)));
                                break;
                            default:
                                return;
                        }

                        mob->loadSprite(_spriteSheet);
                        mob->setWorld(this);
                        addEntity(mob);
                    }
                }
            } else continue;
        }
    }
}

void World::spawnEnemies() {
    constexpr float MOB_SPAWN_RATE_SECONDS = 0.5f;
    constexpr int MOB_SPAWN_CHANCE = 5;
    constexpr float MIN_DIST = 250.f;
    constexpr int PACK_SPREAD = 20;

    if (_enemySpawnClock.getElapsedTime().asSeconds() >= MOB_SPAWN_RATE_SECONDS) {
        _enemySpawnClock.restart();
        boost::random::uniform_int_distribution<> skipChunk(0, MOB_SPAWN_CHANCE);
        for (auto& chunk : _chunks) {
            if (skipChunk(_enemyGen) == 0) {
                boost::random::uniform_int_distribution<> randX(chunk.pos.x, chunk.pos.x + CHUNK_SIZE);
                boost::random::uniform_int_distribution<> randY(chunk.pos.y, chunk.pos.y + CHUNK_SIZE);
                float x = randX(_enemyGen);
                float y = randY(_enemyGen);

                if (std::abs(x - _player->getPosition().x) > MIN_DIST || std::abs(y - _player->getPosition().y) > MIN_DIST) {
                    TERRAIN_TYPE terrainType = getTerrainDataAt(&chunk, sf::Vector2f(x, y));
                    if ((unsigned int)terrainType < (unsigned int)TERRAIN_TYPE::WATER) continue;

                    unsigned int biomeIndex = (unsigned int)terrainType - (unsigned int)TERRAIN_TYPE::WATER;
                    const BiomeMobSpawnData& biomeMobSpawnData = ENEMY_SPAWN_DATA[biomeIndex];
                    if (biomeMobSpawnData.mobData.size() == 0) continue;

                    const MobSpawnData& mobData = biomeMobSpawnData.mobData.at(getRandMobType(biomeMobSpawnData));
                    boost::random::uniform_int_distribution<> individualSpawnChance(0, mobData.spawnChance);
                    if (individualSpawnChance(_enemyGen) != 0) continue;

                    if (_cooldownActive || _maxEnemiesReached) continue;

                    boost::random::uniform_int_distribution<> randPackAmount(mobData.minPackSize, mobData.maxPackSize);
                    int packAmount = randPackAmount(_enemyGen) + (1 * (PLAYER_SCORE / 100.f));

                    boost::random::uniform_int_distribution<> randXi(x - PACK_SPREAD, x + PACK_SPREAD);
                    boost::random::uniform_int_distribution<> randYi(y - PACK_SPREAD, y + PACK_SPREAD);
                    for (int i = 0; i < packAmount; i++) {
                        int xi = randXi(_enemyGen);
                        int yi = randYi(_enemyGen);
                        std::shared_ptr<Entity> mob = nullptr;

                        switch (mobData.mobType) {
                            case MOB_TYPE::PLANT_MAN:
                                mob = std::shared_ptr<PlantMan>(new PlantMan(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::SNOW_MAN:
                                mob = std::shared_ptr<SnowMan>(new SnowMan(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::YETI:
                                mob = std::shared_ptr<Yeti>(new Yeti(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::SKELETON:
                                mob = std::shared_ptr<Skeleton>(new Skeleton(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::CYCLOPS:
                                mob = std::shared_ptr<Cyclops>(new Cyclops(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::FLESH_CHICKEN:
                                mob = std::shared_ptr<FleshChicken>(new FleshChicken(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::CHEESE_BOSS:
                            {
                                mob = std::shared_ptr<CheeseBoss>(new CheeseBoss(sf::Vector2f(xi, yi)));
                                Boss* boss = dynamic_cast<Boss*>(mob.get());
                                boss->deactivateBossMode();
                                break;
                            }
                            case MOB_TYPE::LOG_MONSTER:
                                mob = std::shared_ptr<LogMonster>(new LogMonster(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::BOULDER_BEAST:
                                mob = std::shared_ptr<BoulderBeast>(new BoulderBeast(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::TULIP_MONSTER:
                                mob = std::shared_ptr<TulipMonster>(new TulipMonster(sf::Vector2f(xi, yi)));
                                break;
                            default:
                                return;
                        }

                        mob->loadSprite(_spriteSheet);
                        mob->setWorld(this);
                        addEntity(mob);

                        if ((getEnemyCount() >= _maxActiveEnemies || _enemiesSpawnedThisRound >= _maxActiveEnemies)) {
                            _maxEnemiesReached = true;
                            _enemySpawnCooldownTimeMilliseconds = randomInt(MIN_ENEMY_SPAWN_COOLDOWN_TIME_MILLISECONDS, MAX_ENEMY_SPAWN_COOLDOWN_TIME_MILLISECONDS);
                            _maxActiveEnemies = (int)((12.f * std::log(std::pow(PLAYER_SCORE, 2)) * std::log(PLAYER_SCORE / 2) + 5) * 0.5f);
                            if (_maxActiveEnemies == 2) _maxActiveEnemies = 4;
                            PLAYER_SCORE += 1.f * ((_player->getDamageMultiplier()) * ((float)_player->getMaxHitPoints() / 100.f));
                            _waveCounter++;
                            break;
                        } else _enemiesSpawnedThisRound++;
                    }
                }
            } else continue;
        }
    }
}

int World::getRandMobType(const BiomeMobSpawnData& mobSpawnData) {
    std::vector<int> availableMobTypes;
    for (int i = 0; i < mobSpawnData.mobData.size(); i++) {
        if (mobSpawnData.mobData.at(i).waveNumber <= getCurrentWaveNumber()) {
            availableMobTypes.push_back(i);
        }
    }
    boost::random::uniform_int_distribution<> randMobType(0, availableMobTypes.size() - 1);
    return availableMobTypes[randMobType(_mobGen)];
}

void World::purgeScatterBuffer() {
    if (!_loadingScatters && _scatterBuffer.size() != 0 && _loadingChunks.size() == 0) {
        for (auto& entity : _scatterBuffer) {
            _entities.push_back(entity);
        }
        _scatterBuffer.clear();
    }
}

void World::purgeEntityBuffer() {
    if (_entityBuffer.size() != 0) {
        for (auto& entity : _entityBuffer) {
            if (entity->isOrbiter()) _orbiters.push_back(entity);
            _entities.push_back(entity);
        }
        _entityBuffer.clear();
    }
}

void World::updateEntities() {
    /*for (int i = 0; i < _entities.size(); i++) {
        auto& entity = _entities.at(i);
        if (!entity->isActive()) _entities.erase(_entities.begin() + i);
    }*/
    _entities.erase(std::remove_if(_entities.begin(), _entities.end(), [](std::shared_ptr<Entity> entity) {return !entity->isActive(); }), _entities.end());

    bool foundPlayer = false;

    for (const auto& entity : _entities) {
        if (entity->isOrbiter()) continue;

        if (entity == nullptr) {
            MessageManager::displayMessage("An entity was nullptr", 0, DEBUG);
            continue;
        }

        if (!Tutorial::isCompleted() && entity->getEntityType() == "player") foundPlayer = true;

        if (!entity->isProp() && entity->isActive() && !entity->usesDormancyRules()) {
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
        for (const auto& chunk : _chunks) {
            if (!chunkContains(chunk, entity->getPosition())) {
                notInChunkCount++;
            }
        }

        if (notInChunkCount == _chunks.size() && entity->isProp()) entity->deactivate();
        else if (notInChunkCount == _chunks.size() && entity->usesDormancyRules() && !entity->isDormant() && (!entity->isEnemy() || entity->isInitiallyDocile())) {
            if (entity->getTimeOutOfChunk() >= entity->getMaxTimeOutOfChunk()) {
                entity->setDormant(true);
                continue;
            }

            entity->incrementOutOfChunkTimer();
            entity->update();
        } else if (notInChunkCount < _chunks.size() && entity->usesDormancyRules() && entity->isDormant()) {
            entity->setDormant(false);
            entity->resetDormancyTimer();
        } else if (notInChunkCount < _chunks.size() && entity->usesDormancyRules()) {
            entity->resetOutOfChunkTimer();
            entity->update();
        }
        else if (!entity->isDormant()) entity->update();
    }

    if (!foundPlayer && !Tutorial::isCompleted()) {
        _player->activate();
        addEntity(_player);
    }

    for (const auto& orbiter : _orbiters) {
        if (orbiter->isActive()) orbiter->update();
    }
}

void World::removeInactiveEntitiesFromSubgroups() {
    if (currentTimeMillis() - _lastEntityRemovalTime >= INACTIVE_ENEMY_REMOVAL_INTERVAL && !_enemies.empty()) {
        for (int i = 0; i < _enemies.size(); i++) {
            auto& enemy = _enemies.at(i);
            if (!enemy->isActive()) _enemies.erase(_enemies.begin() + i);
        }

        for (int i = 0; i < _collectorMobs.size(); i++) {
            auto& entity = _collectorMobs.at(i);
            if (!entity->isActive()) _collectorMobs.erase(_collectorMobs.begin() + i);
        }

        for (int i = 0; i < _orbiters.size(); i++) {
            auto& orbiter = _orbiters.at(i);
            if (!orbiter->isActive()) _orbiters.erase(_orbiters.begin() + i);
        }
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
        MessageManager::displayMessage("currentChunk was nullptr", 0, DEBUG);
    }
}

void World::dumpChunkBuffer() {
    while (!_chunkBuffer.empty()) {
        _chunks.push_back(_chunkBuffer.front());
        _chunkBuffer.pop();
    }
}

void World::manageCurrentWave() {
    if (_maxEnemiesReached && !_cooldownActive && getEnemyCount() == 0) {
        onWaveCleared();
        MusicManager::setSituation(MUSIC_SITUTAION::COOLDOWN);
    } else if (_cooldownActive && currentTimeMillis() - _cooldownStartTime >= _enemySpawnCooldownTimeMilliseconds) {
        _cooldownActive = false;
        MusicManager::setSituation(MUSIC_SITUTAION::WAVE);
    } else if (bossIsActive()) incrementEnemySpawnCooldownTimeWhilePaused();
}

void World::onWaveCleared() {
    _cooldownActive = true;
    _maxEnemiesReached = false;
    _enemiesSpawnedThisRound = 0;
    _cooldownStartTime = currentTimeMillis();

    if (_waveCounter != 0) {
        MessageManager::displayMessage("Wave " + std::to_string(_waveCounter) + " cleared", 5);
        StatManager::increaseStat(WAVES_CLEARED, 1.f);

        if (getPlayer()->getHitPoints() < 10) AchievementManager::unlock(SURVIVOR);
    }

    if (!Tutorial::isCompleted() && _waveCounter == 1) Tutorial::completeStep(TUTORIAL_STEP::CLEAR_WAVE_1);
    _currentWaveNumber++;
    if (_currentWaveNumber == 100) AchievementManager::unlock(UNSTOPPABLE);

    int unlockedItemCount = 0;
    for (const auto& item : Item::ITEMS) {
        if (_currentWaveNumber == item->getRequiredWave()) unlockedItemCount++;
    }
    if (unlockedItemCount > 0) {
        MessageManager::displayMessage(std::to_string(unlockedItemCount) + " new shop item" + (unlockedItemCount > 1 ? "s" : "") + " unlocked", 8);
    }

    spawnBoss(_currentWaveNumber);
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
    _chunkBuffer.push(chunk);

    _mutex.unlock();

    if (!disablePropGeneration) {
        std::thread scatterSpawnThread(&World::generateChunkScatters, this, chunk);
        scatterSpawnThread.detach();
    }

    for (int i = 0; i < _loadingChunks.size(); i++) {
        sf::Vector2i loadingChunk = _loadingChunks.at(i);
        if (loadingChunk.x == (int)pos.x && loadingChunk.y == (int)pos.y) {
            _loadingChunks.erase(_loadingChunks.begin() + i);
            break;
        }
    }
}

bool World::chunkContains(const Chunk& chunk, sf::Vector2f pos) const {
    int pX = (int)pos.x;
    int pY = (int)pos.y;
    int chX = (int)chunk.pos.x;
    int chY = (int)chunk.pos.y;
    return pX >= chX && pY >= chY && pX < chX + CHUNK_SIZE && pY < chY + CHUNK_SIZE;
}

void World::generateChunkScatters(Chunk& chunk) {
    _loadingScatters = true;

    int chX = chunk.pos.x;
    int chY = chunk.pos.y;

    const int shopSpawnRate = 5000000;
    const int grassSpawnRate = 5000;
    const int smallTreeSpawnRate = 37500;
    const int cactusSpawnRate = 200000;
    const int smallSavannaTreeSpawnRate = 200000;
    const int largeSavannaTreeSpawnRate = 250000;
    const int smallTundraTreeSpawnRate = 300000;
    const int fingerTreeSpawnRate = 175000;
    const int forestSmallTreeSpawnRate = 4000;

    srand(chX + chY * _seed);
    gen.seed(chX + chY * _seed);
    for (int y = chY; y < chY + CHUNK_SIZE; y++) {
        for (int x = chX; x < chX + CHUNK_SIZE; x++) {
            int dX = x - chX;
            int dY = y - chY;

            TERRAIN_TYPE terrainType = chunk.terrainData[dX + dY * CHUNK_SIZE];
            if (terrainType != TERRAIN_TYPE::WATER && terrainType != TERRAIN_TYPE::EMPTY && terrainType != TERRAIN_TYPE::SAND) {
                boost::random::uniform_int_distribution<> shopDist(0, shopSpawnRate);
                if (shopDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<ShopExterior> shop = std::shared_ptr<ShopExterior>(new ShopExterior(sf::Vector2f(x, y), _spriteSheet));
                    shop->setWorld(this);
                    _scatterBuffer.push_back(shop);

                    if (!shopHasBeenSeenAt(sf::Vector2f(x, y))) {
                        MessageManager::displayMessage("There's a shop around here somewhere!", 5);
                        shopSeenAt(sf::Vector2f(x, y));
                    }
                }
            }

            if (terrainType == TERRAIN_TYPE::GRASS) {
                boost::random::uniform_int_distribution<> grassDist(0, grassSpawnRate);
                boost::random::uniform_int_distribution<> treeDist(0, smallTreeSpawnRate);
                if (grassDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<TallGrass> grass = std::shared_ptr<TallGrass>(new TallGrass(sf::Vector2f(x, y), _spriteSheet));
                    _scatterBuffer.push_back(grass);
                }

                if (treeDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<SmallTree> tree = std::shared_ptr<SmallTree>(new SmallTree(sf::Vector2f(x, y), _spriteSheet));
                    tree->setWorld(this);
                    _scatterBuffer.push_back(tree);
                }
            } else if (terrainType == TERRAIN_TYPE::DESERT) {
                boost::random::uniform_int_distribution<> cactusDist(0, cactusSpawnRate);
                if (cactusDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<Cactus> cactus = std::shared_ptr<Cactus>(new Cactus(sf::Vector2f(x, y), _spriteSheet));
                    cactus->setWorld(this);
                    _scatterBuffer.push_back(cactus);
                }
            } else if (terrainType == TERRAIN_TYPE::SAVANNA) {
                boost::random::uniform_int_distribution<> smallTreeDist(0, smallSavannaTreeSpawnRate);
                boost::random::uniform_int_distribution<> largeTreeDist(0, largeSavannaTreeSpawnRate);
                if (smallTreeDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<SmallSavannaTree> tree = std::shared_ptr<SmallSavannaTree>(new SmallSavannaTree(sf::Vector2f(x, y), _spriteSheet));
                    tree->setWorld(this);
                    _scatterBuffer.push_back(tree);
                }

                if (largeTreeDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<LargeSavannaTree> tree = std::shared_ptr<LargeSavannaTree>(new LargeSavannaTree(sf::Vector2f(x, y), _spriteSheet));
                    tree->setWorld(this);
                    _scatterBuffer.push_back(tree);
                }
            } else if (terrainType == TERRAIN_TYPE::TUNDRA) {
                boost::random::uniform_int_distribution<> smallTreeDist(0, smallTundraTreeSpawnRate);
                if (smallTreeDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<SmallTundraTree> tree = std::shared_ptr<SmallTundraTree>(new SmallTundraTree(sf::Vector2f(x, y), _spriteSheet));
                    tree->setWorld(this);
                    _scatterBuffer.push_back(tree);
                }
            } else if (terrainType == TERRAIN_TYPE::FLESH) {
                boost::random::uniform_int_distribution<> fingerTreeDist(0, fingerTreeSpawnRate);
                if (fingerTreeDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<FingerTree> tree = std::shared_ptr<FingerTree>(new FingerTree(sf::Vector2f(x, y), _spriteSheet));
                    tree->setWorld(this);
                    _scatterBuffer.push_back(tree);
                }
            } else if (terrainType == TERRAIN_TYPE::GRASS_FOREST) {
                boost::random::uniform_int_distribution<> smallTreeDist(0, forestSmallTreeSpawnRate);
                if (smallTreeDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    std::shared_ptr<SmallTree> tree = std::shared_ptr<SmallTree>(new SmallTree(sf::Vector2f(x, y), _spriteSheet));
                    tree->setWorld(this);
                    _scatterBuffer.push_back(tree);
                }
            }
        }
    }

    _loadingScatters = false;
}

sf::Image World::generateChunkTerrain(Chunk& chunk) {
    constexpr float TERRAIN_SCALE = 0;
    const float SCALE_COEFFICIENT = std::pow(10, TERRAIN_SCALE);

    long long startTime = 0;
    long long endTime = 0;
    if (BENCHMARK_TERRAIN_AND_BIOME_GEN) startTime = currentTimeMillis();

    sf::Vector2f pos = chunk.pos;

    // Generator properties
    int octaves = 4;
    double warpSize = 4;
    double warpStrength = 0.8;
    double sampleRate = 0.0001 * SCALE_COEFFICIENT; // 0.0001

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
    image.create(CHUNK_SIZE, CHUNK_SIZE);

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
            double xOffset = 20000. / SCALE_COEFFICIENT; //20000.
            double yOffset = 20000. / SCALE_COEFFICIENT;
            int biomeOctaves = 2;
            double biomeSampleRate = 0.0000135 * SCALE_COEFFICIENT; // 0.00001;
            double temperatureNoise = perlin.normalizedOctave3D_01((x + xOffset) * biomeSampleRate, (y + yOffset) * biomeSampleRate, 10, biomeOctaves);
            double precipitationNoise = perlin.normalizedOctave3D_01((x + xOffset) * biomeSampleRate, (y + yOffset) * biomeSampleRate, 40, biomeOctaves);

            constexpr float biomeEdgeMixing = 125.f;
            temperatureNoise += ((float)randomInt(-(int)biomeEdgeMixing, (int)biomeEdgeMixing)) / 100000.;
            precipitationNoise += ((float)randomInt(-(int)biomeEdgeMixing, (int)biomeEdgeMixing)) / 100000.;

            sf::Vector2f tundraTemp(0.0, 0.4);
            sf::Vector2f tundraPrec(0.1, 0.9);
            
            sf::Vector2f desertTemp(0.5, 0.6);
            sf::Vector2f desertPrec(0.0, 0.5);

            sf::Vector2f savannaTemp(0.5, 0.6);
            sf::Vector2f savannaPrec(0.5, 0.7);

            sf::Vector2f forestTemp(0.3, 0.6);
            sf::Vector2f forestPrec(0.5, 0.9);

            bool tundra = temperatureNoise > tundraTemp.x && temperatureNoise < tundraTemp.y && precipitationNoise > tundraPrec.x && precipitationNoise < tundraPrec.y;
            bool desert = temperatureNoise > desertTemp.x && temperatureNoise < desertTemp.y && precipitationNoise > desertPrec.x && precipitationNoise < desertPrec.y;
            bool savanna = temperatureNoise > savannaTemp.x && temperatureNoise < savannaTemp.y && precipitationNoise > savannaPrec.x && precipitationNoise < savannaPrec.y;
            bool forest = temperatureNoise > forestTemp.x && temperatureNoise < forestTemp.y && precipitationNoise > forestPrec.x && precipitationNoise < forestPrec.y;

            if (!Tutorial::isCompleted()) {
                desert = false;
                tundra = false;
            }

            // rare biomes 
            double rareBiomeSampleRate = biomeSampleRate / 2;
            double rareBiomeTemp = perlin.noise3D_01((x + xOffset) * rareBiomeSampleRate, (y + yOffset) * rareBiomeSampleRate, 8);
            double rareBiomePrec = perlin.noise3D_01((x + xOffset) * rareBiomeSampleRate, (y + yOffset) * rareBiomeSampleRate, 34);
            rareBiomeTemp += ((float)randomInt(-(int)biomeEdgeMixing, (int)biomeEdgeMixing)) / 100000.;
            rareBiomePrec += ((float)randomInt(-(int)biomeEdgeMixing, (int)biomeEdgeMixing)) / 100000.;

            sf::Vector2f fleshTemp(0.0005, 0.3);
            sf::Vector2f fleshPrec(0.04, 0.7);

            bool flesh = rareBiomeTemp > fleshTemp.x && rareBiomeTemp < fleshTemp.y && rareBiomePrec > fleshPrec.x && rareBiomePrec < fleshPrec.y;
            if (_seed == 124959026) flesh = true;
            if (flesh && _seed != 124959026) {
                AchievementManager::unlock(FLESHY);
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

    if (BENCHMARK_TERRAIN_AND_BIOME_GEN) {
        endTime = currentTimeMillis();
        MessageManager::displayMessage("Terrain for chunk at (" + std::to_string(chunk.pos.x) + ", " + std::to_string(chunk.pos.y)
            + ") generated in " + std::to_string(endTime - startTime) + "ms", 2, DEBUG);
    }

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

void World::addEntity(std::shared_ptr<Entity> entity, bool defer) {
    if (entity->getSaveId() == PROJECTILE) {
        MessageManager::displayMessage("Tried to spawn projectile via addEntity", 5, DEBUG);
        return;
    }

    if (defer) _entityBuffer.push_back(entity);
    else _entities.push_back(entity);

    if (entity->isEnemy()) {
        /*float playerDamageMultiplier = _player->getDamageMultiplier();
        entity->setMaxHitPoints(entity->getMaxHitPoints() + (entity->getMaxHitPoints() * (playerDamageMultiplier / 2)) + (((float)getCurrentWaveNumber() / 4)));
        entity->heal(entity->getMaxHitPoints());*/

        _enemies.push_back(entity);
    }

    if (entity->isMob() && (!entity->isEnemy() || entity->isInitiallyDocile())) entity->shouldUseDormancyRules(true);

    if (entity->canPickUpItems()) _collectorMobs.push_back(entity);

    if (entity->isBoss()) {
        _bossIsActive = true;
        _currentBoss = entity;
    }

    if (entity->isOrbiter() && !defer) _orbiters.push_back(entity);
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
    for (auto& entity : getEnemies())
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

unsigned int World::getCurrentWaveNumber() const {
    return _currentWaveNumber;
}

std::vector<std::shared_ptr<Entity>> World::getEntities() const {
    return _entities;
}

std::vector<std::shared_ptr<Entity>> World::getEnemies() const {
    return _enemies;
}

std::vector<std::shared_ptr<Entity>> World::getCollectorMobs() const {
    return _collectorMobs;
}

void World::sortEntities() {
    /*int n = _entities.size();

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
    }*/

    std::sort(_entities.begin(), _entities.end(), 
        [](std::shared_ptr<Entity> e0, std::shared_ptr<Entity> e1) {
            return e0->getPosition().y + e0->getSprite().getGlobalBounds().height < e1->getPosition().y + e1->getSprite().getGlobalBounds().height
            || (e1->displayOnTop() && !e0->displayOnTop());
        });
}

void World::spawnBoss(int currentWaveNumber) {
    sf::Vector2f spawnPos;
    const int dirFromPlayer = randomInt(0, 3);
    constexpr int maxDistFromPlayer = CHUNK_SIZE;
    constexpr int minDistFromPlayer = CHUNK_SIZE / 2;
    if (dirFromPlayer == 0) {
        spawnPos.x = getPlayer()->getPosition().x + randomInt(-maxDistFromPlayer, maxDistFromPlayer);
        spawnPos.y = getPlayer()->getPosition().y - randomInt(minDistFromPlayer, maxDistFromPlayer);
    } else if (dirFromPlayer == 1) {
        spawnPos.x = getPlayer()->getPosition().x + randomInt(-maxDistFromPlayer, maxDistFromPlayer);
        spawnPos.y = getPlayer()->getPosition().y + randomInt(minDistFromPlayer, maxDistFromPlayer);
    } else if (dirFromPlayer == 2) {
        spawnPos.x = getPlayer()->getPosition().x - randomInt(minDistFromPlayer, maxDistFromPlayer);
        spawnPos.y = getPlayer()->getPosition().y + randomInt(-maxDistFromPlayer, maxDistFromPlayer);
    } else if (dirFromPlayer == 3) {
        spawnPos.x = getPlayer()->getPosition().x + randomInt(minDistFromPlayer, maxDistFromPlayer);
        spawnPos.y = getPlayer()->getPosition().y + randomInt(-maxDistFromPlayer, maxDistFromPlayer);
    }

    std::shared_ptr<Entity> boss = nullptr;
    switch (currentWaveNumber) {
        case 16:
            boss = std::shared_ptr<CheeseBoss>(new CheeseBoss(spawnPos));
            break;
        case 32:
            boss = std::shared_ptr<CannonBoss>(new CannonBoss(spawnPos));
            break;
    }

    if (boss != nullptr) {
        boss->loadSprite(getSpriteSheet());
        boss->setWorld(this);
        addEntity(boss);
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

bool World::shopHasBeenSeenAt(sf::Vector2f pos) const {
    for (auto& shop : _seenShops)
        if (shop.x == pos.x && shop.y == pos.y) return true;
    return false;
}

void World::shopSeenAt(sf::Vector2f pos) {
    _seenShops.push_back(pos);
}

void World::reseed(const unsigned int seed) {
    _seed = seed;
    srand(_seed);
    gen.seed(_seed);
}

void World::resetChunks() {
    if (_loadingChunks.size() == 0) {
        while (!_chunkBuffer.empty()) _chunkBuffer.pop();

        _chunks.clear();
        _currentChunk = nullptr;
        _scatterBuffer.clear();
        _entityBuffer.clear();
    } else MessageManager::displayMessage("Tried to reset chunks while chunks were loading", 10, DEBUG);
}

void World::enterBuilding(std::string buildingID, sf::Vector2f buildingPos) {
    _isPlayerInShop = true;
    if (buildingID == "shop") {
        std::shared_ptr<ShopInterior> shopInterior = std::shared_ptr<ShopInterior>(new ShopInterior(buildingPos, getSpriteSheet()));
        shopInterior->setWorld(this);
        addEntity(shopInterior);

        std::shared_ptr<ShopCounter> shopCounter = std::shared_ptr<ShopCounter>(new ShopCounter(sf::Vector2f(buildingPos.x, buildingPos.y + 80), getSpriteSheet()));
        shopCounter->setWorld(this);
        addEntity(shopCounter);

        _shopKeep->setPosition(sf::Vector2f(buildingPos.x + 32, buildingPos.y + 80 - 12));
        _shopKeep->initInventory();
        _shopKeep->activate();
        addEntity(_shopKeep);

        _player->_pos.x = shopCounter->getPosition().x + 90 - 16;
        _player->_pos.y = shopCounter->getPosition().y + 46;

        MessageManager::displayMessage("Approach the shopkeep and press E to see what he's got", 6);
    } else if (buildingID == "barber") {
        std::shared_ptr<BarberInterior> barberInterior = std::shared_ptr<BarberInterior>(new BarberInterior(buildingPos, getSpriteSheet()));
        barberInterior->setWorld(this);
        addEntity(barberInterior);
        _player->_pos = barberInterior->getEntrancePos();

        std::shared_ptr<BarberCounter> barberCounter = std::shared_ptr<BarberCounter>(new BarberCounter(sf::Vector2f(buildingPos.x, buildingPos.y + 32), getSpriteSheet()));
        barberCounter->setWorld(this);
        addEntity(barberCounter);

        std::shared_ptr<BarberChair> barberChair = std::shared_ptr<BarberChair>(new BarberChair(sf::Vector2f(buildingPos.x + 96, buildingPos.y + 32), getSpriteSheet()));
        barberChair->setWorld(this);
        addEntity(barberChair);
    }
}

void World::exitBuilding() {
    _isPlayerInShop = false;
}

bool World::playerIsInShop() const {
    return _isPlayerInShop;
}

void World::setShopKeep(std::shared_ptr<ShopKeep> shopKeep) {
    _shopKeep = shopKeep;
    _shopKeep->setWorld(this);
}

void World::startNewGameCooldown() {
    _newGameCooldown = true;
    _newGameCooldownStartTime = currentTimeMillis();
}

void World::setDisplayedWaveNumber(int waveNumber) {
    _currentWaveNumber = waveNumber;
}

bool World::bossIsActive() const {
    return _bossIsActive;
}

void World::bossDefeated() {
    _bossIsActive = false;
    StatManager::increaseStat(BOSSES_DEFEATED, 1.f);

    switch (getCurrentBoss()->getSaveId()) {
        case CHEESE_BOSS:
            AchievementManager::unlock(DEFEAT_CHEESEBOSS);
            break;
        case CANNON_BOSS:
            AchievementManager::unlock(DEFEAT_CANNONBOSS);
            break;
    }
}

std::shared_ptr<Entity> World::getCurrentBoss() const {
    return _currentBoss;
}
