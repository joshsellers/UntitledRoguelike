#include "Entity.h"
#include <boost/random/uniform_int_distribution.hpp>
#include "World.h"

Entity::Entity(sf::Vector2f pos, float baseSpeed, const int spriteWidth, const int spriteHeight, const bool isProp) : 
    _spriteWidth(spriteWidth), _spriteHeight(spriteHeight), _isProp(isProp) {

    _pos = _isProp ? sf::Vector2f(pos.x - (_spriteWidth * TILE_SIZE) / 2, pos.y - (_spriteHeight * TILE_SIZE)) : pos;
    _baseSpeed = baseSpeed;

    _wanderTargetPos = _pos;
}

void Entity::move(float xa, float ya) {
    if (std::abs(xa) > 0 || std::abs(ya) > 0) {
        _numSteps++;
        _isMoving = true;
    } else _isMoving = false;

    _pos.x += xa;
    _pos.y += ya;
    _velocity.x = xa;
    _velocity.y = ya;
}

void Entity::wander(sf::Vector2f feetPos, boost::random::mt19937& generator) {
    if (_world != nullptr) {
        float xa = 0, ya = 0;

        boost::random::uniform_int_distribution<> shouldMove(0, 100);
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

bool Entity::isMoving() const {
    return _isMoving;
}

MOVING_DIRECTION Entity::getMovingDir() const {
    return (MOVING_DIRECTION)_movingDir;
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

bool Entity::canPickUpItems() const {
    return _canPickUpItems;
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

sf::FloatRect Entity::getHitBox() const {
    return _hitBox;
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
