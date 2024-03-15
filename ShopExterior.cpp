#include "ShopExterior.h"
#include "World.h"
#include "ShopInterior.h"

ShopExterior::ShopExterior(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(pos, 0, 16, 16, false) {
    loadSprite(spriteSheet);

    _hitBoxXOffset = 0;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE;
    _hitBox.height = TILE_SIZE;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    setMaxHitPoints(10000000);
    heal(getMaxHitPoints());

    _entityType = "shopext";
}

void ShopExterior::update() {
    for (auto& entity : getWorld()->getEntities()) {
        if (entity->isActive() && entity->getEntityType() == "player" && entity->getHitBox().intersects(getHitBox())) {
            getWorld()->enterShop(sf::Vector2f(_pos.x - SHOP_INTERIOR_WIDTH / 2 + 16, _pos.y - SHOP_INTERIOR_HEIGHT + 64));
        }
    }
}

void ShopExterior::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void ShopExterior::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(0, 9 * TILE_SIZE, 16, 16)
    );
    _sprite.setPosition(getPosition());
}
