#ifndef _SHOP_EXTERIOR_H
#define _SHOP_EXTERIOR_H

#include "Entity.h"

class ShopExterior : public Entity {
public:
    ShopExterior(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:
    sf::Sprite _closedSprite;
};

#endif
