#include "ShopExterior.h"
#include "../World.h"
#include "ShopInterior.h"

ShopExterior::ShopExterior(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 192, 96, true) {
    _pos = pos;
    loadSprite(spriteSheet);

    _hitBoxXOffset = 80;
    _hitBoxYOffset = 64;
    _hitBox.width = TILE_SIZE * 2;
    _hitBox.height = 4;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    setMaxHitPoints(10000000);
    heal(getMaxHitPoints());

    _entityType = "shopext";

    _hasColliders = true;
    sf::FloatRect colliderLeft;
    colliderLeft.left = getSprite().getPosition().x + 16;
    colliderLeft.top = getSprite().getPosition().y + (16 * 3 - 2);
    colliderLeft.width = 64;
    colliderLeft.height = 30;

    sf::FloatRect colliderRight;
    colliderRight.left = getSprite().getPosition().x + 16 * 7;
    colliderRight.top = getSprite().getPosition().y + (16 * 3 - 2);
    colliderRight.width = 64;
    colliderRight.height = 30;

    sf::FloatRect colliderTop;
    colliderTop.left = getSprite().getPosition().x + 16 * 5;
    colliderTop.top = getSprite().getPosition().y + (16 * 3 - 2);
    colliderTop.width = 32;
    colliderTop.height = 16;

    _colliders.push_back(colliderLeft);
    _colliders.push_back(colliderRight);
    _colliders.push_back(colliderTop);
}

void ShopExterior::update() {
    if (!getWorld()->playerIsInShop()) {
        _hasColliders = true;
        auto& entity = getWorld()->getPlayer();
        if (entity->isActive() && entity->getEntityType() == "player" && entity->getHitBox().intersects(getHitBox())) {
            getWorld()->enterShop(sf::Vector2f(_pos.x + 16, _pos.y - 48));
        }
    } else _hasColliders = false;
}

void ShopExterior::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void ShopExterior::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(45 * TILE_SIZE, 23 * TILE_SIZE, 192, 96)
    );
    _sprite.setPosition(getPosition());
}
