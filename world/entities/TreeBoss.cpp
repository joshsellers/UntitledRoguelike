#include "TreeBoss.h"
#include "../../core/Util.h"
#include "../World.h"
#include "../../statistics/AchievementManager.h"
#include "../../inventory/ConditionalUnlockManager.h"

TreeBoss::TreeBoss(sf::Vector2f pos) : Boss(TREE_BOSS, pos, 1, TILE_SIZE * 3, TILE_SIZE * 5,
    {
        BossState(BEHAVIOR_STATE::CHASE, 2000LL, 2500LL),
        BossState(BEHAVIOR_STATE::SHOOT_LOGS_0, 3000LL, 5000LL)
        //BossState(BEHAVIOR_STATE::SHOOT_LOGS_1, 3000LL, 4000LL)
    }) 
{
    _gen.seed(currentTimeNano());
    setMaxHitPoints(HARD_MODE_ENABLED ? 700 : 475);
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
    const unsigned int pennyAmount = randomInt(150, 200);
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
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - (float)TILE_SIZE * 3.f / 2.f + 16, getPosition().y + (TILE_SIZE * 6 - 8)));
        surface.draw(_wavesSprite);
    }

    int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 7) * TILE_SIZE * 6 : 0;

    _sprite.setTextureRect(sf::IntRect(
        1504, 640 + yOffset, TILE_SIZE * 3, isSwimming() ? TILE_SIZE * 3 : TILE_SIZE * 6
    ));

    surface.draw(_sprite);
}

void TreeBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(1504, 640, TILE_SIZE * 3, TILE_SIZE * 6));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 3 / 2, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 10, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}

void TreeBoss::setWorld(World* world) {
    _world = world;

    _playerIsTree = playerIsWearing("Leaf Hat") && playerIsWearing("Bark Cuirass") && playerIsWearing("Bark Greaves") && playerIsWearing("Bark Sabatons");
    if (_playerIsTree) {
        AchievementManager::unlock(TREECKSTER);
        ConditionalUnlockManager::increaseUnlockProgress("Acorn", 1);
    }
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

    if (_playerIsTree) {
        _playerIsTree = playerIsWearing("Leaf Hat") && playerIsWearing("Bark Cuirass") && playerIsWearing("Bark Greaves") && playerIsWearing("Bark Sabatons");

        const sf::Vector2f feetPos(getPosition().x, getPosition().y + _spriteHeight);
        wander(feetPos, _gen);
        return;
    }

    if (_chaseTarget.x == 0.f && _chaseTarget.y == 0.f) resetChaseTarget();
}

void TreeBoss::onStateChange(const BossState previousState, const BossState newState) {
    if (_playerIsTree) return;

    if (newState.stateId == CHASE) {
        _baseSpeed = 4.5f;
        resetChaseTarget();
    }
    else _baseSpeed = 1.f;
}

void TreeBoss::runCurrentState() {
    if (_playerIsTree) return;

    switch (_currentState.stateId) {
        case CHASE:
        {
            sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 5 / 2);

            const float dist = std::sqrt(std::pow(_chaseTarget.x - cLoc.x, 2) + std::pow(_chaseTarget.y - cLoc.y, 2));
            const float targetDist = _baseSpeed;
            const float distRat = targetDist / dist;

            _pos = sf::Vector2f(((1 - distRat) * cLoc.x + distRat * _chaseTarget.x), ((1 - distRat) * cLoc.y + distRat * _chaseTarget.y) - TILE_SIZE * 5 / 2);

            if (_world->getPlayer()->getHitBox().intersects(getHitBox())) {
                _world->getPlayer()->takeDamage(15);
                _world->getPlayer()->knockBack(15, getMovingDir());
            }

            if (distRat > 1.f) resetChaseTarget();

            _numSteps++;
            _isMoving = true;

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

                    fireTargetedProjectile(degToRads(angle), ProjectileDataManager::getData("_PROJECTILE_TREEBOSS_LOG"), "NONE", true, false, { TILE_SIZE / 2, 16 });
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

void TreeBoss::resetChaseTarget() {
    const sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);
    const sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 5);

    const float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
    const float targetDist = dist + 100.f;
    const float distRat = targetDist / dist;

    _chaseTarget = sf::Vector2f(((1 - distRat) * cLoc.x + distRat * playerPos.x), ((1 - distRat) * cLoc.y + distRat * playerPos.y));
}

bool TreeBoss::playerIsWearing(std::string itemName) {
    const auto& inv = getWorld()->getPlayer()->getInventory();
    const unsigned int itemId = Item::getIdFromName(itemName);
    const int index = inv.findItem(itemId);

    if (index != NO_ITEM) {
        return inv.isEquipped(index);
    }

    return false;
}
