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

DevBoss::DevBoss(sf::Vector2f pos) : Boss(DEV_BOSS, pos, 1.f, 2 * TILE_SIZE, 3 * TILE_SIZE,
    {
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
}

void DevBoss::subUpdate() {
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
}

void DevBoss::draw(sf::RenderTexture& surface) {
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
            102 << SPRITE_SHEET_SHIFT, 
            65 << SPRITE_SHEET_SHIFT, 
            TILE_SIZE * 2, 
            TILE_SIZE * 3 
        });
        surface.draw(_sprite);
    }
}

void DevBoss::onStateChange(const BossState previousState, const BossState newState) {
    if (newState.stateId == RUN_COMMAND) {
        _animationState = TYPING;
        std::vector<COMMAND> availableCommands;
        for (const auto& command : _commands) {
            if (command.first != _currentCommand
                && !(_currentCommand == SLOW_BULLETS && FinalBossEffectManager::effectIsActive(FINAL_BOSS_EFFECT::SLOW_BULLETS))) {
                availableCommands.push_back(command.first);
            }
        }
        _currentCommand = availableCommands.at((size_t)randomInt(0, availableCommands.size() - 1));
        _cmdLine.typeCommand(_commands.at(_currentCommand).text);
    } else if (newState.stateId == FIRE_PROJECILE) _animationState = WALKING;

    if (previousState.stateId == RUN_COMMAND) _ranCommand = false;
}

void DevBoss::runCurrentState() {
    switch (_currentState.stateId) {
        case FIRE_PROJECILE:
        {
            constexpr long long fireRate = 750LL;
            if (currentTimeMillis() - _lastProjectileFireTimeMillis >= fireRate) {
                const sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);
                const auto& proj = fireTargetedProjectile(playerPos, ProjectileDataManager::getData("_PROJECTILE_ERROR"), "NONE", true, false, {}, true, true);
                if (getHitBox().intersects(Viewport::getBounds())) proj->bounceOffViewport = true;
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
                }
                _animationState = HANDS_UP;
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