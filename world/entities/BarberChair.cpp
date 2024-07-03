#include "BarberChair.h"
#include "../World.h"

BarberChair::BarberChair(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 32, 32, false) {
    loadSprite(spriteSheet);

    _entityType = "barberchair";

    _hasColliders = true;
    sf::FloatRect collider;
    collider.left = _pos.x + TILE_SIZE;
    collider.top = _pos.y + TILE_SIZE;
    collider.width = TILE_SIZE;
    collider.height = 4;
    _colliders.push_back(collider);

    setMaxHitPoints(10000000);
    heal(getMaxHitPoints());
}

void BarberChair::update() {
    if (!getWorld()->playerIsInShop()) deactivate();
}

void BarberChair::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void BarberChair::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(896, 640, 32, 32)
    );
    _sprite.setPosition(getPosition());
}
