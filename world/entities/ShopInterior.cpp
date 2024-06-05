#include "ShopInterior.h"
#include "../World.h"

ShopInterior::ShopInterior(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, SHOP_INTERIOR_WIDTH, SHOP_INTERIOR_HEIGHT, false) {
    loadSprite(spriteSheet);
    int posY = _pos.y;
    _pos.y -= 176 * 2;
    _sprite.setPosition(_pos.x, posY);

    _entityType = "shopint";

    _hitBoxXOffset = 80;
    _hitBoxYOffset = 174 * 3;
    _hitBox.width = TILE_SIZE * 2;
    _hitBox.height = 2;

    _hitBox.left = getPosition().x + _hitBoxXOffset - 16;
    _hitBox.top = getPosition().y + _hitBoxYOffset + 4;

    setMaxHitPoints(100000000);
    heal(getMaxHitPoints());

    _hasColliders = true;
    sf::FloatRect topCollider;
    topCollider.left = getSprite().getPosition().x;
    topCollider.top = getSprite().getPosition().y - 10;
    topCollider.width = SHOP_INTERIOR_WIDTH;
    topCollider.height = 18;

    sf::FloatRect leftCollider;
    leftCollider.left = getSprite().getPosition().x - 10;
    leftCollider.top = getSprite().getPosition().y;
    leftCollider.width = 10;
    leftCollider.height = SHOP_INTERIOR_HEIGHT;

    sf::FloatRect rightCollider;
    rightCollider.left = getSprite().getPosition().x + SHOP_INTERIOR_WIDTH - 16;
    rightCollider.top = getSprite().getPosition().y;
    rightCollider.width = 10;
    rightCollider.height = SHOP_INTERIOR_HEIGHT;

    sf::FloatRect bottomCollider;
    bottomCollider.left = getSprite().getPosition().x;
    bottomCollider.top = getSprite().getPosition().y + SHOP_INTERIOR_HEIGHT;
    bottomCollider.width = SHOP_INTERIOR_WIDTH;
    bottomCollider.height = 10;

    sf::FloatRect gapCollider;
    gapCollider.left = getSprite().getPosition().x;
    gapCollider.top = getSprite().getPosition().y;
    gapCollider.width = 96 - 16;
    gapCollider.height = 64 - 8;

    _colliders.push_back(topCollider);
    _colliders.push_back(leftCollider);
    _colliders.push_back(rightCollider);
    _colliders.push_back(bottomCollider);
    _colliders.push_back(gapCollider);
}

void ShopInterior::update() {
    auto& entity = getWorld()->getPlayer();
    if (entity->isActive() && entity->getEntityType() == "player" && entity->getHitBox().intersects(getHitBox())) {
        getWorld()->exitShop();
        deactivate();
    }
}

void ShopInterior::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void ShopInterior::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(46 * TILE_SIZE, 9 * TILE_SIZE, SHOP_INTERIOR_WIDTH, SHOP_INTERIOR_HEIGHT)
    );
} 