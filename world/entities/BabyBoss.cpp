#include "BabyBoss.h"
#include "../World.h"
#include "FallingNuke.h"
#include "Soldier.h"

BabyBoss::BabyBoss(sf::Vector2f pos) : Boss(BABY_BOSS, pos, 1.f, TILE_SIZE * 6, TILE_SIZE * 8,
    {
        BossState(BEHAVIOR_STATE::REST, 1000LL, 2000LL),
        BossState(BEHAVIOR_STATE::SPAM_BOMBS, 4000LL, 5000LL),
        BossState(BEHAVIOR_STATE::DROP_NUKES, 2000LL, 2000LL),
        BossState(BEHAVIOR_STATE::SPAWN_SOLDIERS, 5000LL, 6000LL)
    }) {
    setMaxHitPoints(24000);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 6) / 2 + TILE_SIZE;
    _hitBoxYOffset = TILE_SIZE;
    _hitBox.width = TILE_SIZE * 4;
    _hitBox.height = TILE_SIZE * 7;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;

    _entityType = "babyboss";
    _displayName = "Lieutenant Timmy";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(4500, 5250);
    getInventory().addItem(Item::PENNY.getId(), pennyAmount);
}

void BabyBoss::subUpdate() {
    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 8);

    float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
    constexpr float desiredDist = 200.f;
    float distanceRatio = desiredDist / dist;

    float xa = 0.f, ya = 0.f;

    sf::Vector2f goalPos((1.f - distanceRatio) * playerPos.x + distanceRatio * cLoc.x, (1.f - distanceRatio) * playerPos.y + distanceRatio * cLoc.y);
    if (goalPos.y < cLoc.y) {
        ya--;
        _movingDir = LEFT;
    } else if (goalPos.y > cLoc.y) {
        ya++;
        _movingDir = DOWN;
    }

    if (goalPos.x < cLoc.x) {
        xa--;
    } else if (goalPos.x > cLoc.x) {
        xa++;
    }

    if (xa && ya) {
        xa *= 0.785398;
        ya *= 0.785398;

        if (ya > 0) _movingDir = DOWN;
        else _movingDir = LEFT;
    }

    if (dist < desiredDist) {
        xa = 0;
        ya = 0;

        if (_movingDir == LEFT) _movingDir = DOWN;
        else if (_movingDir == DOWN) _movingDir = LEFT;
    }

    if (_spawnedWithEnemies) hoardMove(xa, ya, true, 128);
    else move(xa, ya);

    _sprite.setPosition(getPosition());

    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE * 2;
        _hitBox.height = TILE_SIZE * 6;
    } else {
        _hitBoxYOffset = TILE_SIZE;
        _hitBox.height = TILE_SIZE * 7;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    constexpr long long contactDamageRateMillis = 500LL;
    if (getWorld()->getPlayer()->isActive()
        && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())
        && currentTimeMillis() - _lastContactDamageTimeMillis >= contactDamageRateMillis) {
        getWorld()->getPlayer()->takeDamage(50);
        _lastContactDamageTimeMillis = currentTimeMillis();
    }

    if (_currentState.stateId != SPAM_BOMBS) blink();
}

void BabyBoss::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 8;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 2));

        int yOffset = ((_numSteps >> 3) & 1) * TILE_SIZE;

        _wavesSprite.setTextureRect(sf::IntRect(400, 688 + yOffset, TILE_SIZE * 5, TILE_SIZE));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - (float)TILE_SIZE * 6.f / 2.f + 8.f, getPosition().y + (TILE_SIZE * 8)));
        surface.draw(_wavesSprite);
    }

    int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 7) * TILE_SIZE * 8 : 0;

    _sprite.setTextureRect(sf::IntRect(
        (_currentState.stateId == SPAM_BOMBS ? 2048 : (_isBlinking ? 1760 : 1664)), 640 + yOffset, TILE_SIZE * 6, isSwimming() ? TILE_SIZE * 6 : TILE_SIZE * 8
    ));

    surface.draw(_sprite);
}

void BabyBoss::onStateChange(const BossState previousState, const BossState newState) {
    if (newState.stateId == DROP_NUKES) _spawnedNukes = false;
    else if (newState.stateId == SPAWN_SOLDIERS) {
        _soldiersToSpawn = randomInt(3, 5);
        _soldiersSpawned = 0;
    }
}

void BabyBoss::runCurrentState() {
    switch (_currentState.stateId) {
        case SPAM_BOMBS:
        {
            constexpr long long fireRate = 100LL;
            if (currentTimeMillis() - _lastFireTimeMillis >= fireRate) {
                const sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);
                constexpr int aimCone = 100;
                const sf::Vector2f targetPos(playerPos.x + randomInt(-aimCone, aimCone), playerPos.y + randomInt(-aimCone, aimCone));
                fireTargetedProjectile(targetPos, ProjectileDataManager::getData("_PROJECTILE_BABYBOSS_BOMB"), "basicprojlaunch", true, false, {0, -12}, false);
                _lastFireTimeMillis = currentTimeMillis();
            }
            break;
        }
        case DROP_NUKES:
        {
            if (!_spawnedNukes) {
                const sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);

                const unsigned int nukeCount = HARD_MODE_ENABLED ? randomInt(1, 2) : 1;
                for (int i = 0; i < nukeCount; i++) {
                    const auto& nuke = std::shared_ptr<FallingNuke>(new FallingNuke(playerPos));
                    nuke->setWorld(getWorld());
                    nuke->loadSprite(getWorld()->getSpriteSheet());
                    getWorld()->addEntity(nuke);
                }

                _spawnedNukes = true;
            }
            break;
        }
        case SPAWN_SOLDIERS:
        {
            if (_soldiersSpawned < _soldiersToSpawn) {
                sf::Vector2f spawnPos;
                const int dist = randomInt(0, 3);
                constexpr int maxDist = 128;
                constexpr int minDist = 64;
                if (dist == 0) {
                    spawnPos.x = getPosition().x + randomInt(-maxDist, maxDist);
                    spawnPos.y = getPosition().y - randomInt(minDist, maxDist);
                } else if (dist == 1) {
                    spawnPos.x = getPosition().x + randomInt(-maxDist, maxDist);
                    spawnPos.y = getPosition().y + randomInt(minDist, maxDist);
                } else if (dist == 2) {
                    spawnPos.x = getPosition().x - randomInt(minDist, maxDist);
                    spawnPos.y = getPosition().y + randomInt(-maxDist, maxDist);
                } else if (dist == 3) {
                    spawnPos.x = getPosition().x + randomInt(minDist, maxDist);
                    spawnPos.y = getPosition().y + randomInt(-maxDist, maxDist);
                }

                if (currentTimeMillis() - _lastSoldierSpawnTime >= _timeToNextSoldierSpawn) {
                    std::shared_ptr<Soldier> soldier = std::shared_ptr<Soldier>(new Soldier(spawnPos));
                    soldier->loadSprite(getWorld()->getSpriteSheet());
                    soldier->setWorld(getWorld());
                    getWorld()->addEntity(soldier);

                    _lastSoldierSpawnTime = currentTimeMillis();
                    _timeToNextSoldierSpawn = randomInt(250, 1000);
                    _soldiersSpawned++;
                }
            }
            break;
        }
    }
}

void BabyBoss::blink() {
    const long long blinkDuration = 150LL;
    const int blinkChance = 600;
    if (!_isBlinking) {
        _isBlinking = randomInt(0, blinkChance) == 0;
        if (_isBlinking) _blinkStartTime = currentTimeMillis();
    } else if (currentTimeMillis() - _blinkStartTime > blinkDuration) _isBlinking = false;
}

void BabyBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(1664, 640, TILE_SIZE * 6, TILE_SIZE * 8));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 6 / 2, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}