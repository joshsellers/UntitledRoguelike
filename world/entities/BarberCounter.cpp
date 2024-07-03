#include "BarberCounter.h"
#include "../World.h"

BarberCounter::BarberCounter(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 64, 48, false) {
    loadSprite(spriteSheet);

    _entityType = "barbercounter";

    _hasColliders = true;
    sf::FloatRect collider;
    collider.left = _pos.x;
    collider.top = _pos.y + 20;
    collider.width = 64;
    collider.height = 10;
    _colliders.push_back(collider);

    setMaxHitPoints(10000000);
    heal(getMaxHitPoints());
}

void BarberCounter::update() {
    if (!getWorld()->playerIsInShop()) deactivate();
}

void BarberCounter::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void BarberCounter::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(704, 640, 64, 48)
    );
    _sprite.setPosition(getPosition());
}
