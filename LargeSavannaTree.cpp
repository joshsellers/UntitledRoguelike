#include "LargeSavannaTree.h"

LargeSavannaTree::LargeSavannaTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(pos, 0) {
    loadSprite(spriteSheet);

    _isProp = true;
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