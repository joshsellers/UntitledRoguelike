#include "LargeSavannaTree.h"
#include "World.h"

LargeSavannaTree::LargeSavannaTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 4, 7, true) {
    loadSprite(spriteSheet);

    setMaxHitPoints(45);
    heal(getMaxHitPoints());

    _hitBoxXOffset = 0;
    _hitBoxYOffset = TILE_SIZE;
    _hitBox.width = 16 * 3;
    _hitBox.height = 16 * 6;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    unsigned int woodAmount = randomInt(1, 8);
    getInventory().addItem(Item::WOOD.getId(), woodAmount);

    _displayBottom = true;
}

void LargeSavannaTree::update() {
}

void LargeSavannaTree::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void LargeSavannaTree::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(LARGE_SAVANNA_TREE_SPRITE_POS_X, LARGE_SAVANNA_TREE_SPRITE_POS_Y, TILE_SIZE * 4, TILE_SIZE * 7)
    );
    _sprite.setPosition(getPosition());
}

void LargeSavannaTree::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        getWorld()->propDestroyedAt(sf::Vector2f(_pos.x + (_spriteWidth * TILE_SIZE) / 2, _pos.y + (_spriteHeight * TILE_SIZE)));
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}
