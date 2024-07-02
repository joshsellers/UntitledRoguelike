#include "FingerTree.h"
#include "../World.h"

FingerTree::FingerTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 3, 5, true) {
    loadSprite(spriteSheet);

    setMaxHitPoints(55);
    heal(getMaxHitPoints());

    _hitBoxXOffset = 6;
    _hitBoxYOffset = TILE_SIZE;
    _hitBox.width = 18;
    _hitBox.height = 16 * 4;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    //unsigned int woodAmount = randomInt(1, 8);
    //getInventory().addItem(Item::WOOD.getId(), woodAmount);

    _displayBottom = true;
}

void FingerTree::update() {
}

void FingerTree::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void FingerTree::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(240, 48, TILE_SIZE * 2, TILE_SIZE * 5)
    );
    _sprite.setPosition(getPosition());
}

void FingerTree::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        getWorld()->propDestroyedAt(sf::Vector2f(_pos.x + (_spriteWidth * TILE_SIZE) / 2, _pos.y + (_spriteHeight * TILE_SIZE)));
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}
