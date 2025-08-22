#include "DevBoss.h"
#include "../World.h"
#include "projectiles/Projectile.h"
#include "../../core/Viewport.h"
#include "PlantMan.h"
#include "SnowMan.h"
#include "Yeti.h"
#include "Skeleton.h"
#include "Cyclops.h"
#include "FleshChicken.h"
#include "LogMonster.h"
#include "BoulderBeast.h"
#include "TulipMonster.h"
#include "BurgerBeast.h"
#include "BombBoy.h"
#include "MegaBombBoy.h"
#include "Soldier.h"
#include "BigSnowMan.h"
#include "FungusMan.h"
#include "Funguy.h"
#include "../../core/FinalBossEffectManager.h"
#include "../../inventory/abilities/AbilityManager.h"
#include "../../inventory/abilities/Ability.h"
#include "../../core/EndGameSequence.h"
#include "projectiles/ProjectilePoolManager.h"
#include "../../core/SoundManager.h"

DevBoss::DevBoss(sf::Vector2f pos) : Boss(DEV_BOSS, pos, 1.f, 2 * TILE_SIZE, 3 * TILE_SIZE,
    {
        BossState(BEHAVIOR_STATE::DUMMY_STATE, 10LL, 10LL),
        BossState(BEHAVIOR_STATE::RUN_COMMAND, 5000LL, 5000LL),
        BossState(BEHAVIOR_STATE::FIRE_PROJECILE, 5000LL, 5000LL)
    }) {
    setMaxHitPoints(179000);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 2) / 2 + 5;
    _hitBoxYOffset = 5;
    _hitBox.width = 22;
    _hitBox.height = 42;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;

    _entityType = "devboss";
    _displayName = "The Developer";

    FinalBossEffectManager::devBossIsActive = true;
}

void DevBoss::subUpdate() {
    if (!_defeated) {
        if (_animationState == WALKING) {
            sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
            sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 3);

            float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
            constexpr float desiredDist = 100.f;
            float distanceRatio = desiredDist / dist;

            float xa = 0.f, ya = 0.f;

            sf::Vector2f goalPos((1.f - distanceRatio) * playerPos.x + distanceRatio * cLoc.x, (1.f - distanceRatio) * playerPos.y + distanceRatio * cLoc.y);
            if (goalPos.y < cLoc.y) {
                ya--;
                _movingDir = UP;
            } else if (goalPos.y > cLoc.y) {
                ya++;
                _movingDir = DOWN;
            }

            if (goalPos.x < cLoc.x) {
                xa--;
                _movingDir = LEFT;
            } else if (goalPos.x > cLoc.x) {
                xa++;
                _movingDir = RIGHT;
            }

            if (xa && ya) {
                xa *= 0.785398;
                ya *= 0.785398;

                if (std::abs(xa) > std::abs(ya)) {
                    if (xa > 0) _movingDir = RIGHT;
                    else _movingDir = LEFT;
                } else {
                    if (ya > 0) _movingDir = DOWN;
                    else _movingDir = UP;
                }
            }

            if (dist < desiredDist) {
                xa = 0;
                ya = 0;
            }

            move(xa, ya);

            if (!isMoving()) _movingDir = DOWN;

            _sprite.setPosition(getPosition());
            _headSprite.setPosition(getPosition().x, getPosition().y + 3);
            _bodySprite.setPosition(getPosition().x, getPosition().y + TILE_SIZE * 2);

            _hitBox.left = getPosition().x + _hitBoxXOffset;
            _hitBox.top = getPosition().y + _hitBoxYOffset;

            blink();
        } else if (_animationState == TYPING) {
            _cmdLine.update(getPosition());
            _numSteps++;
        }

        constexpr long long phaseTransitionLengthMillis = 3000LL;
        if (!_beganPhaseTwoCommand && _permitStateChange && !_phaseTransitionStarted && !_secondPhaseStared && getHitPoints() <= getMaxHitPoints() / 2 && !_ranCommand) {
            _beganPhaseTwoCommand = true;
            _permitStateChange = false;

            _currentCommand = START_PHASE_TWO;
            const BossState newState = BossState(RUN_COMMAND, 5000LL, 5000LL);
            onStateChange(_currentState, newState);
            _currentState = newState;
        } else if (_beganPhaseTwoCommand && !_phaseTransitionStarted && !_secondPhaseStared && _ranCommand) {
            _phaseTransitionStarted = true;
            _phaseTransitionStartTime = currentTimeMillis();
            _animationState = HANDS_UP;

            getWorld()->disablePropGeneration = true;
            getWorld()->resetChunks();
            FinalBossEffectManager::activateEffect(FINAL_BOSS_EFFECT::MONOCHROME_TERRAIN, 10LL * 60000LL);

        } else if (_phaseTransitionStarted && currentTimeMillis() - _phaseTransitionStartTime < phaseTransitionLengthMillis) {
            constexpr float scaleDelta = 1.f / (((float)phaseTransitionLengthMillis / 1000.f) * 60.f);
            _headSprite.setScale(_headSprite.getScale().x + scaleDelta, _headSprite.getScale().y + scaleDelta);
        } else if (_phaseTransitionStarted && currentTimeMillis() - _phaseTransitionStartTime >= phaseTransitionLengthMillis) {
            _headSprite.setScale(2.f, 2.f);

            _animationState = WALKING;
            _phaseTransitionStarted = false;
            _secondPhaseStared = true;
            _permitStateChange = true;

            _hitBoxXOffset = -(TILE_SIZE * 2) + 10;
            _hitBoxYOffset = 10;
            _hitBox.width = 42;
            _hitBox.height = 58;

            _currentCommand = NONE;

            _bossStates = {
                BossState(RADIAL_PROJECTILES, 4000LL, 6000LL),
                BossState(ROTATING_LASERS, 6000LL, 8000LL),
                BossState(BOMBS, 2000LL, 5000LL)
            };
        }
    } else if (!_deathAnimationComplete) {
        constexpr int ticksPerFrame = 10;
        constexpr int frameCount = 14;
        const int yFrame = ((_numSteps/ ticksPerFrame) % frameCount);
        _headSprite.setTextureRect({
            102 << SPRITE_SHEET_SHIFT,
            (80 + yFrame * 2) << SPRITE_SHEET_SHIFT,
            TILE_SIZE * 2,
            TILE_SIZE * 2
        });

        _numSteps++;

        if (yFrame == frameCount - 1) {
            _deathCompletionTimeMillis = currentTimeMillis();
            _deathAnimationComplete = true;
        }
    } else if (_deathAnimationComplete && currentTimeMillis() - _deathCompletionTimeMillis >= 1000LL) {
        deactivate();
        getWorld()->bossDefeated();
        getWorld()->disablePropGeneration = false;
        EndGameSequence::start();
    }
}

void DevBoss::draw(sf::RenderTexture& surface) {
    if (!_secondPhaseStared && !_defeated) {
        if (_animationState == WALKING) {
            const int yFrame = isMoving() ? (_numSteps >> _animSpeed & 3) : 0;
            _headSprite.setTextureRect({
                (98 + (_isBlinking ? 2 : 0)) << SPRITE_SHEET_SHIFT,
                (65 + yFrame * 2) << SPRITE_SHEET_SHIFT,
                TILE_SIZE * 2,
                TILE_SIZE * 2
                });

            _bodySprite.setTextureRect({
                (98 + (int)_movingDir) << SPRITE_SHEET_SHIFT,
                (73 + yFrame) << SPRITE_SHEET_SHIFT,
                TILE_SIZE,
                TILE_SIZE
                });

            surface.draw(_bodySprite);
            surface.draw(_headSprite);
        } else if (_animationState == TYPING) {
            const int yFrame = _numSteps >> _animSpeed & 3;
            _sprite.setTextureRect({
                102 << SPRITE_SHEET_SHIFT,
                (68 + yFrame * 3) << SPRITE_SHEET_SHIFT,
                TILE_SIZE * 2,
                TILE_SIZE * 3
                });
            surface.draw(_sprite);

            _cmdLine.draw(surface);
        } else {
            _sprite.setTextureRect({
                (_phaseTransitionStarted ? 99 : 102) << SPRITE_SHEET_SHIFT,
                (_phaseTransitionStarted ? 83 : 65) << SPRITE_SHEET_SHIFT,
                TILE_SIZE * 2,
                TILE_SIZE * 3
                });
            surface.draw(_sprite);

            if (_phaseTransitionStarted) {
                _headSprite.setTextureRect({
                    (98 + (_isBlinking ? 2 : 0)) << SPRITE_SHEET_SHIFT,
                    (65) << SPRITE_SHEET_SHIFT,
                    TILE_SIZE * 2,
                    TILE_SIZE * 2
                    });
                surface.draw(_headSprite);
            }
        }
    } else if (!_defeated) {
        _headSprite.setTextureRect({
            (98 + (_isBlinking ? 2 : 0)) << SPRITE_SHEET_SHIFT,
                (65) << SPRITE_SHEET_SHIFT,
                TILE_SIZE * 2,
                TILE_SIZE * 2
            });
        surface.draw(_headSprite);
    } else if (!_deathAnimationComplete) {
        surface.draw(_headSprite);
    }
}

void DevBoss::onStateChange(const BossState previousState, const BossState newState) {
    if (newState.stateId == RUN_COMMAND) {
        _animationState = TYPING;
        if (_currentCommand != START_PHASE_TWO) {
            std::vector<COMMAND> availableCommands;
            for (const auto& command : _commands) {
                if (command.first != _currentCommand
                    && command.first != START_PHASE_TWO
                    && !(command.first == SLOW_BULLETS && FinalBossEffectManager::effectIsActive(FINAL_BOSS_EFFECT::SLOW_BULLETS))) {
                    availableCommands.push_back(command.first);
                }
            }
            _currentCommand = availableCommands.at((size_t)randomInt(0, availableCommands.size() - 1));
        }
        _cmdLine.typeCommand(_commands.at(_currentCommand).text);
    } else if (newState.stateId == FIRE_PROJECILE) _animationState = WALKING;
    else if (newState.stateId == RADIAL_PROJECTILES || newState.stateId == BOMBS) _animationState = WALKING;
    else if (newState.stateId == ROTATING_LASERS) _animationState = HANDS_UP;

    if (previousState.stateId == RUN_COMMAND) _ranCommand = false;
}

void DevBoss::runCurrentState() {
    if (_defeated) return;

    switch (_currentState.stateId) {
        case FIRE_PROJECILE:
        {
            constexpr long long fireRate = 750LL;
            if (currentTimeMillis() - _lastProjectileFireTimeMillis >= fireRate) {
                const sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);
                const auto& proj = fireTargetedProjectile(playerPos, ProjectileDataManager::getData("_PROJECTILE_ERROR"), "NONE", true, false, {}, true, true);
                /*if (getHitBox().intersects(Viewport::getBounds())) */proj->bounceOffViewport = true;
                _lastProjectileFireTimeMillis = currentTimeMillis();
            }
            break;
        }
        case RUN_COMMAND:
        {
            if (!_cmdLine.isActive()) {
                if (!_ranCommand) runCommand(_currentCommand);
                else if (_ranCommand && _currentCommand == SPAWN_ENEMIES) {
                    constexpr long long spawnRate = 250LL;
                    if (currentTimeMillis() - _lastPackSpawnTime >= spawnRate && _numPacksSpawned < _maxPackAmount) {
                        spawnPack();
                        _lastPackSpawnTime = currentTimeMillis();
                        _numPacksSpawned++;
                        if (_numPacksSpawned >= _maxPackAmount) _permitStateChange = true;
                    }
                } else if (_ranCommand && _currentCommand == START_PHASE_TWO) {
                    if (getHitPoints() < getMaxHitPoints()) {
                        heal((getMaxHitPoints() - (getMaxHitPoints() / 2)) / (2 * 60));
                    } else _ranCommand = false;
                }
                _animationState = HANDS_UP;
            }
            break;
        }
        case BOMBS:
        {
            constexpr float bombChance = 0.15f;
            if (randomChance(bombChance)) {
                const sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);
                const sf::FloatRect vpBounds = Viewport::getBounds();
                const bool sides = randomChance(0.5f);
                const sf::Vector2f firePos = {
                    sides ? randomChance(0.5f) ? vpBounds.left : vpBounds.left + vpBounds.width : randomInt(vpBounds.left, vpBounds.left + vpBounds.width),
                    sides ? randomInt(vpBounds.top, vpBounds.top + vpBounds.height) : randomChance(0.5f) ? vpBounds.top : vpBounds.top + vpBounds.height
                };

                const float angle = std::atan2(playerPos.y - firePos.y, playerPos.x - firePos.x);
                const ProjectileData& projData = ProjectileDataManager::getData("_PROJECTILE_BOMB");
                Projectile* proj = ProjectilePoolManager::addProjectile(firePos, this, angle, projData.baseVelocity, projData, true, 0, false);
                proj->explosionsOnlyDamagePlayer = true;
                SoundManager::playSound("basicprojlaunch");
            }
            break;
        }
        case RADIAL_PROJECTILES:
        {
            constexpr float mouseChance = 0.05f;
            if (randomChance(mouseChance)) {
                const float angle = (float)randomInt(0, 360);
                Projectile* proj = fireTargetedProjectile(degToRads(angle), ProjectileDataManager::getData("_PROJECTILE_MOUSE"), "NONE", true, false, {}, false);

                constexpr float seekChance = 0.25f;
                if (randomChance(seekChance)) {
                    proj->targetSeeking = true;
                    proj->targetSeekStrength = (float)randomInt(50, 100) / 1000.f;
                }
            }
            break;
        }
    }
}

void DevBoss::blink() {
    const long long blinkDuration = 150LL;
    const int blinkChance = 600;
    if (!_isBlinking) {
        _isBlinking = randomInt(0, blinkChance) == 0;
        if (_isBlinking) _blinkStartTime = currentTimeMillis();
    } else if (currentTimeMillis() - _blinkStartTime > blinkDuration) _isBlinking = false;
}

void DevBoss::runCommand(COMMAND cmd) {
    if (cmd != NONE && !_ranCommand) {
        _ranCommand = true;
        switch (cmd) {
            case RESEED:
            {
                getWorld()->resetChunks();
                getWorld()->reseed(randomInt(0, 99999999));
                break;
            }
            case SPAWN_ENEMIES:
            {
                _permitStateChange = false;
                _numPacksSpawned = 0;
                _maxPackAmount = randomInt(4, 6);
                break;
            }
            case SLOW_BULLETS:
            {
                FinalBossEffectManager::activateEffect(FINAL_BOSS_EFFECT::SLOW_BULLETS, 5000LL);
                break;
            }
            case ANTIHOMING_BULLETS:
            {
                FinalBossEffectManager::activateEffect(FINAL_BOSS_EFFECT::ANTIHOMING_BULLETS, 8000LL);
                break;
            }
            case REVERSE_CONTROLS:
            {
                FinalBossEffectManager::activateEffect(FINAL_BOSS_EFFECT::INVERT_CONTROLS, 3000LL);
                break;
            }
        }
    }
}


void DevBoss::spawnPack() {
    constexpr int numMobTypes = 16;
    constexpr MOB_TYPE mobTypes[numMobTypes] = {
        MOB_TYPE::PLANT_MAN, MOB_TYPE::SNOW_MAN, MOB_TYPE::YETI, MOB_TYPE::SKELETON,
        MOB_TYPE::CYCLOPS, MOB_TYPE::FLESH_CHICKEN, MOB_TYPE::LOG_MONSTER, 
        MOB_TYPE::BOULDER_BEAST, MOB_TYPE::TULIP_MONSTER, MOB_TYPE::BURGER_BEAST,
        MOB_TYPE::BOMB_BOY, MOB_TYPE::MEGA_BOMB_BOY, MOB_TYPE::SOLDIER,
        MOB_TYPE::BIG_SNOW_MAN, MOB_TYPE::FUNGUY, MOB_TYPE::FUNGUS_MAN
    };

    const MOB_TYPE enemyType = mobTypes[randomInt(0, numMobTypes)];

    const int packSize = randomInt(4, 8);

    constexpr float packDist = 100.f;
    const sf::Vector2f packPos = { 
        (float)randomInt(getPosition().x - packDist, getPosition().x + packDist), 
        (float)randomInt(getPosition().y - packDist, getPosition().y + packDist) 
    };

    for (int i = 0; i < packSize; i++) {
        std::shared_ptr<Entity> enemy = nullptr;
        switch (enemyType) {
            case MOB_TYPE::PLANT_MAN:
                enemy = std::shared_ptr<PlantMan>(new PlantMan(packPos));
                break;
            case MOB_TYPE::SNOW_MAN:
                enemy = std::shared_ptr<SnowMan>(new SnowMan(packPos));
                break;
            case MOB_TYPE::YETI:
                enemy = std::shared_ptr<Yeti>(new Yeti(packPos));
                break;
            case MOB_TYPE::SKELETON:
                enemy = std::shared_ptr<Skeleton>(new Skeleton(packPos));
                break;
            case MOB_TYPE::CYCLOPS:
                enemy = std::shared_ptr<Cyclops>(new Cyclops(packPos));
                break;
            case MOB_TYPE::FLESH_CHICKEN:
                enemy = std::shared_ptr<FleshChicken>(new FleshChicken(packPos));
                break;
            case MOB_TYPE::LOG_MONSTER:
                enemy = std::shared_ptr<LogMonster>(new LogMonster(packPos));
                break;
            case MOB_TYPE::BOULDER_BEAST:
                enemy = std::shared_ptr<BoulderBeast>(new BoulderBeast(packPos));
                break;
            case MOB_TYPE::TULIP_MONSTER:
                enemy = std::shared_ptr<TulipMonster>(new TulipMonster(packPos));
                break;
            case MOB_TYPE::BURGER_BEAST:
                enemy = std::shared_ptr<BurgerBeast>(new BurgerBeast(packPos));
                break;
            case MOB_TYPE::BOMB_BOY:
                enemy = std::shared_ptr<BombBoy>(new BombBoy(packPos));
                break;
            case MOB_TYPE::MEGA_BOMB_BOY:
                enemy = std::shared_ptr<MegaBombBoy>(new MegaBombBoy(packPos));
                break;
            case MOB_TYPE::SOLDIER:
                enemy = std::shared_ptr<Soldier>(new Soldier(packPos));
                break;
            case MOB_TYPE::BIG_SNOW_MAN:
                enemy = std::shared_ptr<BigSnowMan>(new BigSnowMan(packPos));
                break;
            case MOB_TYPE::FUNGUY:
                enemy = std::shared_ptr<Funguy>(new Funguy(packPos));
                break;
            case MOB_TYPE::FUNGUS_MAN:
                enemy = std::shared_ptr<FungusMan>(new FungusMan(packPos));
                break;
        }

        if (enemy != nullptr) {
            enemy->setWorld(getWorld());
            enemy->loadSprite(getWorld()->getSpriteSheet());
            const int pennyIndex = enemy->getInventory().findItem(Item::PENNY.getId());
            if (pennyIndex != NO_ITEM) {
                enemy->getInventory().removeItem(Item::PENNY.getId(), enemy->getInventory().getItemAmountAt(pennyIndex));
            }
            getWorld()->addEntity(enemy);
        }
    }
}


void DevBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect({102 << SPRITE_SHEET_SHIFT, 68 << SPRITE_SHEET_SHIFT, TILE_SIZE * 2, TILE_SIZE * 3});
    _sprite.setOrigin((float)TILE_SIZE * 2.f / 2.f, 0);
    _sprite.setPosition(getPosition());

    _headSprite.setTexture(*spriteSheet);
    _headSprite.setTextureRect({98 << SPRITE_SHEET_SHIFT, 65 << SPRITE_SHEET_SHIFT, TILE_SIZE * 2, TILE_SIZE * 2});
    _headSprite.setOrigin((float)TILE_SIZE * 2.f / 2.f, 0);
    _headSprite.setPosition(getPosition().x, getPosition().y + 3);

    _bodySprite.setTexture(*spriteSheet);
    _bodySprite.setTextureRect({98 << SPRITE_SHEET_SHIFT, 73 << SPRITE_SHEET_SHIFT, TILE_SIZE, TILE_SIZE});
    _bodySprite.setOrigin((float)TILE_SIZE / 2.f, 0);
    _bodySprite.setPosition(getPosition().x, getPosition().y + TILE_SIZE * 2);
}

void DevBoss::damage(int damage) {
    if (_defeated) return;

    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _defeated = true;
        _permitStateChange = false;
        _numSteps = 0;
    }
}

void DevBoss::setWorld(World* world) {
    _world = world;

    if (!AbilityManager::playerHasAbility(Ability::BLESSING.getId())) {
        Inventory& pInventory = getWorld()->getPlayer()->getInventory();
        const unsigned int itemId = Item::getIdFromName("Dev's Blessing");
        pInventory.addItem(itemId, 1);
        pInventory.useItem(pInventory.findItem(itemId));
        pInventory.removeItem(itemId, 1);
    }
}
