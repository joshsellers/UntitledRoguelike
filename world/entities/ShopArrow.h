#ifndef _SHOP_ARROW_H
#define _SHOP_ARROW_H

#include "../World.h"

class ShopArrow {
public:
    void update();
    void draw(sf::RenderTexture& surface);

    void setWorld(World* world);
    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:
    sf::Sprite _sprite;

    World* _world;

    bool _isVisible = false;
};

#endif

