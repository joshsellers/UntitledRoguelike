#include "Entity.h"
#include <boost/random/uniform_int_distribution.hpp>
#include "../World.h"
#include "DamageParticle.h"
#include "projectiles/Projectile.h"
#include "../../core/SoundManager.h"

Entity::Entity(ENTITY_SAVE_ID saveId, sf::Vector2f pos, float baseSpeed, const int spriteWidth, const int spriteHeight, const bool isProp) : 
    _spriteWidth(spriteWidth), _spriteHeight(spriteHeight), _isProp(isProp), _saveId(saveId) {

    _pos = _isProp ? sf::Vector2f(pos.x - (_spriteWidth * TILE_SIZE) / 2, pos.y - (_spriteHeight * TILE_SIZE)) : pos;
    _baseSpeed = baseSpeed;

    _wanderTargetPos = _pos;

    _uid = generateUID();
}

void Entity::move(float xa, float ya) {
    xa *= _baseSpeed;
    ya *= _baseSpeed;

    if (isSwimming()) {
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
    sf::Vector2f acceleration(xa, ya);

    acceleration += separate(acceleration, sameTypeOnly, minDist);
    acceleration += align(sameTypeOnly, visionRange);
    acceleration += cohesion(acceleration, sameTypeOnly, visionRange);

    acceleration.x *= 0.4f;
    acceleration.y *= 0.4f;
    
    _velocity += acceleration;

    float size = sqrt(_velocity.x * _velocity.x + _velocity.y * _velocity.y);
    if (size > _baseSpeed) {
        _velocity.x /= size;
        _velocity.y /= size;
    }

    if (isSwimming()) {
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

const sf::Vector2f Entity::separate(sf::Vector2f acceleration, bool sameTypeOnly, float minDist) {
    sf::Vector2f steer(0, 0);
    int count = 0;
    for (auto& entity : _world->getEnemies()) {
        if (entity->isMob() && entity->isEnemy() && entity->isActive() && !entity->compare(this) && !entity->compare(_world->getPlayer().get())
            && (!sameTypeOnly || entity->getEntityType() == getEntityType())) {
            float dx = getPosition().x - entity->getPosition().x;
            float dy = getPosition().y - entity->getPosition().y;
            float dist = sqrt(dx * dx + dy * dy);

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

const sf::Vector2f Entity::align(bool sameTypeOnly, float visionRange) {
    sf::Vector2f sum(0, 0);
    int count = 0;

    for (auto& entity : _world->getEnemies()) {
        if (entity->isMob() && entity->isEnemy() && entity->isActive() && !entity->compare(this) && !entity->compare(_world->getPlayer().get())
            && (!sameTypeOnly || entity->getEntityType() == getEntityType())) {
            float dx = getPosition().x - entity->getPosition().x;
            float dy = getPosition().y - entity->getPosition().y;
            float dist = sqrt(dx * dx + dy * dy);

            if (dist < visionRange) {
                count++;
                sum += entity->getVelocity();
            }
        }
    }

    if (count > 0) {
        sum.x /= (float)count;
        sum.y /= (float)count;

        float sumMagnitude = sqrt(sum.x * sum.x + sum.y * sum.y);
        if (sumMagnitude > 0) {
            sum.x /= sumMagnitude;
            sum.y /= sumMagnitude;
        }
        
        sum.x *= _baseSpeed;
        sum.y *= _baseSpeed;

        sf::Vector2f steer(0, 0);
        steer = sum - _velocity;

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

const sf::Vector2f Entity::cohesion(sf::Vector2f acceleration, bool sameTypeOnly, float visionRange) {
    sf::Vector2f sum(0, 0);
    int count = 0;

    for (auto& entity : _world->getEnemies()) {
        if (entity->isMob() && entity->isEnemy() && entity->isActive() && !entity->compare(this) && !entity->compare(_world->getPlayer().get())
            && (!sameTypeOnly || entity->getEntityType() == getEntityType())) {
            float dx = getPosition().x - entity->getPosition().x;
            float dy = getPosition().y - entity->getPosition().y;
            float dist = sqrt(dx * dx + dy * dy);

            if (dist < visionRange) {
                count++;
                sum += entity->getPosition();
            }
        }
    }

    if (count > 0) {
        sum.x /= (float) count;
        sum.y /= (float) count;

        // seek
        sf::Vector2f desired(0, 0);
        desired -= sum;

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

void Entity::wander(sf::Vector2f feetPos, boost::random::mt19937& generator, int movementChance) {
    if (_world != nullptr) {
        float xa = 0, ya = 0;

        boost::random::uniform_int_distribution<> shouldMove(0, movementChance);
        if (_wanderTargetPos == feetPos && shouldMove(generator) == 0 ||
            _world->getTerrainDataAt(_wanderTargetPos) == TERRAIN_TYPE::WATER) {
            const int maxDist = _world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER ? 200 : 100;
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

void Entity::fireTargetedProjectile(sf::Vector2f targetPos, const ProjectileData projData, std::string soundName, bool onlyDamagePlayer) {
    const sf::Vector2f centerPoint(getPosition().x - _spriteWidth / 2, getPosition().y + _spriteHeight / 2);
    sf::Vector2f spawnPos(centerPoint.x, centerPoint.y);

    double x = (double)(targetPos.x - centerPoint.x);
    double y = (double)(targetPos.y - centerPoint.y);

    float angle = (float)((std::atan2(y, x)));

    std::shared_ptr<Projectile> proj = std::shared_ptr<Projectile>(new Projectile(
        spawnPos, this, angle, projData.baseVelocity, projData
    ));
    proj->onlyDamagePlayer = onlyDamagePlayer;
    proj->loadSprite(getWorld()->getSpriteSheet());
    proj->setWorld(getWorld());
    getWorld()->addEntity(proj);

    if (soundName != "NONE") SoundManager::playSound(soundName);
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

void Entity::takeDamage(int damage) {
    if (getEntityType() != "player" && getEntityType() != "shopext" && getEntityType() != "shopint" && getEntityType() != "barberext" && getEntityType() != "barberint") {
        std::shared_ptr<DamageParticle> damageParticle = std::shared_ptr<DamageParticle>(new DamageParticle(getPosition(), damage));
        damageParticle->setWorld(getWorld());
        damageParticle->loadSprite(getWorld()->getSpriteSheet());
        getWorld()->addEntity(damageParticle);
    }
    this->damage(damage);
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

void Entity::increaseStaminaRefreshRate(int amount) {
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

