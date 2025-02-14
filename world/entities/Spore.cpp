#include "Spore.h"
#include "../World.h"

Spore::Spore(sf::Vector2f pos) : Entity(NO_SAVE, pos, 1.f, TILE_SIZE, TILE_SIZE, false), _spawnTime(currentTimeMillis()), _lifeTime(8000LL + randomInt(0, 8000)) {
    _entityType = "spore";
    
    _goalPos = _pos;
}

void Spore::update() {
    if (currentTimeMillis() - _spawnTime >= _lifeTime) deactivate();

    const sf::Vector2f currentPos(getPosition().x, getPosition().y);

    const float dx = _goalPos.x - currentPos.x;
    const float dy = _goalPos.y - currentPos.y;

    const float speed = 0.5f;
    const float angle = std::atan2(dy, dx);
    const float xa = speed * std::cos(angle);
    const float ya = speed * std::sin(angle);

    move(xa, ya); 

    const float distSquared = (dx * dx) + (dy * dy);
    constexpr float desiredDistanceSquared = 1.f;
    if (distSquared <= desiredDistanceSquared) {
        _goalPos = sf::Vector2f(_pos.x + randomInt(-2, 2), _pos.y + randomInt(-2, 2));
    }
    
    if (_world->getPlayer()->getHitBox().intersects(getSprite().getGlobalBounds())) {
        _world->getPlayer()->takeDamage(15);
    }
    _sprite.setPosition(_pos);
}

void Spore::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void Spore::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(160 << SPRITE_SHEET_SHIFT, 43 << SPRITE_SHEET_SHIFT, TILE_SIZE * 1, TILE_SIZE * 1));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE / 2.f, 0);
}
