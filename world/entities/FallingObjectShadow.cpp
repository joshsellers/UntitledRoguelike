#include "FallingObjectShadow.h"

FallingObjectShadow::FallingObjectShadow(sf::Vector2f pos) : Entity(NO_SAVE, pos, 0, 3 * TILE_SIZE, TILE_SIZE, false) {
}

void FallingObjectShadow::update() {
}

void FallingObjectShadow::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void FallingObjectShadow::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(896, 1696, TILE_SIZE * 3, TILE_SIZE));
    _sprite.setOrigin((float)TILE_SIZE * 3 / 2, 0);
    _sprite.setPosition(getPosition());
}
