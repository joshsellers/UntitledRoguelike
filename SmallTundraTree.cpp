#include "SmallTundraTree.h"
#include "Util.h"

SmallTundraTree::SmallTundraTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(pos, 0) {
    loadSprite(spriteSheet);

    _isProp = true;
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
