#include "CannonBoss.h"
#include "../../core/Util.h"
#include "../World.h"
#include "orbiters/Orbiter.h"

CannonBoss::CannonBoss(sf::Vector2f pos) : Boss(CANNON_BOSS, pos, 1, TILE_SIZE * 6, TILE_SIZE * 7,
    {
        BossState(BEHAVIOR_STATE::REST, 3000LL, 5000LL),
        BossState(BEHAVIOR_STATE::RING_OF_BLOOD, 11000LL, 12000LL),
        //BossState(BEHAVIOR_STATE::TARGETED_FIRE, 2000LL, 4000LL),
        BossState(BEHAVIOR_STATE::BLASTING, 4000LL, 6000LL)
    })
{
    setMaxHitPoints(1750);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 6) / 2;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 6;
    _hitBox.height = TILE_SIZE * 7;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;

    _entityType = "cannonboss";
    _displayName = "Professor Hands";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(50000, 100000);
    getInventory().addItem(Item::PENNY.getId(), pennyAmount);

    _animSpeed = 1;
}

void CannonBoss::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 7;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 3));

        int yOffset = ((_numSteps >> 3) & 1) * TILE_SIZE;

        _wavesSprite.setTextureRect(sf::IntRect(400, 688 + yOffset, TILE_SIZE * 5, TILE_SIZE));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - (float)TILE_SIZE * 7.f / 2.f + 10.f, getPosition().y + (TILE_SIZE * 7) - 16));
        surface.draw(_wavesSprite);
    }

    int xOffset = getMovingDir() * TILE_SIZE * 6;
    int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 7) * TILE_SIZE * 7 : 0;

    _sprite.setTextureRect(sf::IntRect(
        1040 + xOffset, 640 + yOffset, TILE_SIZE * 6, isSwimming() ? TILE_SIZE * 3 : TILE_SIZE * 7
    ));

    surface.draw(_sprite);
}

void CannonBoss::subUpdate() {
    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 7);

    float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
    float desiredDist = 110.f;
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
        _hitBoxYOffset = TILE_SIZE * 3;
        _hitBox.height = TILE_SIZE * 3;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 7;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    constexpr long long contactDamageRateMillis = 500LL;
    if (getWorld()->getPlayer()->isActive()
        && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())
        && currentTimeMillis() - _lastContactDamageTimeMillis >= contactDamageRateMillis) {
        getWorld()->getPlayer()->takeDamage(10);
        _lastContactDamageTimeMillis = currentTimeMillis();
    }
}

void CannonBoss::onStateChange(const BossState previousState, const BossState newState) {
    switch (previousState.stateId) {
        case RING_OF_BLOOD:
            _currentOrbiterCount = 0;
            break;
    }
}

void CannonBoss::runCurrentState() {
    switch (_currentState.stateId) {
        case RING_OF_BLOOD:
        {
            constexpr int maxOrbiterCount = 8*2;
            constexpr long long orbiterSpawnRateMillis = 250LL / 2;

            if (_currentOrbiterCount < maxOrbiterCount && currentTimeMillis() - _lastOrbiterSpawnTime >= orbiterSpawnRateMillis) {
                std::shared_ptr<Orbiter> orbiter = std::shared_ptr<Orbiter>(new Orbiter(90.f, OrbiterType::BLOOD_BALL.getId(), this));
                orbiter->loadSprite(getWorld()->getSpriteSheet());
                orbiter->setWorld(getWorld());
                getWorld()->addEntity(orbiter);

                orbiter->setCenterPointOffset(0, TILE_SIZE * 7.f / 2.f - 16.f);

                _currentOrbiterCount++;
                _lastOrbiterSpawnTime = currentTimeMillis();
            }
            break;
        }
        case TARGETED_FIRE:
        {
            if (currentTimeMillis() - _lastFireTimeMillis >= _fireRateMillis) {
                sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
                
                const sf::Vector2f playerVel = _world->getPlayer()->getVelocity();
                playerPos.x += playerVel.x * 4;
                playerPos.y += playerVel.y * 4;

                fireTargetedProjectile(playerPos, Item::DATA_PROJECTILE_LARGE_BLOOD_BALL, "NONE", true, getMovingDir() == DOWN);
                _lastFireTimeMillis = currentTimeMillis();
            }
            break;
        }
        case BLASTING:
        {
            if (currentTimeMillis() - _lastBlastTimeMillis >= _blastRateMillis) {
                constexpr int projectilesPerBlast = 30;
                float fireAngle = (float)randomInt(0, 360);
                for (int i = 0; i < projectilesPerBlast; i++) {
                    fireAngle += 360.f / projectilesPerBlast;
                    if (fireAngle >= 360.f) fireAngle -= 360.f;

                    const float fireAngleRads = fireAngle * ((float)PI / 180.f);
                    fireTargetedProjectile(fireAngleRads, Item::DATA_PROJECTILE_LARGE_BLOOD_BALL, "NONE", true, getMovingDir() == DOWN);

                    _lastBlastTimeMillis = currentTimeMillis();
                }
            }
            break;
        }
    }
}

void CannonBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(1040, 640, TILE_SIZE * 6, TILE_SIZE * 7));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 6 / 2, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}