#include "PenguinBoss.h"
#include "../World.h"
#include "../../core/SoundManager.h"
#include "LaserBeam.h"
#include "projectiles/Projectile.h"
#include "HypnoPenguin.h"

PenguinBoss::PenguinBoss(sf::Vector2f pos) : Boss(PENGUIN_BOSS, pos, 1.f, TILE_SIZE * 9, TILE_SIZE * 12, 
    {
        BossState(PSYCH_LASER, 4000LL, 5000LL),
        BossState(HOMING_SNOWBALLS, 1500LL, 3000LL),
        BossState(SPAWN_PENGUINS, 2500LL, 4500LL)
    }) {
    setMaxHitPoints(117000);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 9) / 2 + (float)TILE_SIZE * 2.f;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 5;
    _hitBox.height = TILE_SIZE * 12;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;

    _entityType = "penguinboss";
    _displayName = "Princeton the Penguin";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(20000, 22000);
    getInventory().addItem(Item::PENNY.getId(), pennyAmount);

    _animSpeed = 1;
}

void PenguinBoss::subUpdate() {
    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 8);

    float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
    float desiredDist = 150.f;
    float distanceRatio = desiredDist / dist;

    float xa = 0.f, ya = 0.f;

    sf::Vector2f goalPos((1.f - distanceRatio) * playerPos.x + distanceRatio * cLoc.x, (1.f - distanceRatio) * playerPos.y + distanceRatio * cLoc.y);
    if (goalPos.y < cLoc.y) {
        ya--;
    } else if (goalPos.y > cLoc.y) {
        ya++;
    }

    if (goalPos.x < cLoc.x) {
        xa--;
    } else if (goalPos.x > cLoc.x) {
        xa++;
    }

    if (xa && ya) {
        xa *= 0.785398;
        ya *= 0.785398;
    }

    if (dist < desiredDist) {
        xa = 0;
        ya = 0;
    }

    if (playerPos.x < cLoc.x) _movingDir = LEFT;
    else if (playerPos.x > cLoc.x) _movingDir = RIGHT;

    if (_movingDir == LEFT) {
        _sprite.setScale({ -1, 1 });
        _laserEyesSprite.setScale(-1, 1);
    } else if (_movingDir == RIGHT) {
        _sprite.setScale({ 1, 1 });
        _laserEyesSprite.setScale(1, 1);
    }

    if (_spawnedWithEnemies) hoardMove(xa, ya, true, 128);
    else if (!_isChargingLaser) move(xa, ya);

    _sprite.setPosition(getPosition());
    _laserEyesSprite.setPosition(getPosition());

    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE * 6;
        _hitBox.height = TILE_SIZE * 6;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 12;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    if (_isChargingLaser) {
        const sf::Vector2i textureCoords(178, 32);
        constexpr int ticksPerFrame = 2;
        constexpr int frameCount = 6;
        const int yOffset = ((_animCounter / ticksPerFrame) % frameCount);

        _animCounter++;

        if (yOffset == frameCount - 1) _isChargingLaser = false;
    }
}

void PenguinBoss::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 13;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 6));
        _laserEyesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 6));

        int yOffset = ((_numSteps >> 3) & 1) * TILE_SIZE;

        _wavesSprite.setTextureRect(sf::IntRect(400, 688 + yOffset, TILE_SIZE * 5, TILE_SIZE));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - (float)TILE_SIZE * 9.f / 2.f + 32.f, getPosition().y + (TILE_SIZE * 12)));
        surface.draw(_wavesSprite);
    }

    int yOffset = isMoving() && !isSwimming()  ? ((_numSteps >> _animSpeed) & 7) * TILE_SIZE * 12 : 0;

    const sf::Vector2i texturePos = isMoving() && !isSwimming() ? 
        sf::Vector2i(169 << SPRITE_SHEET_SHIFT, 20 << SPRITE_SHEET_SHIFT) 
        : sf::Vector2i(178 << SPRITE_SHEET_SHIFT, 20 << SPRITE_SHEET_SHIFT);

    _sprite.setTextureRect(sf::IntRect(
        texturePos.x, texturePos.y + yOffset, TILE_SIZE * 9, isSwimming() ? TILE_SIZE * 6 : TILE_SIZE * 12
    ));

    surface.draw(_sprite);

    if (_isChargingLaser) {
        _laserEyesSprite.setTextureRect(sf::IntRect(
            178 << SPRITE_SHEET_SHIFT, (32 << SPRITE_SHEET_SHIFT) + yOffset * TILE_SIZE * 12, TILE_SIZE * 9, TILE_SIZE * 12
        ));
        
        surface.draw(_laserEyesSprite);
    } else if (_currentState.stateId == PSYCH_LASER || _currentState.stateId == HOMING_SNOWBALLS) {
        _laserEyesSprite.setTextureRect(sf::IntRect(
            178 << SPRITE_SHEET_SHIFT, (32 << SPRITE_SHEET_SHIFT) + 5 * TILE_SIZE * 12, TILE_SIZE * 9, TILE_SIZE * 12
        ));

        surface.draw(_laserEyesSprite);
    }
}

void PenguinBoss::onStateChange(const BossState previousState, const BossState newState) {
    if (newState.stateId == PSYCH_LASER) {
        _laserChargeStartTime = currentTimeMillis();
        _isChargingLaser = true;
        _firedLaser = false;
        _animCounter = 0;

        SoundManager::playSound("altaractivation");
    }
}

void PenguinBoss::runCurrentState() {
    switch (_currentState.stateId) {
        case PSYCH_LASER:
        {
            constexpr long long laserFireDelay = 450LL;
            if (!_isChargingLaser && !_firedLaser && currentTimeMillis() - _laserChargeStartTime >= laserFireDelay) {
                const sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);
                const auto& laser = std::shared_ptr<LaserBeam>(new LaserBeam(this, playerPos, 0xFFFFFFFF, 16, 600, 30, {0, isSwimming() ? (float)TILE_SIZE * 7.f : 16.f}, true, 1250));
                laser->setWorld(getWorld());
                laser->loadSprite(getWorld()->getSpriteSheet());
                laser->setTextureRect(
                    sf::IntRect(
                        2144, 1984, 10 * TILE_SIZE, 6 * TILE_SIZE
                    ),
                    0, 
                    0,
                    true
                );
                getWorld()->addEntity(laser);
                _firedLaser = true;
            }
            break;
        }
        case HOMING_SNOWBALLS:
        {
            constexpr long long fireRate = 900LL;
            if (currentTimeMillis() - _lastFireTimeMillis >= fireRate) {
                const sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);
                const auto& projectile = fireTargetedProjectile(
                    playerPos, ProjectileDataManager::getData("_PROJECTILE_HOMING_SNOWBALL"), "NONE", true, false, { 0, -(float)TILE_SIZE * 6.f }, true
                );
                projectile->targetSeeking = true;
                projectile->targetSeekStrength = 0.15f;
                _lastFireTimeMillis = currentTimeMillis();
            }
            break;
        }
        case SPAWN_PENGUINS:
        {
            constexpr long long spawnRate = 500LL;
            if (currentTimeMillis() - _lastFireTimeMillis >= spawnRate) {
                const auto& penguin = std::shared_ptr<HypnoPenguin>(new HypnoPenguin({getPosition().x, getPosition().y + 150.f}));
                penguin->setWorld(getWorld());
                penguin->loadSprite(getWorld()->getSpriteSheet());
                getWorld()->addEntity(penguin);
                _lastFireTimeMillis = currentTimeMillis();
            }
            break;
        }
    }
}

void PenguinBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(178 << SPRITE_SHEET_SHIFT, 20 << SPRITE_SHEET_SHIFT, TILE_SIZE * 9, TILE_SIZE * 12));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 9 / 2, 0);

    _laserEyesSprite.setTexture(*spriteSheet);
    _laserEyesSprite.setTextureRect(sf::IntRect(178 << SPRITE_SHEET_SHIFT, 32 << SPRITE_SHEET_SHIFT, TILE_SIZE * 9, TILE_SIZE * 12));
    _laserEyesSprite.setPosition(getPosition());
    _laserEyesSprite.setOrigin((float)TILE_SIZE * 9 / 2, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}
