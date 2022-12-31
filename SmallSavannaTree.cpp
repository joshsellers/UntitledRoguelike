#include "SmallSavannaTree.h"

SmallSavannaTree::SmallSavannaTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(pos, 0) {
    loadSprite(spriteSheet);

    _isProp = true;
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