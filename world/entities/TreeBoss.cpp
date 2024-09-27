#include "TreeBoss.h"
#include "../../core/Util.h"
#include "../World.h"

TreeBoss::TreeBoss(sf::Vector2f pos) : Boss(TREE_BOSS, pos, 1, TILE_SIZE * 3, TILE_SIZE * 5,
    {
        BossState(BEHAVIOR_STATE::CHASE, 2000LL, 2500LL),
        BossState(BEHAVIOR_STATE::SHOOT_LOGS_0, 5000LL, 5000LL),
        BossState(BEHAVIOR_STATE::SHOOT_LOGS_1, 5000LL, 5000LL)
    }) 
{
    setMaxHitPoints(475);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 3) / 2;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 3;
    _hitBox.height = TILE_SIZE * 5;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;

    _entityType = "treeboss";
    _displayName = "Phillip McTree";

    srand(currentTimeNano());
    const unsigned int pennyAmount = randomInt(15000, 20000);
    getInventory().addItem(Item::PENNY.getId(), pennyAmount);

    _animSpeed = 1;
}

void TreeBoss::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 5;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 2));

        int xOffset = ((_numSteps >> 3) & 1) * TILE_SIZE;

        _wavesSprite.setTextureRect(sf::IntRect(0 + xOffset, 160, TILE_SIZE, TILE_SIZE));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - (float)TILE_SIZE * 3.f / 2.f + 16, getPosition().y + (TILE_SIZE * 5 - 8)));
        surface.draw(_wavesSprite);
    }

    int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 7) * TILE_SIZE * 5 : 0;

    _sprite.setTextureRect(sf::IntRect(
        1504, 640 + yOffset, TILE_SIZE * 3, isSwimming() ? TILE_SIZE * 3 : TILE_SIZE * 5
    ));

    surface.draw(_sprite);
}

void TreeBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(1504, 640, TILE_SIZE * 3, TILE_SIZE * 5));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 3 / 2, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 10, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}

void TreeBoss::subUpdate() {
    _sprite.setPosition(getPosition());

    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE * 3;
        _hitBox.height = TILE_SIZE * 3;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 5;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void TreeBoss::onStateChange(const BossState previousState, const BossState newState) {
    if (newState.stateId == CHASE) _baseSpeed = 3.f;
    else _baseSpeed = 1.f;
}

void TreeBoss::runCurrentState() {
    switch (_currentState.stateId) {
        case CHASE:
        {
            sf::Vector2f goalPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
            sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 5);

            float xa = 0.f, ya = 0.f;

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

            if (_spawnedWithEnemies) hoardMove(xa, ya, true, 50);
            else move(xa, ya);

            if (_world->getPlayer()->getHitBox().intersects(getHitBox())) {
                _world->getPlayer()->takeDamage(15);
                _world->getPlayer()->knockBack(15, getMovingDir());
            }
            break;
        }
        case SHOOT_LOGS_0:
        {
            if (currentTimeMillis() - _lastFireTimeMillis >= _fireRateMillis) {
                float angle = 45.f / 2.f;
                int firePoints = 360 / 45;
                for (int i = 0; i < firePoints; i++) {
                    angle += (float)i * 45.f;
                    if (angle >= 360.f) angle -= 360.f;

                    fireTargetedProjectile(degToRads(angle), ProjectileDataManager::getData("_PROJECTILE_TREEBOSS_LOG"), "NONE", true, false, { TILE_SIZE / 2, 0 });
                    _lastFireTimeMillis = currentTimeMillis();
                }
            }

            sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
            sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 5);

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

            if (_spawnedWithEnemies) hoardMove(xa, ya, true, 50);
            else move(xa, ya);

            constexpr long long contactDamageRateMillis = 500LL;
            if (getWorld()->getPlayer()->isActive()
                && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())
                && currentTimeMillis() - _lastContactDamageTimeMillis >= contactDamageRateMillis) {
                getWorld()->getPlayer()->takeDamage(10);
                _lastContactDamageTimeMillis = currentTimeMillis();
            }
            break;
        }
        case SHOOT_LOGS_1:
        {
            if (currentTimeMillis() - _lastFireTimeMillis >= _fireRateMillis) {
                float angle = 45.f / 2.f;
                int firePoints = 360 / 45;
                for (int i = 0; i < firePoints; i++) {
                    angle += (float)i * 45.f;
                    if (angle >= 360.f) angle -= 360.f;

                    fireTargetedProjectile(degToRads(angle), ProjectileDataManager::getData("_PROJECTILE_TREEBOSS_LOG"), "NONE", true, false, {TILE_SIZE / 2, 0});
                    _lastFireTimeMillis = currentTimeMillis();
                }
            }

            sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
            sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 5);

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

            if (_spawnedWithEnemies) hoardMove(xa, ya, true, 50);
            else move(xa, ya);

            constexpr long long contactDamageRateMillis = 500LL;
            if (getWorld()->getPlayer()->isActive()
                && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())
                && currentTimeMillis() - _lastContactDamageTimeMillis >= contactDamageRateMillis) {
                getWorld()->getPlayer()->takeDamage(10);
                _lastContactDamageTimeMillis = currentTimeMillis();
            }
            break;
        }
    }
}
