#include "TallGrass.h"

#include "Util.h"

TallGrass::TallGrass(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(pos, 0, 1, 1, true) {
    loadSprite(spriteSheet);

    _displayBottom = true;
}

void TallGrass::update() {}

void TallGrass::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void TallGrass::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(TGRASS_SPRITE_POS_X + (randomInt(0, 2) << SPRITE_SHEET_SHIFT), TGRASS_SPRITE_POS_Y, TILE_SIZE, TILE_SIZE)
    );
    _sprite.setPosition(getPosition());
}
