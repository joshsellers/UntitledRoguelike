#ifndef _CACTUS_H
#define _CACTUS_H

#include "Entity.h"

constexpr int CACTUS_SPRITE_POS_X = 4 << SPRITE_SHEET_SHIFT;
constexpr int CACTUS_SPRITE_POS_Y = 5 << SPRITE_SHEET_SHIFT;

class Cactus : public Entity {
public:
    Cactus(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
};

#endif