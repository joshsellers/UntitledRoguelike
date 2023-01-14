#include "Projectile.h"
#include <iostream>

constexpr long double PI = 3.14159265358979L;

Projectile::Projectile(sf::Vector2f pos, float directionAngle, float velocity, unsigned int itemId) : 
    Entity(pos, 0, 1, 1, false), _originalPos(pos), _directionAngle(directionAngle), _velocity(velocity),
    _itemId(itemId) {

    _velocityComponents.x = _velocity * std::cos(directionAngle);
    _velocityComponents.y = _velocity * std::sin(directionAngle);
}

void Projectile::update() {
    _pos.x = _velocityComponents.x * _currentTime + _originalPos.x;
    _pos.y = _velocityComponents.y * _currentTime + _originalPos.y;

    _currentTime++;
    _sprite.setPosition(_pos);
}

void Projectile::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void Projectile::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    const Item* item = Item::ITEMS[_itemId];
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(item->getTextureRect());
    _sprite.setOrigin(0, item->getTextureRect().height / 2);
    _sprite.setRotation(_directionAngle * (180.f / PI));
}
