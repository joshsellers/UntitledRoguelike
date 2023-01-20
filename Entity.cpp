#include "Entity.h"

Entity::Entity(sf::Vector2f pos, float baseSpeed, const int spriteWidth, const int spriteHeight, const bool isProp) : 
    _spriteWidth(spriteWidth), _spriteHeight(spriteHeight), _isProp(isProp) {

    _pos = _isProp ? sf::Vector2f(pos.x - (_spriteWidth * TILE_SIZE) / 2, pos.y - (_spriteHeight * TILE_SIZE)) : pos;
    _baseSpeed = baseSpeed;
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

bool Entity::isDamageable() const {
    return getMaxHitPoints() > 0;
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
