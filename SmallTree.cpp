#include "SmallTree.h"
#include "Util.h"

SmallTree::SmallTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(pos, 0, 2, 3, true) {
    loadSprite(spriteSheet);

    setMaxHitPoints(10);
    heal(getMaxHitPoints());
}

void SmallTree::update() {}

void SmallTree::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void SmallTree::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(STREE_SPRITE_POS_X + (randomInt(0, 1) * TILE_SIZE * 2), STREE_SPRITE_POS_Y, TILE_SIZE * 2, TILE_SIZE * 3)
    );
    _sprite.setPosition(getPosition());
}
