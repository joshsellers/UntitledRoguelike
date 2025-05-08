#include "Entity.h"
#include <boost/random/uniform_int_distribution.hpp>
#include "../World.h"
#include "DamageParticle.h"
#include "projectiles/Projectile.h"
#include "../../core/SoundManager.h"
#include "../../statistics/StatManager.h"
#include "projectiles/ProjectilePoolManager.h"
#include "../../inventory/abilities/AbilityManager.h"
#include "../../inventory/abilities/Ability.h"

Entity::Entity(ENTITY_SAVE_ID saveId, sf::Vector2f pos, float baseSpeed, const int spriteWidth, const int spriteHeight, const bool isProp) : 
    _spriteWidth(spriteWidth), _spriteHeight(spriteHeight), _isProp(isProp), _saveId(saveId) {

    _pos = _isProp ? sf::Vector2f(pos.x - (_spriteWidth * TILE_SIZE) / 2, pos.y - (_spriteHeight * TILE_SIZE)) : pos;
    _baseSpeed = baseSpeed;

    _wanderTargetPos = _pos;
                                                          
    if (saveId == PLAYER) _uid = "abadbabe-beef-beef-beef-abadbabed00d";
    else _uid = generateUID();
}

void Entity::move(float xa, float ya) {
    xa *= _baseSpeed;
    ya *= _baseSpeed;

    if (isSwimming() && !_isEnemy) {
        xa /= 2;
        ya /= 2;
    }

    if (std::abs(xa) > 0 || std::abs(ya) > 0) {
        _numSteps++;
        _isMoving = true;
    } else if (isSwimming()) {
        _numSteps++;
        _isMoving = false;
    } else _isMoving = false;

    _pos.x += xa;
    _pos.y += ya;
    _velocity.x = xa;
    _velocity.y = ya;
}

void Entity::hoardMove(float xa, float ya, bool sameTypeOnly, float minDist, float visionRange) {
    if (_isScared) {
        constexpr int fearTime = 60 * 5;
        if (_fearTimer >= fearTime) _isScared = false;
        _fearTimer++;
        return;
    }

    if (_isEnemy && !_checkedFear && AbilityManager::playerHasAbility(Ability::FEAR.getId())) {
        const float distSquared = 150 * 150;
        const sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);
        const sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + getSpriteSize().y / 2);
        const float dist = std::pow(playerPos.x - cLoc.x, 2) + std::pow(playerPos.y - cLoc.y, 2);

        if (dist <= distSquared) {
            _checkedFear = true;
            const float chance = AbilityManager::getParameter(Ability::FEAR.getId(), "chance");
            if (chance >= 1.f) {
                _isScared = true;
                return;
            }
            _isScared = randomChance(chance);
            if (_isScared) return;
        }
    }

    sf::Vector2f acceleration(xa, ya);

    sf::Vector2f visionSum(0, 0);
    int visionCount = 0;

    acceleration += separate(acceleration, sameTypeOnly, minDist, visionRange, visionCount, visionSum);
    acceleration += align(sameTypeOnly, visionCount, visionSum);
    acceleration += cohesion(acceleration, sameTypeOnly, visionCount, visionSum);

    acceleration.x *= 0.4f;
    acceleration.y *= 0.4f;
    
    _velocity += acceleration;

    float size = sqrt(_velocity.x * _velocity.x + _velocity.y * _velocity.y);
    if (size > _baseSpeed) {
        _velocity.x /= size;
        _velocity.y /= size;
    }

    if (isSwimming() && !_isEnemy) {
        _velocity.x /= 2.f;
        _velocity.y /= 2.f;
    }

    _pos += _velocity;

    if (std::abs(_velocity.x) > 0 || std::abs(_velocity.y) > 0) {
        _numSteps++;
        _isMoving = true;
    } else if (isSwimming()) {
        _numSteps++;
        _isMoving = false;
    } else _isMoving = false;

    if (std::abs(_velocity.x) < std::abs(_velocity.y)) {
        if (_velocity.y < 0) _movingDir = UP;
        else if (_velocity.y > 0) _movingDir = DOWN;
    } else if (std::abs(_velocity.x) > std::abs(_velocity.y)) {
        if (_velocity.x < 0) _movingDir = LEFT;
        else if (_velocity.x > 0) _movingDir = RIGHT;
    }
}

const sf::Vector2f Entity::separate(sf::Vector2f acceleration, bool sameTypeOnly, float minDist, float visionRange, int& visionCount, sf::Vector2f& visionSum) {
    minDist *= minDist;

    sf::Vector2f steer(0, 0);
    int count = 0;
    for (auto& entity : _world->getEnemies()) {
        if (entity->isActive() && !entity->compare(this) 
            && (!sameTypeOnly || entity->getEntityType() == getEntityType())) {
            float dx = getPosition().x - entity->getPosition().x;
            float dy = getPosition().y - entity->getPosition().y;
            float dist = dx * dx + dy * dy;

            if (dist > 0 && dist < minDist) {
                count++;

                sf::Vector2f diff(getPosition().x - entity->getPosition().x, getPosition().y - entity->getPosition().y);
                float magnitude = sqrt(diff.x * diff.x + diff.y * diff.y);

                // normalize 
                if (magnitude > 0) {
                    diff.x /= magnitude;
                    diff.y /= magnitude;
                }

                // divScalar
                diff.x /= dist;
                diff.y /= dist;

                steer += diff;
            }

            if (dist < visionRange) {
                visionCount++;
                visionSum += entity->getVelocity();
            }
        }
    }

    if (count > 0) {
        steer.x /= (float) count;
        steer.y /= (float) count;
    }
    float steerMagnitude = sqrt(steer.x * steer.x + steer.y * steer.y);
    if (steerMagnitude > 0) {
        steer.x /= steerMagnitude;
        steer.y /= steerMagnitude;

        steer.x *= _baseSpeed;
        steer.y *= _baseSpeed;

        steer -= _velocity;

        float size = steerMagnitude;
        float max = 0.5f;
        if (size > max) {
            steer.x /= size;
            steer.y /= size;
        }
    }

    return steer;
}

const sf::Vector2f Entity::align(bool sameTypeOnly, int& visionCount, sf::Vector2f& visionSum) {
    if (visionCount > 0) {
        visionSum.x /= (float)visionCount;
        visionSum.y /= (float)visionCount;

        float sumMagnitude = sqrt(visionSum.x * visionSum.x + visionSum.y * visionSum.y);
        if (sumMagnitude > 0) {
            visionSum.x /= sumMagnitude;
            visionSum.y /= sumMagnitude;
        }
        
        visionSum.x *= _baseSpeed;
        visionSum.y *= _baseSpeed;

        sf::Vector2f steer(0, 0);
        steer = visionSum - _velocity;

        float size = sqrt(steer.x * steer.x + steer.y * steer.y);
        float max = 0.5f;
        if (size > max) {
            steer.x /= size;
            steer.y /= size;
        }

        return steer;
    } else {
        return sf::Vector2f(0, 0);
    }
}

const sf::Vector2f Entity::cohesion(sf::Vector2f acceleration, bool sameTypeOnly, int& visionCount, sf::Vector2f& visionSum) {
    if (visionCount > 0) {
        visionSum.x /= (float)visionCount;
        visionSum.y /= (float)visionCount;

        // seek
        sf::Vector2f desired(0, 0);
        desired -= visionSum;

        float magnitude = sqrt(desired.x * desired.x + desired.y * desired.y);

        // normalize 
        if (magnitude > 0) {
            desired.x /= magnitude;
            desired.y /= magnitude;
        }

        desired.x *= _baseSpeed;
        desired.y *= _baseSpeed;

        //acceleration = desired - _velocity;

        float size = sqrt(acceleration.x * acceleration.x + acceleration.y * acceleration.y);
        float max = 0.5f;
        if (size > max) {
            acceleration.x /= size;
            acceleration.y /= size;
        }

        return acceleration;
    } else {
        return sf::Vector2f(0, 0);
    }
}

void Entity::wander(sf::Vector2f feetPos, boost::random::mt19937& generator, int movementChance, int maxDistance) {
    if (_world != nullptr) {
        float xa = 0, ya = 0;

        boost::random::uniform_int_distribution<> shouldMove(0, movementChance);
        if (_wanderTargetPos == feetPos && shouldMove(generator) == 0 ||
            _world->getTerrainDataAt(_wanderTargetPos) == TERRAIN_TYPE::WATER) {
            const int maxDist = _world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER ? 200 : maxDistance;
            boost::random::uniform_int_distribution<> xDist(-maxDist, maxDist);
            boost::random::uniform_int_distribution<> yDist(-maxDist, maxDist);
            _wanderTargetPos.x = feetPos.x + xDist(generator);
            _wanderTargetPos.y = feetPos.y + yDist(generator);
        } else if (feetPos.x < _wanderTargetPos.x) {
            if (_world->getTerrainDataAt(feetPos.x + getBaseSpeed(), feetPos.y) == TERRAIN_TYPE::WATER &&
                _world->getTerrainDataAt(feetPos) != TERRAIN_TYPE::WATER) {
                _wanderTargetPos = feetPos;
            } else {
                if (_world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER) _wanderTargetPos == feetPos;
                xa += getBaseSpeed();
                _movingDir = RIGHT;
            }
        } else if (feetPos.x > _wanderTargetPos.x) {
            if (_world->getTerrainDataAt(feetPos.x - getBaseSpeed(), feetPos.y) == TERRAIN_TYPE::WATER &&
                _world->getTerrainDataAt(feetPos) != TERRAIN_TYPE::WATER) {
                _wanderTargetPos = feetPos;
            } else {
                if (_world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER) _wanderTargetPos == feetPos;
                xa -= getBaseSpeed();
                _movingDir = LEFT;
            }
        } else if (feetPos.y < _wanderTargetPos.y) {
            if (_world->getTerrainDataAt(feetPos.x, feetPos.y + getBaseSpeed()) == TERRAIN_TYPE::WATER &&
                _world->getTerrainDataAt(feetPos) != TERRAIN_TYPE::WATER) {
                _wanderTargetPos = feetPos;
            } else {
                if (_world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER) _wanderTargetPos == feetPos;
                ya += getBaseSpeed();
                _movingDir = DOWN;
            }
        } else if (feetPos.y > _wanderTargetPos.y) {
            if (_world->getTerrainDataAt(feetPos.x, feetPos.y - getBaseSpeed()) == TERRAIN_TYPE::WATER &&
                _world->getTerrainDataAt(feetPos) != TERRAIN_TYPE::WATER) {
                _wanderTargetPos = feetPos;
            } else {
                if (_world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER) _wanderTargetPos == feetPos;
                ya -= getBaseSpeed();
                _movingDir = UP;
            }
        }

        move(xa, ya);
    }
}

void Entity::swimWander(sf::Vector2f feetPos, boost::random::mt19937& generator, int movementChance) {
    if (_world != nullptr) {
        float xa = 0, ya = 0;

        boost::random::uniform_int_distribution<> shouldMove(0, movementChance);
        if (_wanderTargetPos == feetPos && shouldMove(generator) == 0 ||
            _world->getTerrainDataAt(_wanderTargetPos) != TERRAIN_TYPE::WATER) {
            const int maxDist = _world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER ? 200 : 100;
            boost::random::uniform_int_distribution<> xDist(-maxDist, maxDist);
            boost::random::uniform_int_distribution<> yDist(-maxDist, maxDist);
            _wanderTargetPos.x = feetPos.x + xDist(generator);
            _wanderTargetPos.y = feetPos.y + yDist(generator);
        } else if (feetPos.x < _wanderTargetPos.x) {
            if (_world->getTerrainDataAt(feetPos.x + getBaseSpeed(), feetPos.y) != TERRAIN_TYPE::WATER &&
                _world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER) {
                _wanderTargetPos = feetPos;
            } else {
                if (_world->getTerrainDataAt(feetPos) != TERRAIN_TYPE::WATER) _wanderTargetPos == feetPos;
                xa += getBaseSpeed();
                _movingDir = RIGHT;
            }
        } else if (feetPos.x > _wanderTargetPos.x) {
            if (_world->getTerrainDataAt(feetPos.x - getBaseSpeed(), feetPos.y) != TERRAIN_TYPE::WATER &&
                _world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER) {
                _wanderTargetPos = feetPos;
            } else {
                if (_world->getTerrainDataAt(feetPos) != TERRAIN_TYPE::WATER) _wanderTargetPos == feetPos;
                xa -= getBaseSpeed();
                _movingDir = LEFT;
            }
        } else if (feetPos.y < _wanderTargetPos.y) {
            if (_world->getTerrainDataAt(feetPos.x, feetPos.y + getBaseSpeed()) != TERRAIN_TYPE::WATER &&
                _world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER) {
                _wanderTargetPos = feetPos;
            } else {
                if (_world->getTerrainDataAt(feetPos) != TERRAIN_TYPE::WATER) _wanderTargetPos == feetPos;
                ya += getBaseSpeed();
                _movingDir = DOWN;
            }
        } else if (feetPos.y > _wanderTargetPos.y) {
            if (_world->getTerrainDataAt(feetPos.x, feetPos.y - getBaseSpeed()) != TERRAIN_TYPE::WATER &&
                _world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER) {
                _wanderTargetPos = feetPos;
            } else {
                if (_world->getTerrainDataAt(feetPos) != TERRAIN_TYPE::WATER) _wanderTargetPos == feetPos;
                ya -= getBaseSpeed();
                _movingDir = UP;
            }
        }

        move(xa, ya);
    }
}

Projectile* Entity::fireTargetedProjectile(sf::Vector2f targetPos, const ProjectileData projData, std::string soundName, bool onlyDamagePlayer, 
    bool displayProjectileOnTop, sf::Vector2f centerOffset, bool addParentVelocity, bool fireIfScared) {
    const sf::Vector2f centerPoint(getPosition().x, getPosition().y + _spriteHeight / 2);

    double x = (double)(targetPos.x - (centerPoint.x + centerOffset.x));
    double y = (double)(targetPos.y - (centerPoint.y + centerOffset.y));

    float angle = (float)((std::atan2(y, x)));
    return fireTargetedProjectile(angle, projData, soundName, onlyDamagePlayer, displayProjectileOnTop, centerOffset, addParentVelocity, fireIfScared);
}

/**
* THIS CAN RETURN NULLPTR
* Always make sure Projectile* is not nullptr before doing anything with it
* why do I do things like this
*/
Projectile* Entity::fireTargetedProjectile(float angle, const ProjectileData projData, std::string soundName, bool onlyDamagePlayer, 
    bool displayProjectileOnTop, sf::Vector2f centerOffset, bool addParentVelocity, bool fireIfScared) {
    if (_isScared && !fireIfScared) return nullptr;

    const sf::Vector2f centerPoint(getPosition().x, getPosition().y + _spriteHeight / 2);
    sf::Vector2f spawnPos(centerPoint.x - TILE_SIZE / 2 + centerOffset.x, centerPoint.y + centerOffset.y);

    Projectile* projectile = ProjectilePoolManager::addProjectile(spawnPos, this, angle, projData.baseVelocity, projData, onlyDamagePlayer, 0, addParentVelocity);

    if (soundName != "NONE") SoundManager::playSound(soundName);
    return projectile;
}

bool Entity::isMoving() const {
    return _isMoving;
}

MOVING_DIRECTION Entity::getMovingDir() const {
    return (MOVING_DIRECTION)_movingDir;
}

bool Entity::isSwimming() const {
    return _isSwimming;
}

void Entity::setBaseSpeed(float speed) {
    _baseSpeed = speed;
}

float Entity::getBaseSpeed() const {
    return _baseSpeed;
}

sf::Vector2f Entity::getPosition() const {
    return _pos;
}

sf::Vector2f Entity::getVelocity() const {
    return _velocity;
}

bool Entity::isMob() const {
    return _isMob;
}

bool Entity::isProp() const {
    return _isProp;
}

bool Entity::isEnemy() const {
    return _isEnemy;
}

bool Entity::isBoss() const {
    return _isBoss;
}

bool Entity::isMiniBoss() const {
    return _isMiniboss;
}

bool Entity::isOrbiter() const {
    return _isOrbiter;
}

bool Entity::canPickUpItems() const {
    return _canPickUpItems;
}

bool Entity::usesDormancyRules() const {
    return _usesDormancyRules;
}

void Entity::shouldUseDormancyRules(bool usesDormancyRules) {
    _usesDormancyRules = usesDormancyRules;
}

void Entity::setDormancyTimeout(int dormancyTimeout) {
    _dormancyTimeout = dormancyTimeout;
}

void Entity::setMaxTimeOutOfChunk(int maxTimeOutOfChunk) {
    _maxTimeOutOfChunk = maxTimeOutOfChunk;
}

bool Entity::isInitiallyDocile() const {
    return _isInitiallyDocile;
}

bool Entity::isHostile() const {
    return _isHostile;
}

sf::Sprite Entity::getSprite() const {
    return _sprite;
}

bool Entity::displayBottom() const {
    return _displayBottom;
}

bool Entity::displayOnTop() const {
    return _displayOnTop;
}

sf::Vector2i Entity::getSpriteSize() const {
    return sf::Vector2i(_spriteWidth, _spriteHeight);
}

Inventory& Entity::getInventory() {
    return _inventory;
}

void Entity::setWorld(World* world) {
    _world = world;
}

World* Entity::getWorld() const {
    return _world;
}

bool Entity::isActive() const {
    return _isActive;
}

void Entity::deactivate() {
    _isActive = false;
}

void Entity::activate() {
    _isActive = true;
}

bool Entity::isDormant() const {
    return _isDormant;
}

void Entity::setDormant(bool dormant) {
    _isDormant = dormant;
}

int Entity::getDormancyTimeout() const {
    return _dormancyTimeout;
}

int Entity::getDormancyTime() const {
    return _dormancyTimer;
}

void Entity::resetDormancyTimer() {
    _dormancyTimer = 0;
}

void Entity::incrementDormancyTimer() {
    _dormancyTimer++;
}

int Entity::getMaxTimeOutOfChunk() const {
    return _maxTimeOutOfChunk;
}

int Entity::getTimeOutOfChunk() const {
    return _outOfChunkTimer;
}

void Entity::resetOutOfChunkTimer() {
    _outOfChunkTimer = 0;
}

void Entity::incrementOutOfChunkTimer() {
    _outOfChunkTimer++;
}

bool Entity::isDamageable() const {
    return getMaxHitPoints() > 0;
}

void Entity::knockBack(float amt, MOVING_DIRECTION dir) {
    switch (dir) {
        case UP:
            move(0, -amt);
            break;
        case DOWN:
            move(0, amt);
            break;
        case LEFT:
            move(-amt, 0);
            break;
        case RIGHT:
            move(amt, 0);
            break;
    }
}

void Entity::setMaxHitPoints(int maxHitPoints) {
    _maxHitPoints = maxHitPoints;
}

int Entity::getMaxHitPoints() const {
    return _maxHitPoints;
}

int& Entity::getMaxHitPointsRef() {
    return _maxHitPoints;
}

void Entity::takeDamage(int damage, bool crit) {
    if (getEntityType() != "player" && getEntityType() != "shopext" && getEntityType() != "shopint" && getEntityType() != "barberext" && getEntityType() != "barberint" 
        && getEntityType() != "shopkeep") {
        sf::Vector2f pos = getPosition();
        if (_isProp) pos.x += (float)(_spriteWidth * TILE_SIZE) / 2;

        std::shared_ptr<DamageParticle> damageParticle = std::shared_ptr<DamageParticle>(new DamageParticle(pos, damage, crit));
        damageParticle->setWorld(getWorld());
        damageParticle->loadSprite(getWorld()->getSpriteSheet());
        getWorld()->addEntity(damageParticle);
    }
    this->damage(damage);

    if (_isEnemy && _hitPoints <= 0) StatManager::increaseStat(ENEMIES_DEFEATED, 1.f);
}

void Entity::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
    }
}

void Entity::heal(int hitPoints) {
    if (_hitPoints + hitPoints > getMaxHitPoints())
        hitPoints -= _hitPoints + hitPoints - getMaxHitPoints();
    _hitPoints += hitPoints;
}

int Entity::getHitPoints() const {
    return _hitPoints;
}

int& Entity::getHitPointsRef() {
    return _hitPoints;
}

int Entity::getStamina() const {
    return 0;
}

int Entity::getMaxStamina() const {
    return 0;
}

void Entity::setMaxStamina(int amount) {
}

void Entity::restoreStamina(int amount) {
}

float Entity::getDamageMultiplier() const {
    return _damageMultiplier;
}

void Entity::increaseDamageMultiplier(float amount) {
    _damageMultiplier += amount;
}

void Entity::setDamageMultiplier(float amount) {
    _damageMultiplier = amount;
}

void Entity::increaseStaminaRefreshRate(int amount) {
}

void Entity::setStaminaRefreshRate(int amount) {
}

int Entity::getStaminaRefreshRate() const {
    return 0;
}

sf::FloatRect Entity::getHitBox() const {
    return _hitBox;
}

bool Entity::hasColliders() const {
    return _hasColliders;
}

std::vector<sf::FloatRect> Entity::getColliders() const {
    return _colliders;
}

sf::Vector2f Entity::getCalculatedBarrelPos() const {
    return _calculatedBarrelPos;
}

sf::Vector2f Entity::getTargetPos() const {
    return _targetPos;
}

bool Entity::compare(Entity* entity) const {
    return entity->_hitBox == _hitBox;
}

const std::string& Entity::getEntityType() const {
    return _entityType;
}

const std::string& Entity::getDisplayName() const {
    return _displayName;
}

unsigned int Entity::getMagazineAmmoType() const {
    return _magazineAmmoType;
}

int& Entity::getMagazineSize() {
    return _magazineSize;
}

int& Entity::getMagazineContents() {
    return _magazineContents;
}

void Entity::decrementMagazine() {
    if (_magazineContents != 0) _magazineContents--;
}

void Entity::emptyMagazine() {
    _magazineContents = 0;
}

bool Entity::isReloading() const {
    return _isReloading;
}

ENTITY_SAVE_ID Entity::getSaveId() const {
    return _saveId;
}

std::string Entity::getSaveData() const {
    return "NONE";
}

std::string Entity::getUID() const {
    return _uid;
}

void Entity::setUID(std::string uuid) {
    _uid = uuid;
}

