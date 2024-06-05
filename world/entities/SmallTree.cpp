#include "SmallTree.h"
#include "../World.h"

SmallTree::SmallTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 2, 3, true) {
    loadSprite(spriteSheet);

    setMaxHitPoints(20);
    heal(getMaxHitPoints());

    _hitBoxXOffset = 0;
    _hitBoxYOffset = 0;
    _hitBox.width = 16 * 2;
    _hitBox.height = 16 * 3;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    srand(currentTimeNano());
    unsigned int appleAmount = randomInt(0, 24);
    if (appleAmount >= 18) getInventory().addItem(Item::APPLE.getId(), appleAmount - 18);

    unsigned int woodAmount = randomInt(1, 5);
    getInventory().addItem(Item::WOOD.getId(), woodAmount);
}

void SmallTree::update() {}

void SmallTree::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void SmallTree::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        getWorld()->propDestroyedAt(sf::Vector2f(_pos.x + (_spriteWidth * TILE_SIZE) / 2, _pos.y + (_spriteHeight * TILE_SIZE)));
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}

void SmallTree::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(STREE_SPRITE_POS_X + (randomInt(0, 1) * TILE_SIZE * 2), STREE_SPRITE_POS_Y, TILE_SIZE * 2, TILE_SIZE * 3)
    );
    _sprite.setPosition(getPosition());
}
