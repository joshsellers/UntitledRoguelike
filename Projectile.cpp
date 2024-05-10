#include "Projectile.h"
#include "World.h"

constexpr float LIFETIME = 60 * 5;

Projectile::Projectile(sf::Vector2f pos, Entity* parent, float directionAngle, float velocity, const ProjectileData data) :
    Entity(pos, 0, 1, 1, false), _originalPos(pos), _parent(parent), _directionAngle(directionAngle), _velocity(velocity), _data(data),
    _itemId(data.itemId) {

    sf::Vector2f shooterVelocity(parent->getVelocity().x, parent->getVelocity().y);

    _velocityComponents.x = _velocity * std::cos(directionAngle) + shooterVelocity.x;
    _velocityComponents.y = _velocity * std::sin(directionAngle) + shooterVelocity.y;

    _lifeTime = data.lifeTime;

    //setMaxHitPoints(1000000);
    //heal(getMaxHitPoints());

    _hitBoxXOffset = _data.hitBox.left;
    _hitBoxYOffset = _data.hitBox.top;
    _hitBox.width = _data.hitBox.width;
    _hitBox.height = _data.hitBox.height;
}

void Projectile::update() {
    if (_currentTime > _lifeTime) {
        _isActive = false;
        return;
    }

    for (auto& entity : getWorld()->getEntities()) {
        if (!entity->compare(_parent) && entity->getHitBox() != getHitBox() && entity->isActive() && entity->isDamageable()
            && (!_data.onlyHitEnemies || entity->isEnemy()) && !(_parent->getEntityType() == "player" && entity->getEntityType() == "dontblockplayershots")) {
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
    if (_data.isAnimated) {
        const Item* item = Item::ITEMS[_itemId];
        sf::IntRect projRect = item->getTextureRect();

        int yOffset = ((((_animationTime) >> _data.animationSpeed) & (_data.animationFrames - 1))) * 16;
        if (_data.animationSpeed == 0) {
            srand(currentTimeMillis());
            yOffset = randomInt(0, _data.animationFrames - 1);
        }
        _sprite.setTextureRect(sf::IntRect(projRect.left, projRect.top + yOffset, projRect.width, projRect.height));

        _animationTime++;
    }


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
