#include "Cactus.h"
#include "Util.h"

Cactus::Cactus(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 2, 2, true) {
    loadSprite(spriteSheet);
    
    _displayBottom = true;
}

void Cactus::update() {
}

void Cactus::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void Cactus::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(CACTUS_SPRITE_POS_X + (randomInt(0, 1) * TILE_SIZE * 2), CACTUS_SPRITE_POS_Y, TILE_SIZE * 2, TILE_SIZE * 2)
    );
    _sprite.setPosition(getPosition());
}
