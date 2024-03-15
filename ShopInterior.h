#ifndef _SHOP_INTERIOR_H
#define _SHOP_INTERIOR_H

#include "Entity.h"

constexpr int SHOP_INTERIOR_WIDTH = 192;
constexpr int SHOP_INTERIOR_HEIGHT = 176;

class ShopInterior : public Entity {
public:
    ShopInterior(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
};

#endif