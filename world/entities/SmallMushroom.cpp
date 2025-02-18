#include "SmallMushroom.h"
#include "../World.h"

SmallMushroom::SmallMushroom(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 1, 1, true) {
    loadSprite(spriteSheet);

    setMaxHitPoints(5);
    heal(getMaxHitPoints());

    _hitBoxXOffset = 0;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 1;
    _hitBox.height = TILE_SIZE * 1;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void SmallMushroom::update() {}

void SmallMushroom::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void SmallMushroom::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        getWorld()->propDestroyedAt(sf::Vector2f(_pos.x + (_spriteWidth * TILE_SIZE) / 2, _pos.y + (_spriteHeight * TILE_SIZE)));
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}

void SmallMushroom::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(160 * TILE_SIZE, (44 + randomInt(0, 1)) * TILE_SIZE, TILE_SIZE * 1, TILE_SIZE * 1)
    );
    _sprite.setPosition(getPosition());
}
