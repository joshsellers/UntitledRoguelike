#include "Cactus.h"
#include "Util.h"

Cactus::Cactus(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(pos, 0) {
    loadSprite(spriteSheet);

    _isProp = true;
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
