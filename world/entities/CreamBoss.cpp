#include "CreamBoss.h"
#include "../../core/Util.h"
#include "../World.h"
#include "CreamDropper.h"
#include "orbiters/Orbiter.h"

CreamBoss::CreamBoss(sf::Vector2f pos) : Boss(CREAM_BOSS, pos, 1, TILE_SIZE * 8, TILE_SIZE * 6,
    {
        BossState(BEHAVIOR_STATE::REST, 1000LL, 200LL),
        BossState(BEHAVIOR_STATE::CREAM_DROP, 11000LL, 12000LL),
        BossState(BEHAVIOR_STATE::CREAM_RING_IN, 8000LL, 8000LL),
        BossState(BEHAVIOR_STATE::CREAM_RING_OUT, 8000LL, 8000LL)
    }) {
    setMaxHitPoints(10000);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 8) / 2;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 8;
    _hitBox.height = TILE_SIZE * 6;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;

    _entityType = "creamboss";
    _displayName = "Mr. Cream";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(150000, 175000);
    getInventory().addItem(Item::PENNY.getId(), pennyAmount);

    _animSpeed = 1;
}

void CreamBoss::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 6;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 4));

        int yOffset = ((_numSteps >> 3) & 1) * TILE_SIZE;

        _wavesSprite.setTextureRect(sf::IntRect(400, 688 + yOffset, TILE_SIZE * 5, TILE_SIZE));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - (float)TILE_SIZE * 8.f / 2.f + 10.f, getPosition().y + (TILE_SIZE * 6) - 16));
        surface.draw(_wavesSprite);
    }

    int xOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 7) * TILE_SIZE * 8 : 0;

    _sprite.setTextureRect(sf::IntRect(
        400 + xOffset, 1552, TILE_SIZE * 8, isSwimming() ? TILE_SIZE * 4 : TILE_SIZE * 6
    ));

    surface.draw(_sprite);
}

void CreamBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(400, 1552, TILE_SIZE * 8, TILE_SIZE * 6));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 8 / 2, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}

void CreamBoss::subUpdate() {
    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 6);

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
        _hitBoxYOffset = TILE_SIZE * 4;
        _hitBox.height = TILE_SIZE * 4;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 6;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    constexpr long long contactDamageRateMillis = 500LL;
    if (getWorld()->getPlayer()->isActive()
        && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())
        && currentTimeMillis() - _lastContactDamageTimeMillis >= contactDamageRateMillis) {
        getWorld()->getPlayer()->takeDamage(25);
        _lastContactDamageTimeMillis = currentTimeMillis();
    }
}

void CreamBoss::onStateChange(const BossState previousState, const BossState newState) {
    if (newState.stateId == CREAM_DROP) {
        const unsigned int dropCount = randomInt(5, 10);
        auto dropper = std::shared_ptr<CreamDropper>(new CreamDropper(getPosition(), this, dropCount));
        dropper->loadSprite(getWorld()->getSpriteSheet());
        dropper->setWorld(getWorld());
        getWorld()->addEntity(dropper);
    }

    if (previousState.stateId == CREAM_RING_IN || previousState.stateId == CREAM_RING_OUT) {
        _currentLayerCount = 0;
    }
}

void CreamBoss::runCurrentState() {
    constexpr long long layerSpawnRate = 500LL;

    if (_currentLayerCount < _maxLayers && currentTimeMillis() - _lastLayerSpawnTime >= layerSpawnRate) {
        constexpr float layerDist = 100.f;
        constexpr int orbiterCount = 8;

        switch (_currentState.stateId) {
            case CREAM_RING_OUT:
            {
                float angle = 0;
                for (int i = 0; i < orbiterCount * (_currentLayerCount + 1); i++) {
                    angle += 360.f / (orbiterCount * (_currentLayerCount + 1));
                    if (angle >= 360.f) angle -= 360.f;

                    unsigned int oId = OrbiterType::CREAM_CLOCKWISE_OUT.getId();
                    if (_currentLayerCount % 2 == 0) oId = OrbiterType::CREAM_COUNTERCLOCKWISE_OUT.getId();

                    auto orbiter = std::shared_ptr<Orbiter>(new Orbiter(angle, oId, this));
                    orbiter->setDistance(layerDist * (_currentLayerCount + 1));
                    orbiter->setCenterPointOffset(0, (float)TILE_SIZE * 3);

                    orbiter->loadSprite(getWorld()->getSpriteSheet());
                    orbiter->setWorld(getWorld());
                    getWorld()->addEntity(orbiter);
                }

                _currentLayerCount++;
                _lastLayerSpawnTime = currentTimeMillis();
                break;
            }

            case CREAM_RING_IN:
            {
                float angle = 0;
                for (int i = 0; i < orbiterCount * (_currentLayerCount + 1); i++) {
                    angle += 360.f / (orbiterCount * (_currentLayerCount + 1));
                    if (angle >= 360.f) angle -= 360.f;

                    unsigned int oId = OrbiterType::CREAM_CLOCKWISE_IN.getId();
                    if (_currentLayerCount % 2 == 0) oId = OrbiterType::CREAM_COUNTERCLOCKWISE_IN.getId();

                    auto orbiter = std::shared_ptr<Orbiter>(new Orbiter(angle, oId, this));
                    orbiter->setDistance(layerDist * (_currentLayerCount + 1)); 
                    orbiter->setCenterPointOffset(0, (float)TILE_SIZE * 3);

                    orbiter->loadSprite(getWorld()->getSpriteSheet());
                    orbiter->setWorld(getWorld());
                    getWorld()->addEntity(orbiter);
                }

                _currentLayerCount++;
                _lastLayerSpawnTime = currentTimeMillis();
                break;
            }
        }
    }
}
