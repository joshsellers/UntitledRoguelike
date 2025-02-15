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
#include "FastNoise/FastNoise.h"
#include "../inventory/abilities/AbilityManager.h"
#include "../core/music/MusicManager.h"
#include "entities/LogMonster.h"
#include "MobSpawnConfigs.h"
#include "entities/projectiles/ProjectilePoolManager.h"
#include "../core/Viewport.h"
#include "entities/BoulderBeast.h"
#include "entities/TulipMonster.h"
#include "TerrainGenParameters.h"
#include "entities/TreeBoss.h"
#include "entities/CreamBoss.h"
#include "entities/Altar.h"
#include "entities/AltarArrow.h"
#include "entities/ShopKeepCorpse.h"
#include "entities/ChefBoss.h"
#include "../inventory/abilities/Ability.h"
#include "entities/BombBoy.h"
#include "entities/MegaBombBoy.h"
#include "entities/BabyBoss.h"
#include "entities/BigSnowMan.h"
#include "entities/TeethBoss.h"
#include "../inventory/ConditionalUnlockManager.h"
#include "entities/ShopATM.h"
#include "entities/MushroomBoss.h"
#include "../core/SoundManager.h"

World::World(std::shared_ptr<Player> player, bool& showDebug) : _showDebug(showDebug) {
    _player = player;
    _player->setWorld(this);

    _entities.push_back(_player);
}

void World::init(unsigned int seed) {
    MessageManager::displayMessage("Initializing world with seed " + std::to_string(seed), 0, DEBUG);
    _seed = seed;
    srand(_seed);
    gen.seed(_seed);

    _newGameCooldown = false;

    givePlayerDefaultAbilities();

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

        updateEntities();
        ProjectilePoolManager::update();
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
                || entity->getEntityType() == "barberchair"
                || entity->getEntityType() == "shopatm") entity->update();
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
            || (_isPlayerInShop && (entity->getEntityType() == "shopint" 
                || entity->getEntityType() == "player" 
                || entity->getEntityType() == "shopcounter" 
                || entity->getEntityType() == "shopkeep"
                || entity->getEntityType() == "barberint"
                || entity->getEntityType() == "barbercounter"
                || entity->getEntityType() == "barberchair"
                || entity->getEntityType() == "barber"
                || entity->getEntityType() == "shopatm"))) entity->draw(surface);
        
        if (_showHitBoxes && entity->isDamageable()) {
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

    if (!playerIsInShop()) {
        ProjectilePoolManager::draw(surface);
        AbilityManager::drawAbilities(_player.get(), surface);
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
    const float MOB_SPAWN_RATE_SECONDS = std::max(0.05f, (500.f - _currentWaveNumber * 10.f) / 1000.f);
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
                            case MOB_TYPE::BOMB_BOY:
                                mob = std::shared_ptr<BombBoy>(new BombBoy(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::MEGA_BOMB_BOY:
                                mob = std::shared_ptr<MegaBombBoy>(new MegaBombBoy(sf::Vector2f(xi, yi)));
                                break;
                            case MOB_TYPE::BIG_SNOW_MAN:
                                mob = std::shared_ptr<BigSnowMan>(new BigSnowMan(sf::Vector2f(xi, yi)));
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
                            if (AbilityManager::playerHasAbility(Ability::STOPWATCH.getId())) {
                                _enemySpawnCooldownTimeMilliseconds += (long long)AbilityManager::getParameter(Ability::STOPWATCH.getId(), "count") * 30000LL;
                            }

                            _maxActiveEnemies = (int)((12.f * std::log(std::pow(PLAYER_SCORE, 2)) * std::log(PLAYER_SCORE / 2) + 5) * 0.5f);
                            if (_maxActiveEnemies == 2) _maxActiveEnemies = 4;
                            if (HARD_MODE_ENABLED) _maxActiveEnemies *= 2;
                            if (_player->getMaxHitPoints() > _highestPlayerHp) _highestPlayerHp = _player->getMaxHitPoints();
                            PLAYER_SCORE += 1.f * ((_player->getDamageMultiplier()) * ((float)_highestPlayerHp / 100.f));
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
        if (!bossIsActive()) MusicManager::setSituation(MUSIC_SITUTAION::COOLDOWN);
    } else if (_cooldownActive && currentTimeMillis() - _cooldownStartTime >= _enemySpawnCooldownTimeMilliseconds) {
        _cooldownActive = false;
        if (!bossIsActive()) MusicManager::setSituation(MUSIC_SITUTAION::WAVE);
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

        checkAltarSpawn();
    }

    if (!Tutorial::isCompleted() && _waveCounter == 1) Tutorial::completeStep(TUTORIAL_STEP::CLEAR_WAVE_1);
    _currentWaveNumber++;
    if (_currentWaveNumber == 100) {
        AchievementManager::unlock(UNSTOPPABLE);
        if (HARD_MODE_ENABLED) AchievementManager::unlock(HARDMODE_UNSTOPPABLE);
    }

    int unlockedItemCount = 0;
    if (_currentWaveNumber > StatManager::getOverallStat(HIGHEST_WAVE_REACHED)) {
        StatManager::increaseStat(HIGHEST_WAVE_REACHED, _currentWaveNumber - StatManager::getOverallStat(HIGHEST_WAVE_REACHED));
        for (const auto& item : Item::ITEMS) {
            if (_currentWaveNumber == item->getRequiredWave()) unlockedItemCount++;
        }
    }
    if (unlockedItemCount > 0) {
        MessageManager::displayMessage(std::to_string(unlockedItemCount) + " new shop item" + (unlockedItemCount > 1 ? "s" : "") + " unlocked!", 8, SPECIAL);
        SoundManager::playSound("itemunlock");
        StatManager::increaseStat(ITEMS_UNLOCKED, unlockedItemCount);
    }

    if ((_currentWaveNumber + 1) % 8 == 0 && _currentWaveNumber != 95) {
        // !TODO: increase the number in the if statement below
        // as new bosses are added, remove the if statement
        // once all bosses are added
        if (_currentWaveNumber < 64) {
            MessageManager::displayMessage("Something scary will appear after you beat the next wave.\nBe prepared", 10);
        }
    }

    spawnBoss(_currentWaveNumber);
}

void World::checkAltarSpawn() {
    if (AbilityManager::getParameter(Ability::ALTAR_CHANCE.getId(), "damageThisWave") == 0.f && getCurrentWaveNumber() > 5) {
        AbilityManager::setParameter(
            Ability::ALTAR_CHANCE.getId(), "wavesWithoutDamage",
            AbilityManager::getParameter(Ability::ALTAR_CHANCE.getId(), "wavesWithoutDamage") + 1.f
        );

        constexpr float maxAltarChance = 50.f;
        const float altarChance = std::min((AbilityManager::getParameter(Ability::ALTAR_CHANCE.getId(), "wavesWithoutDamage") / 10.f) * maxAltarChance, maxAltarChance);
        if (altarChance != 0.f) {
            if (randomChance(altarChance / 100.f)) {
                sf::Vector2f spawnPos;
                const int dirFromPlayer = randomInt(0, 3);
                const int maxDistFromPlayer = WIDTH;
                const int minDistFromPlayer = WIDTH;
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

                std::shared_ptr<Altar> altar = std::shared_ptr<Altar>(new Altar(
                    spawnPos, false, getSpriteSheet()
                ));
                altar->setWorld(this);
                addEntity(altar);

                if (getTerrainDataAt(spawnPos) != TERRAIN_TYPE::EMPTY) {
                    AltarArrow::altarSpawned(spawnPos);

                    AbilityManager::setParameter(Ability::ALTAR_CHANCE.getId(), "wavesWithoutDamage", 0.f);
                } else {
                    MessageManager::displayMessage("Did not spawn altar because it would have spawned in empty terrain", 5, DEBUG);
                }
            }

            MessageManager::displayMessage("Altar chance: " + std::to_string(altarChance) + "%", 5, DEBUG);
        }
    }
    AbilityManager::setParameter(Ability::ALTAR_CHANCE.getId(), "damageThisWave", 0.f);
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

    constexpr int altarSpawnRate = 28000;
    const int shopSpawnRate = (/*HARD_MODE_ENABLED ? 19700 :*/ 12000);
    constexpr int grassSpawnRate = 25;
    constexpr int smallTreeSpawnRate = 187;
    constexpr int cactusSpawnRate = 1000;
    constexpr int smallSavannaTreeSpawnRate = 1000;
    constexpr int largeSavannaTreeSpawnRate = 1250;
    constexpr int smallTundraTreeSpawnRate = 1500;
    constexpr int fingerTreeSpawnRate = 875;
    constexpr int forestSmallTreeSpawnRate = 20;

    bool spawnedShopThisChunk = false;
    bool spawnedAltarThisChunk = false;

    srand(chX + chY * _seed);
    gen.seed(chX + chY * _seed);
    for (int yTile = chY; yTile < chY + CHUNK_SIZE; yTile += TILE_SIZE) {
        for (int xTile = chX; xTile < chX + CHUNK_SIZE; xTile += TILE_SIZE) {
            boost::random::uniform_int_distribution tileOffset(0, TILE_SIZE - 1);
            int x = xTile + tileOffset(gen);
            int y = yTile + tileOffset(gen);

            int dX = x - chX;
            int dY = y - chY;

            const TERRAIN_TYPE terrainType = chunk.terrainData[dX + dY * CHUNK_SIZE];
            if (!spawnedShopThisChunk && terrainType != TERRAIN_TYPE::WATER && terrainType != TERRAIN_TYPE::EMPTY && terrainType != TERRAIN_TYPE::SAND 
                && terrainType != TERRAIN_TYPE::MOUNTAIN_HIGH) {
                boost::random::uniform_int_distribution<> shopDist(0, shopSpawnRate);
                if (shopDist(gen) == 0 && !isPropDestroyedAt(sf::Vector2f(x, y))) {
                    const TERRAIN_TYPE shopDoorTerrain = getTerrainDataAt(&chunk, sf::Vector2f(x + 192 / 2, y + 96));
                    if (shopDoorTerrain != TERRAIN_TYPE::WATER && shopDoorTerrain != TERRAIN_TYPE::EMPTY) {
                        std::shared_ptr<ShopExterior> shop = std::shared_ptr<ShopExterior>(new ShopExterior(sf::Vector2f(x, y), _spriteSheet));
                        shop->setWorld(this);
                        _scatterBuffer.push_back(shop);
                        spawnedShopThisChunk = true;

                        if (!shopHasBeenSeenAt(sf::Vector2f(x, y))) {
                            MessageManager::displayMessage("There's a shop around here somewhere!", 5);
                            shopSeenAt(sf::Vector2f(x, y));
                        }
                    }
                }
            }

            if (!spawnedAltarThisChunk && (terrainType == TERRAIN_TYPE::MOUNTAIN_HIGH)) {
                const sf::Vector2f pos = sf::Vector2f(x, y);
                boost::random::uniform_int_distribution<> altarDist(0, altarSpawnRate * (terrainType == TERRAIN_TYPE::TUNDRA ? 18 : 1));
                if (altarDist(gen) == 0 && !isPropDestroyedAt(pos)) {
                    std::shared_ptr<Altar> altar = std::shared_ptr<Altar>(new Altar(pos, altarHasBeenActivatedAt(pos), _spriteSheet));
                    altar->setWorld(this);
                    _scatterBuffer.push_back(altar);
                    spawnedAltarThisChunk = true;

                    if (!altarHasBeenActivatedAt(pos)) {
                        AltarArrow::altarSpawned(pos);
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
    const float TERRAIN_SCALE = TerrainGenInitializer::getParameters()->terrainScale;
    const float SCALE_COEFFICIENT = std::pow(10, TERRAIN_SCALE);

    long long startTime = 0;
    long long endTime = 0;
    if (BENCHMARK_TERRAIN_AND_BIOME_GEN) startTime = currentTimeMillis();

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

    sf::Image image;
    image.create(CHUNK_SIZE, CHUNK_SIZE);

    for (int y = chY; y < chY + CHUNK_SIZE; y++) {
        for (int x = chX; x < chX + CHUNK_SIZE; x++) {
            double warpNoise = fractal->GenSingle2D(warpSize * ((float)x * sampleRate * 2), warpSize * ((float)y * sampleRate * 2), _seed);

            double warpNoise2 = fractal->GenSingle2D(warpSize * ((float)x * sampleRate * 4), warpSize * ((float)y * sampleRate * 4), _seed);

            double val = fractal->GenSingle3D((x)*sampleRate, (y)*sampleRate, warpStrength * warpNoise * (warpStrength / 2) * warpNoise2, _seed);

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

            double temperatureNoise = perlin->GenSingle2D((x + xOffset) * biomeSampleRate, (y + yOffset) * biomeSampleRate, _seed + 10);
            double precipitationNoise = perlin->GenSingle2D((x + xOffset) * biomeSampleRate, (y + yOffset) * biomeSampleRate, _seed + 40);
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

            bool tundra = temperatureNoise > tundraTemp.x && temperatureNoise < tundraTemp.y && precipitationNoise > tundraPrec.x && precipitationNoise < tundraPrec.y;
            bool desert = temperatureNoise > desertTemp.x && temperatureNoise < desertTemp.y && precipitationNoise > desertPrec.x && precipitationNoise < desertPrec.y;
            bool savanna = temperatureNoise > savannaTemp.x && temperatureNoise < savannaTemp.y && precipitationNoise > savannaPrec.x && precipitationNoise < savannaPrec.y;
            bool forest = temperatureNoise > forestTemp.x && temperatureNoise < forestTemp.y && precipitationNoise > forestPrec.x && precipitationNoise < forestPrec.y;

            if (!Tutorial::isCompleted()) {
                desert = false;
                tundra = false;
                forest = false;
            }

            // rare biomes 
            const double rareBiomeSampleRate = biomeSampleRate / 2;

            double rareBiomeTemp = perlin->GenSingle2D((x + xOffset) * rareBiomeSampleRate, (y + yOffset) * rareBiomeSampleRate, _seed + 8);
            double rareBiomePrec = perlin->GenSingle2D((x + xOffset) * rareBiomeSampleRate, (y + yOffset) * rareBiomeSampleRate, _seed + 34);
            rareBiomeTemp = norm_0_1(rareBiomeTemp, -1, 1);
            rareBiomePrec = norm_0_1(rareBiomePrec, -1, 1);

            rareBiomeTemp += ((float)randomInt(-(int)biomeEdgeMixing, (int)biomeEdgeMixing)) / 100000.;
            rareBiomePrec += ((float)randomInt(-(int)biomeEdgeMixing, (int)biomeEdgeMixing)) / 100000.;

            const sf::Vector2f fleshTemp = TerrainGenInitializer::getParameters()->fleshTemp;
            const sf::Vector2f fleshPrec = TerrainGenInitializer::getParameters()->fleshPrec;

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
        if (HARD_MODE_ENABLED && entity->getSaveId() != TREE_BOSS) {
            entity->setMaxHitPoints(entity->getMaxHitPoints() + ((float)entity->getMaxHitPoints() * 2.5f));
            entity->heal(entity->getMaxHitPoints());
        } else if (!HARD_MODE_ENABLED) {
            /*entity->heal(-entity->getMaxHitPoints());
            entity->setMaxHitPoints(entity->getMaxHitPoints() - ((float)entity->getMaxHitPoints() * 0.05f));
            entity->heal(entity->getMaxHitPoints());*/
        }

        _bossIsActive = true;
        _currentBoss = entity;
    } else if (HARD_MODE_ENABLED && entity->isMiniBoss()) {
        //entity->setMaxHitPoints(entity->getMaxHitPoints() + ((float)entity->getMaxHitPoints() * 1.15f));
        entity->heal(entity->getMaxHitPoints());
    }

    if (entity->isOrbiter() && !defer) _orbiters.push_back(entity);
}

bool World::showDebug() const {
    return _showDebug;
}

void World::toggleShowHitBoxes() {
    _showHitBoxes = !_showHitBoxes;
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
        case 8:
            boss = std::shared_ptr<TreeBoss>(new TreeBoss(spawnPos));
            break;
        case 16:
            boss = std::shared_ptr<CheeseBoss>(new CheeseBoss(spawnPos));
            break;
        case 24:
            boss = std::shared_ptr<CannonBoss>(new CannonBoss(spawnPos));
            break;
        case 32:
            boss = std::shared_ptr<CreamBoss>(new CreamBoss(spawnPos));
            break;
        case 40:
            boss = std::shared_ptr<ChefBoss>(new ChefBoss(spawnPos));
            break;
        case 48:
            boss = std::shared_ptr<BabyBoss>(new BabyBoss(spawnPos));
            break;
        case 56:
            boss = std::shared_ptr<TeethBoss>(new TeethBoss(spawnPos));
            break;
        case 64:
            boss = std::shared_ptr<MushroomBoss>(new MushroomBoss(spawnPos));
            break;
    }

    if (boss != nullptr) {
        MusicManager::setSituation(MUSIC_SITUTAION::BOSS);

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

bool World::altarHasBeenActivatedAt(sf::Vector2f pos) const {
    for (auto& altar : _activatedAltars)
        if (altar.x == pos.x && altar.y == pos.y) return true;
    return false;
}

void World::altarActivatedAt(sf::Vector2f pos) {
    _activatedAltars.push_back(pos);
    AltarArrow::altarActivated();
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
        MusicManager::setSituation(MUSIC_SITUTAION::SHOP);

        std::shared_ptr<ShopInterior> shopInterior = std::shared_ptr<ShopInterior>(new ShopInterior(buildingPos, getSpriteSheet()));
        shopInterior->setWorld(this);
        addEntity(shopInterior);

        std::shared_ptr<ShopCounter> shopCounter = std::shared_ptr<ShopCounter>(new ShopCounter(sf::Vector2f(buildingPos.x, buildingPos.y + 80), getSpriteSheet()));
        shopCounter->setWorld(this);
        addEntity(shopCounter);

        _shopKeep->setPosition(sf::Vector2f(buildingPos.x + 32, buildingPos.y + 80 - 12));
        _shopKeep->initInventory();
        if (!isShopKeepDead(_shopKeep->getPosition().x + _shopKeep->getPosition().y * (_shopKeep->getPosition().x - _shopKeep->getPosition().y))) {
            _shopKeep->activate();
            addEntity(_shopKeep);
        } else {
            sf::Vector2f corpsePos(_shopKeep->getPosition().x - 8, _shopKeep->getPosition().y + 10);
            std::shared_ptr<ShopKeepCorpse> corpse = std::shared_ptr<ShopKeepCorpse>(new ShopKeepCorpse(corpsePos, getSpriteSheet()));
            corpse->setWorld(this);
            addEntity(corpse);
        }

        float atmChance = 0.1f;
        if (AbilityManager::playerHasAbility(Ability::DEBIT_CARD.getId())) {
            atmChance += AbilityManager::getParameter(Ability::DEBIT_CARD.getId(), "chance");
        }

        const sf::Vector2f shopkeepPos = _shopKeep->getPosition();
        const unsigned int seed = shopkeepPos.x + shopkeepPos.y * (shopkeepPos.x - shopkeepPos.y);
        srand(seed);
        if (randomChance(atmChance)) {
            std::shared_ptr<ShopATM> shopAtm = std::shared_ptr<ShopATM>(new ShopATM(sf::Vector2f(buildingPos.x + 96, buildingPos.y), getSpriteSheet()));
            shopAtm->setWorld(this);
            addEntity(shopAtm);
        }

        _player->_pos.x = shopCounter->getPosition().x + 90 - 16;
        _player->_pos.y = shopCounter->getPosition().y + 46;

        if (!Tutorial::isCompleted()) {
            if (GamePad::isConnected()) {
                MessageManager::displayMessage("Approach the shopkeep and press X to see what he's got", 6);
            } else {
                MessageManager::displayMessage("Approach the shopkeep and press E to see what he's got", 6);
            }
        }
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

    if (onEnemySpawnCooldown()) _shopEntranceTimeMillis = currentTimeMillis();
}

void World::exitBuilding() {
    if (onEnemySpawnCooldown() && !bossIsActive()) MusicManager::setSituation(MUSIC_SITUTAION::COOLDOWN);
    else if (bossIsActive()) MusicManager::setSituation(MUSIC_SITUTAION::BOSS);
    else MusicManager::setSituation(MUSIC_SITUTAION::WAVE);

    _isPlayerInShop = false;

    if (onEnemySpawnCooldown()) {
        _cooldownStartTime = currentTimeMillis() - (_shopEntranceTimeMillis - _cooldownStartTime);
    }
}

bool World::playerIsInShop() const {
    return _isPlayerInShop;
}

void World::shopKeepKilled(unsigned int shopSeed) {
    _deadShopKeeps.push_back(shopSeed);
}

bool World::isShopKeepDead(unsigned int shopSeed) const {
    for (unsigned int seed : _deadShopKeeps) {
        if (seed == shopSeed) return true;
    }
    return false;
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
    if (onEnemySpawnCooldown()) MusicManager::setSituation(MUSIC_SITUTAION::COOLDOWN);
    else MusicManager::setSituation(MUSIC_SITUTAION::WAVE);

    _bossIsActive = false;
    StatManager::increaseStat(BOSSES_DEFEATED, 1.f);

    ACHIEVEMENT achievement = MILLIONAIRE;
    switch (getCurrentBoss()->getSaveId()) {
        case CHEESE_BOSS:
            achievement = DEFEAT_CHEESEBOSS;
            break;
        case CANNON_BOSS:
            achievement = DEFEAT_CANNONBOSS;
            break;
        case TREE_BOSS:
            achievement = DEFEAT_TREEBOSS;
            break;
        case CREAM_BOSS:
            achievement = DEFEAT_CREAMBOSS;
            break;
        case CHEF_BOSS:
            achievement = DEFEAT_CHEFBOSS;
            ConditionalUnlockManager::increaseUnlockProgress("Chef's Hat", 1);
            break;
        case BABY_BOSS:
            achievement = DEFEAT_BABYBOSS;
            break;
        case TEETH_BOSS:
            achievement = DEFEAT_TEETHBOSS;
            break;
        case MUSHROOM_BOSS:
            achievement = DEFEAT_SHROOMBOSS;
            break;
    }

    if (achievement != MILLIONAIRE) {
        AchievementManager::unlock(achievement);
        if (HARD_MODE_ENABLED) AchievementManager::unlock((ACHIEVEMENT)((int)achievement + 1));
    }
}

std::shared_ptr<Entity> World::getCurrentBoss() const {
    return _currentBoss;
}

void World::givePlayerDefaultAbilities() const {
    if (!AbilityManager::playerHasAbility(Ability::ALTAR_CHANCE.getId())) {
        AbilityManager::givePlayerAbility(Ability::ALTAR_CHANCE.getId());
    }

    if (!AbilityManager::playerHasAbility(Ability::CRIT_CHANCE.getId())) {
        AbilityManager::givePlayerAbility(Ability::CRIT_CHANCE.getId());
    }
}