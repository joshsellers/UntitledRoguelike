#include "MushroomBoss.h"
#include "../World.h"
#include "SwellingShroom.h"

MushroomBoss::MushroomBoss(sf::Vector2f pos) : Boss(MUSHROOM_BOSS, pos, 1.f, 8 * TILE_SIZE, 9 * TILE_SIZE, 
    {
        BossState(SPAWN_SHROOMS, 5000, 5000),
        BossState(RAPID_FIRE_SHROOMS, 5000, 7000),
        BossState(BIGSHROOM_SNIPE, 1000, 1000)
    }
) {
    setMaxHitPoints(41500);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 2) - 8;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 4;
    _hitBox.height = TILE_SIZE * 9;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;

    _entityType = "shroomboss";
    _displayName = "Anita";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(8500, 10000);
    getInventory().addItem(Item::PENNY.getId(), pennyAmount);
}

void MushroomBoss::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 8;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + (float)TILE_SIZE * 4.5f));

        const int yOffset = ((_numSteps >> 3) & 1) * TILE_SIZE;

        _wavesSprite.setTextureRect(sf::IntRect(400, 688 + yOffset, TILE_SIZE * 5, TILE_SIZE));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - (float)TILE_SIZE * 8.f / 2.f + 24.f - 8.f, getPosition().y + (TILE_SIZE * 9)));
        surface.draw(_wavesSprite);
    }

    const int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 7) * TILE_SIZE * 9 : 0;
    const int xOffset = _currentState.stateId == BIGSHROOM_SNIPE ? 11 : 0;

    _sprite.setTextureRect(sf::IntRect(
        ((150 + xOffset) << SPRITE_SHEET_SHIFT), (40 << SPRITE_SHEET_SHIFT) + yOffset, TILE_SIZE * 8, isSwimming() ? (float)TILE_SIZE * 4.5f : TILE_SIZE * 9
    ));

    surface.draw(_sprite);
}

void MushroomBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(150 << SPRITE_SHEET_SHIFT, 40 << SPRITE_SHEET_SHIFT, TILE_SIZE * 8, TILE_SIZE * 9));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 9 / 2, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}

void MushroomBoss::subUpdate() {
    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 9);

    float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
    float desiredDist = 150.f;
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
        _hitBoxYOffset = TILE_SIZE * 5;
        _hitBox.height = TILE_SIZE * 4;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 9;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    constexpr long long contactDamageRateMillis = 500LL;
    if (getWorld()->getPlayer()->isActive()
        && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())
        && currentTimeMillis() - _lastContactDamageTimeMillis >= contactDamageRateMillis) {
        getWorld()->getPlayer()->takeDamage(30);
        _lastContactDamageTimeMillis = currentTimeMillis();
    }
}

void MushroomBoss::onStateChange(const BossState previousState, const BossState newState) {
    if (previousState.stateId == SPAWN_SHROOMS) {
        _numShroomsSpawned = 0;
    }
}

void MushroomBoss::runCurrentState() {
    switch (_currentState.stateId) {
        case SPAWN_SHROOMS:
        {
            constexpr long long spawnDelay = 500LL;
            constexpr int shroomCount = 6;
            if (currentTimeMillis() - _lastShroomSpawnTime >= spawnDelay && _numShroomsSpawned < 6) {
                const sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);

                auto& shroom = std::shared_ptr<SwellingShroom>(new SwellingShroom(sf::Vector2f(playerPos.x + randomInt(-32, 32), playerPos.y + randomInt(-32, 32))));
                shroom->setWorld(getWorld());
                shroom->loadSprite(getWorld()->getSpriteSheet());
                getWorld()->addEntity(shroom);

                _numShroomsSpawned++;
                _lastShroomSpawnTime = currentTimeMillis();
            }
            break;
        }
        case RAPID_FIRE_SHROOMS:
        {
            constexpr long long fireRate = 500LL;
            if (currentTimeMillis() - _lastFireTimeMillis >= fireRate) {
                float angle = 0;
                constexpr int projCount = 90;
                bool skippedLast = false;
                for (int i = 0; i < projCount; i++) {
                    if (skippedLast) {
                        skippedLast = false;
                        continue;
                    }

                    if (randomChance(0.25f)) {
                        skippedLast = true;
                        continue;
                    }
                    angle = i * (360.f / (float)projCount);
                    fireTargetedProjectile(degToRads(angle), ProjectileDataManager::getData("_PROJECTILE_SMALL_SHROOM"), "NONE", true);
                }
                _lastFireTimeMillis = currentTimeMillis();
            }
            break;
        }
        case BIGSHROOM_SNIPE:
        {
            constexpr long long fireRate = 333LL;
            if (currentTimeMillis() - _lastFireTimeMillis >= fireRate) {
                const sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
                fireTargetedProjectile(playerPos, ProjectileDataManager::getData("_PROJECTILE_LARGE_SHROOM"), "basicprojlaunch", true, false, {0, 50});
                _lastFireTimeMillis = currentTimeMillis();
            }
            break;
        }
    }
}
