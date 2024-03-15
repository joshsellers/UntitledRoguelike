#ifndef _SHOP_COUNTER_H
#define _SHOP_COUNTER_H

#include "Entity.h"

class ShopCounter : public Entity {
public:
    ShopCounter(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
};

#endif
