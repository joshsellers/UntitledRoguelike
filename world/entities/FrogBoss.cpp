#include "FrogBoss.h"
#include "../../core/Util.h"
#include "../World.h"
#include "../../core/Viewport.h"
#include "ImpactExplosion.h"
#include "orbiters/Orbiter.h"
#include "../../core/SoundManager.h"

FrogBoss::FrogBoss(sf::Vector2f pos) : Boss(FROG_BOSS, pos, 1.f, TILE_SIZE * 7, TILE_SIZE * 10,
    {
        BossState(REST, 1000LL, 3000LL),
        BossState(JUMP, 1000LL, 1000LL),
        BossState(SPAWN_ORBITERS, 1000LL, 2000LL),
        BossState(SHOOT_FROGS, 2000LL, 3000LL)
    }
) {
    setMaxHitPoints(86000);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -((float)TILE_SIZE * 7.f) / 2.f + TILE_SIZE;
    _hitBoxYOffset = TILE_SIZE;
    _hitBox.width = TILE_SIZE * 5;
    _hitBox.height = TILE_SIZE * 10;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;

    _entityType = "frogboss";
    _displayName = "Remmy Ribbit";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(15000, 17000);
    getInventory().addItem(Item::PENNY.getId(), pennyAmount);
}

void FrogBoss::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 10;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 2));
        _headSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 2));

        int yOffset = ((_numSteps >> 3) & 1) * TILE_SIZE;

        _wavesSprite.setTextureRect(sf::IntRect(400, 688 + yOffset, TILE_SIZE * 5, TILE_SIZE));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - (float)TILE_SIZE * 6.f / 2.f + 8.f, getPosition().y + (TILE_SIZE * 10)));
        surface.draw(_wavesSprite);
    }

    if (_currentState.stateId != JUMP) {
        int xOffset = isMoving() ? ((_numSteps >> _animSpeed) & 7) * TILE_SIZE * 7 : 0;

        _sprite.setTextureRect(sf::IntRect(
            1680 + xOffset, 464, TILE_SIZE * 7, isSwimming() ? TILE_SIZE * 8 : TILE_SIZE * 10
        ));

        _headSprite.setTextureRect(sf::IntRect(
            1696 + _headFrame * TILE_SIZE * 5, 400, TILE_SIZE * 5, TILE_SIZE * 4
        ));

        surface.draw(_sprite);
        surface.draw(_headSprite);
    } else {
        _sprite.setTextureRect(sf::IntRect(
            2096 + _jumpFrame * TILE_SIZE * 7, 304, TILE_SIZE * 7, isSwimming() ? TILE_SIZE * 8 : TILE_SIZE * 10
        ));

        if (_jumpState == FALLING || !getHitBox().intersects(Viewport::getBounds())) surface.draw(_shadowSprite);

        surface.draw(_sprite);
    }
}

void FrogBoss::subUpdate() {
    if (_currentState.stateId != JUMP) {
        sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
        sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 10);

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
    }

    _sprite.setPosition(getPosition());
    _headSprite.setPosition(getPosition());

    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE * 2;
        _hitBox.height = TILE_SIZE * 8;
    } else {
        _hitBoxYOffset = TILE_SIZE;
        _hitBox.height = TILE_SIZE * 9;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    constexpr long long contactDamageRateMillis = 500LL;
    if (_currentState.stateId != JUMP && getWorld()->getPlayer()->isActive()
        && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())
        && currentTimeMillis() - _lastContactDamageTimeMillis >= contactDamageRateMillis) {
        getWorld()->getPlayer()->takeDamage(50);
        _lastContactDamageTimeMillis = currentTimeMillis();
    }
}

void FrogBoss::onStateChange(const BossState previousState, const BossState newState) {
    if (previousState.stateId == SHOOT_FROGS) {
        _headFrame = 0;
        _animCounter = 0;
    } else if (previousState.stateId == JUMP) {
        _jumpState = CHARGING;
    } else if (previousState.stateId == SPAWN_ORBITERS) _orbitersSpawned = false;

    if (newState.stateId == JUMP) {
        _permitStateChange = false;
    }
}

void FrogBoss::runCurrentState() {
    switch (_currentState.stateId) {
        case SHOOT_FROGS: {
            constexpr int ticksPerFrame = 4;
            constexpr int frameCount = 5;
            _headFrame = ((_animCounter / ticksPerFrame) % frameCount);

            constexpr long long fireRate = 5LL;

            if (_headFrame < frameCount - 1) _animCounter++;
            else if (currentTimeMillis() - _lastFireTimeMillis >= fireRate){
                sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + (float)PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);
                const sf::Vector2f targetPos(playerPos.x, playerPos.y);

                const sf::Vector2f centerPoint(getPosition().x, getPosition().y + _spriteHeight / 2);
                const sf::Vector2f centerOffset(8, -35);
                const double x = (double)(targetPos.x - (centerPoint.x + centerOffset.x));
                const double y = (double)(targetPos.y - (centerPoint.y + centerOffset.y));

                const float angle = (float)((std::atan2(y, x)));

                const float fireChance = HARD_MODE_ENABLED ? 0.15f : 0.05f;

                if (randomChance(fireChance)) {
                    fireTargetedProjectile(
                        angle, ProjectileDataManager::getData("_PROJECTILE_FROG"), "basicprojlaunch", true, false, { 8, -35 }
                    );
                }
                const float angleOffset = degToRads(15.f);
                if (randomChance(fireChance)) {
                    fireTargetedProjectile(
                        angle + angleOffset, ProjectileDataManager::getData("_PROJECTILE_FROG"), "basicprojlaunch", true, false, { 8, -35 }
                    );
                }
                if (randomChance(fireChance)) {
                    fireTargetedProjectile(
                        angle - angleOffset, ProjectileDataManager::getData("_PROJECTILE_FROG"), "basicprojlaunch", true, false, { 8, -35 }
                    );
                }
                _lastFireTimeMillis = currentTimeMillis();
            }
            break;
        }
        case JUMP: {
            const float airSpeed = HARD_MODE_ENABLED ? 20.f : 14.f;
            switch (_jumpState) {
                case CHARGING: {
                    constexpr int ticksPerFrame = 4;
                    constexpr int frameCount = 4;
                    _jumpFrame = ((_animCounter / ticksPerFrame) % frameCount);

                    if (_jumpFrame < frameCount - 1) _animCounter++;
                    else {
                        _jumpState = RISING;
                        _jumpFrame = 0;
                        _animCounter = 0;
                        SoundManager::playSound("jump");
                    }
                    break;
                }
                case RISING: {
                    constexpr float jumpHeight = 200.f;
                    if (getPosition().y + TILE_SIZE * 10 > getWorld()->getPlayer()->getPosition().y - jumpHeight) {
                        move(0, -airSpeed);
                    } else {
                        _jumpState = FALLING;
                        _pos.x = _world->getPlayer()->getPosition().x + (float)PLAYER_WIDTH / 2;
                        _pos.y = _world->getPlayer()->getPosition().y - TILE_SIZE * 10 - Viewport::getBounds().height / 2.f;
                        _landingTarget = { _world->getPlayer()->getPosition().x + (float)PLAYER_WIDTH / 2, _world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2 };
                        _shadowSprite.setPosition(_landingTarget);
                    }
                    break;
                }
                case FALLING: {
                    if (_pos.y + TILE_SIZE * 10 < _landingTarget.y) move(0, airSpeed);
                    else {
                        _jumpState = LANDED;
                        if (getHitBox().intersects(_world->getPlayer()->getHitBox())) {
                            _world->getPlayer()->takeDamage(60);
                        }
                        const int layerCount = HARD_MODE_ENABLED ? 12 : 10;
                        const int propogationRate = HARD_MODE_ENABLED ? 4 : 2;
                        const auto& impact = std::shared_ptr<ImpactExplosion>(new ImpactExplosion({ getPosition().x, getPosition().y + TILE_SIZE * 8 }, layerCount, propogationRate));
                        impact->setWorld(getWorld());
                        getWorld()->addEntity(impact);
                    }
                    break;
                }
                case LANDED: {
                    _permitStateChange = true;
                    break;
                }
            }
            break;
        }
        case SPAWN_ORBITERS: {
            if (!_orbitersSpawned) {
                const int orbiterCount = HARD_MODE_ENABLED ? 8 : 6;
                for (int i = 0; i < orbiterCount; i++) {
                    const auto& orbiter = std::shared_ptr<Orbiter>(new Orbiter(360.f / orbiterCount * i, OrbiterType::FROG.getId(), _world->getPlayer().get()));
                    orbiter->setWorld(getWorld());
                    orbiter->loadSprite(getWorld()->getSpriteSheet());
                    orbiter->setAttackFrequencyOffset(randomInt(250LL, 2000LL));

                    getWorld()->addEntity(orbiter);
                }

                _orbitersSpawned = true;
            }
            break;
        }
    }
}

void FrogBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(1680, 464, TILE_SIZE * 7, TILE_SIZE * 10));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 7.f / 2.f, 0);

    _headSprite.setTexture(*spriteSheet);
    _headSprite.setTextureRect(sf::IntRect(1696, 400, TILE_SIZE * 5, TILE_SIZE * 4));
    _headSprite.setPosition(getPosition());
    _headSprite.setOrigin((float)TILE_SIZE * 5.f / 2.f, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));

    _shadowSprite.setTexture(*spriteSheet);
    _shadowSprite.setTextureRect(sf::IntRect(896, 1696, TILE_SIZE * 3, TILE_SIZE));
    _shadowSprite.setOrigin((float)TILE_SIZE * 3 / 2, 0);
    _shadowSprite.setPosition(getPosition());
}