#ifndef _HAIR_STYLE_H
#define _HAIR_STYLE_H

#include <vector>
#include "HairType.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

class Hairstyle {
public:
    static const Hairstyle HAIR_EMO;
    static const Hairstyle HAIR_BASIC;
    static std::vector<const Hairstyle*> HAIRSTYLES;

    Hairstyle(const unsigned int id, HAIR_TYPE hairType, sf::Vector2i spriteCoords);

    unsigned int getId() const;
    HAIR_TYPE getHairType() const;
    sf::IntRect getTextureRect() const;
private:
    const unsigned int _id;
    const HAIR_TYPE _hairType;
    const sf::IntRect _textureRect;
};

#endif