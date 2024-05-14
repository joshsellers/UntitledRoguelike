#include "SmallSavannaTree.h"
#include "World.h"

SmallSavannaTree::SmallSavannaTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 3, 4, true) {
    loadSprite(spriteSheet);

    setMaxHitPoints(35);
    heal(getMaxHitPoints());

    _hitBoxXOffset = 0;
    _hitBoxYOffset = 0;
    _hitBox.width = 16 * 3;
    _hitBox.height = 16 * 4;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    unsigned int woodAmount = randomInt(1, 6);
    getInventory().addItem(Item::WOOD.getId(), woodAmount);

    _displayBottom = true;
}

void SmallSavannaTree::update() {
}

void SmallSavannaTree::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void SmallSavannaTree::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(SMALL_SAVANNA_TREE_SPRITE_POS_X, SMALL_SAVANNA_TREE_SPRITE_POS_Y, TILE_SIZE * 3, TILE_SIZE * 4)
    );
    _sprite.setPosition(getPosition());
}

void SmallSavannaTree::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        getWorld()->propDestroyedAt(sf::Vector2f(_pos.x + (_spriteWidth * TILE_SIZE) / 2, _pos.y + (_spriteHeight * TILE_SIZE)));
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}
