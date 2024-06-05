#ifndef _TALL_GRASS_H
#define _TALL_GRASS_H

#include "Entity.h"

constexpr int TGRASS_SPRITE_POS_X = 4 << SPRITE_SHEET_SHIFT;
constexpr int TGRASS_SPRITE_POS_Y = 3 << SPRITE_SHEET_SHIFT; 

class TallGrass : public Entity {
public:
    TallGrass(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
};

#endif
