#include "ChefBoss.h"
#include "../../core/Util.h"
#include "orbiters/Orbiter.h"
#include "../World.h"

ChefBoss::ChefBoss(sf::Vector2f pos) : Boss(CHEF_BOSS, pos, 3.f, TILE_SIZE * 2, TILE_SIZE * 4, 
    {
        BossState(BEHAVIOR_STATE::REST, 1000LL, 2000LL),
        BossState(BEHAVIOR_STATE::PIZZA_RING, 8000LL, 8000LL),
        BossState(BEHAVIOR_STATE::KNIFE_ATTACK, 6000LL, 9000LL)
    }) {
    setMaxHitPoints(14500);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 2) / 2;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 2;
    _hitBox.height = TILE_SIZE * 3;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;

    _entityType = "chefboss";
    _displayName = "Chef Pete";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(2300, 3000);
    getInventory().addItem(Item::PENNY.getId(), pennyAmount);
}

void ChefBoss::subUpdate() {
    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 2);

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

    if (dist - desiredDist < 10.f) _baseSpeed = 1.f;
    else _baseSpeed = 3.f;

    if (dist < desiredDist || _currentState.stateId == KNIFE_ATTACK) {
        xa = 0;
        ya = 0;

        if (_movingDir == LEFT) _movingDir = DOWN;
        else if (_movingDir == DOWN) _movingDir = LEFT;
    }

    if (_spawnedWithEnemies) hoardMove(xa, ya, true, 128);
    else move(xa, ya);

    _sprite.setPosition(getPosition());
    _tearsSprite.setPosition(getPosition());
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    constexpr long long contactDamageRateMillis = 500LL;
    if (getWorld()->getPlayer()->isActive()
        && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())
        && currentTimeMillis() - _lastContactDamageTimeMillis >= contactDamageRateMillis) {
        getWorld()->getPlayer()->takeDamage(50);
        _lastContactDamageTimeMillis = currentTimeMillis();
    }

    _animCounter++;
}

void ChefBoss::draw(sf::RenderTexture& surface) {
    constexpr int ticksPerFrame = 4;
    constexpr int frameCount = 26;
    const int xOffset = ((_animCounter / ticksPerFrame) % frameCount) * TILE_SIZE * 2;
    _tearsSprite.setTextureRect(sf::IntRect(
        960 + xOffset, 1728, TILE_SIZE * 2, TILE_SIZE * 4
    ));

    surface.draw(_sprite);
    surface.draw(_tearsSprite);
}

void ChefBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(928, 1728, TILE_SIZE * 2, TILE_SIZE * 4));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 2 / 2, 0);

    _tearsSprite.setTexture(*spriteSheet);
    _tearsSprite.setTextureRect(sf::IntRect(960, 1728, TILE_SIZE * 2, TILE_SIZE * 4));
    _tearsSprite.setPosition(getPosition());
    _tearsSprite.setOrigin((float)TILE_SIZE * 2 / 2, 0);
}

void ChefBoss::onStateChange(const BossState previousState, const BossState newState) {
    if (newState.stateId == PIZZA_RING) {
        _orbiterCount = 0;
    }
}

void ChefBoss::runCurrentState() {
    switch (_currentState.stateId) {
        case PIZZA_RING:
        {
            constexpr int pizzaCount = 16;
            constexpr long long orbiterSpawnRateMillis = 250LL;

            if (_orbiterCount < pizzaCount && currentTimeMillis() - _lastOrbiterSpawnTime >= orbiterSpawnRateMillis) {
                std::shared_ptr<Orbiter> orbiter = std::shared_ptr<Orbiter>(new Orbiter(90.f, OrbiterType::PIZZA_CHEFBOSS.getId(), getWorld()->getPlayer().get()));
                orbiter->setAttackFrequencyOffset(((long long)((pizzaCount - 1)) - _orbiterCount) * orbiterSpawnRateMillis);

                orbiter->loadSprite(getWorld()->getSpriteSheet());
                orbiter->setWorld(getWorld());
                getWorld()->addEntity(orbiter);

                _orbiterCount++;
                _lastOrbiterSpawnTime = currentTimeMillis();
            }
            break;
        }
        case KNIFE_ATTACK:
        {
            constexpr long long fireRateMillis = 128LL;
            _fireAngle++;
            if (_fireAngle > 360.f) _fireAngle = 0;

            if (currentTimeMillis() - _lastFireTimeMillis >= fireRateMillis) {
                constexpr int dirNumber = 6;
                for (int i = 0; i < dirNumber; i++) {
                    float currentAngle = _fireAngle + (360.f / dirNumber) * i;
                    if (currentAngle >= 360.f) currentAngle -= 360.f;

                    float fireAngleRads = currentAngle * ((float)PI / 180.f);
                    fireTargetedProjectile(fireAngleRads, ProjectileDataManager::getData("_PROJECTILE_CHEFBOSS_KNIFE"), "NONE", true, false, {8, 0});
                }
                _lastFireTimeMillis = currentTimeMillis();
            }
            break;
        }
    }
}
