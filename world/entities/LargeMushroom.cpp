#include "LargeMushroom.h"
#include "../World.h"

LargeMushroom::LargeMushroom(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 2, 2, true) {
    loadSprite(spriteSheet);

    setMaxHitPoints(15);
    heal(getMaxHitPoints());

    _hitBoxXOffset = 7;
    _hitBoxYOffset = 7;
    _hitBox.width = 20;
    _hitBox.height = 26;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    srand(currentTimeNano());
    if (randomChance(0.02f)) getInventory().addItem(Item::getIdFromName("Funny Mushroom"), 1);
}

void LargeMushroom::update() {}

void LargeMushroom::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void LargeMushroom::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        getWorld()->propDestroyedAt(sf::Vector2f(_pos.x + (_spriteWidth * TILE_SIZE) / 2, _pos.y + (_spriteHeight * TILE_SIZE)));
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}

void LargeMushroom::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(158 * TILE_SIZE, 53 * TILE_SIZE, TILE_SIZE * 2, TILE_SIZE * 2)
    );
    _sprite.setPosition(getPosition());
}
