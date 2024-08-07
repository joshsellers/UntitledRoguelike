#include "Hairstyle.h"
#include "../../world/entities/Entity.h"


const Hairstyle Hairstyle::HAIR_EMO(0, HAIR_TYPE::HEAD, sf::Vector2i(0, 47));
const Hairstyle Hairstyle::HAIR_BASIC(1, HAIR_TYPE::HEAD, sf::Vector2i(12, 47));

std::vector<const Hairstyle*> Hairstyle::HAIRSTYLES;

Hairstyle::Hairstyle(const unsigned int id, HAIR_TYPE hairType, sf::Vector2i spriteCoords) :
_id(id), _hairType(hairType), 
_textureRect(sf::IntRect(spriteCoords.x << SPRITE_SHEET_SHIFT, spriteCoords.y << SPRITE_SHEET_SHIFT, TILE_SIZE * 3, TILE_SIZE * 3)) {
    HAIRSTYLES.push_back(this);
}

unsigned int Hairstyle::getId() const {
    return _id;
}

HAIR_TYPE Hairstyle::getHairType() const {
    return _hairType;
}

sf::IntRect Hairstyle::getTextureRect() const {
    return _textureRect;
}
