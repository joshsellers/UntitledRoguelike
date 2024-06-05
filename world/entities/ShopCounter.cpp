#include "ShopCounter.h"
#include "../World.h"

ShopCounter::ShopCounter(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 96, 48, false) {
    loadSprite(spriteSheet);

    _entityType = "shopcounter";

    _hasColliders = true;
    sf::FloatRect collider;
    collider.left = _pos.x;
    collider.top = _pos.y + 20;
    collider.width = 96;
    collider.height = 10;
    _colliders.push_back(collider);

    setMaxHitPoints(10000000);
    heal(getMaxHitPoints());
}

void ShopCounter::update() {
    if (!getWorld()->playerIsInShop()) deactivate();
}

void ShopCounter::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void ShopCounter::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(39 * TILE_SIZE, 14 * TILE_SIZE, 96, 48)
    );
    _sprite.setPosition(getPosition());
}
