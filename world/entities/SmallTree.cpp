#include "SmallTree.h"
#include "../World.h"

SmallTree::SmallTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 3, 4, true) {
    loadSprite(spriteSheet);

    setMaxHitPoints(20);
    heal(getMaxHitPoints());

    _hitBoxXOffset = 0;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 3;
    _hitBox.height = TILE_SIZE * 4;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    srand(currentTimeNano());
    unsigned int appleAmount = randomInt(0, (HARD_MODE_ENABLED ? 99 : 3));
    if (appleAmount == 0) getInventory().addItem(Item::APPLE.getId(), 1);

    unsigned int woodAmount = 1;
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
        sf::IntRect(STREE_SPRITE_POS_X * TILE_SIZE, STREE_SPRITE_POS_Y * TILE_SIZE, TILE_SIZE * 3, TILE_SIZE * 4)
    );
    _sprite.setPosition(getPosition());
}
