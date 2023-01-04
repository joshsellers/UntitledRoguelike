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
}

bool Entity::isMoving() const {
    return _isMoving;
}

sf::Vector2f Entity::getPosition() const {
    return _pos;
}

bool Entity::isProp() const {
    return _isProp;
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
