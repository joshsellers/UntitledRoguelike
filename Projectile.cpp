#include "Projectile.h"
#include "World.h"

constexpr float LIFETIME = 60 * 5;

Projectile::Projectile(sf::Vector2f pos, sf::Vector2f shooterVelocity, float directionAngle, float velocity, const ProjectileData data) :
    Entity(pos, 0, 1, 1, false), _originalPos(pos), _directionAngle(directionAngle), _velocity(velocity), _data(data),
    _itemId(data.itemId) {

    _velocityComponents.x = _velocity * std::cos(directionAngle) + shooterVelocity.x;
    _velocityComponents.y = _velocity * std::sin(directionAngle) + shooterVelocity.y;

    setMaxHitPoints(1);

    _hitBoxXOffset = _data.hitBox.left;
    _hitBoxYOffset = _data.hitBox.top;
    _hitBox.width = _data.hitBox.width;
    _hitBox.height = _data.hitBox.height;
}

void Projectile::update() {
    if (_currentTime > LIFETIME) {
        _isActive = false;
        return;
    }

    for (auto& entity : getWorld()->getEntities()) {
        if (entity->getHitBox() != getHitBox() && entity->isActive() && entity->isDamageable()) {
            if (entity->getHitBox().intersects(_hitBox)) {
                entity->damage(Item::ITEMS[_itemId]->getDamage());
                _isActive = false;
                return;
            }
        }
    }

    _pos.x = _velocityComponents.x * _currentTime + _originalPos.x;
    _pos.y = _velocityComponents.y * _currentTime + _originalPos.y;

    _sprite.setPosition(_pos);

    _hitBox.left = _sprite.getGlobalBounds().left + _hitBoxXOffset;
    _hitBox.top = _sprite.getGlobalBounds().top + _hitBoxYOffset;

    _currentTime++;
}

void Projectile::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void Projectile::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    const Item* item = Item::ITEMS[_itemId];
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(item->getTextureRect());
    _sprite.setOrigin(0, item->getTextureRect().height / 2);
    if (_data.rotateSprite) _sprite.setRotation(_directionAngle * (180.f / PI));

    _hitBox.left = _sprite.getGlobalBounds().left + _hitBoxXOffset;
    _hitBox.top = _sprite.getGlobalBounds().top + _hitBoxYOffset;
}
