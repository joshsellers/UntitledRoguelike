#include "SmallTundraTree.h"
#include "Util.h"
#include "World.h"

SmallTundraTree::SmallTundraTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 2, 3, true) {
    loadSprite(spriteSheet); 
    
    setMaxHitPoints(20);
    heal(getMaxHitPoints());

    _hitBoxXOffset = 0;
    _hitBoxYOffset = 0;
    _hitBox.width = 16 * 2;
    _hitBox.height = 16 * 3;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    unsigned int woodAmount = randomInt(1, 5);
    getInventory().addItem(Item::WOOD.getId(), woodAmount);
}

void SmallTundraTree::update() {}

void SmallTundraTree::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void SmallTundraTree::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(S_TUNDRA_TREE_SPRITE_POS_X + (randomInt(0, 1) * TILE_SIZE * 2), S_TUNDRA_TREE_SPRITE_POS_Y, TILE_SIZE * 2, TILE_SIZE * 3)
    );
    _sprite.setPosition(getPosition());
}

void SmallTundraTree::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        getWorld()->propDestroyedAt(sf::Vector2f(_pos.x + (_spriteWidth * TILE_SIZE) / 2, _pos.y + (_spriteHeight * TILE_SIZE)));
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}
