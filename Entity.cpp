#include "Entity.h"

Entity::Entity(sf::Vector2f pos, float baseSpeed) {
    _pos = pos;
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
