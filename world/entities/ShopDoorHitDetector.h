#ifndef _SHOP_DOOR_HIT_DETECTOR_H
#define _SHOP_DOOR_HIT_DETECTOR_H

#include "Entity.h"
#include "ShopExterior.h"

class ShopDoorHitDetector : public Entity {
public:
    ShopDoorHitDetector(sf::Vector2f pos, ShopExterior* shopExt);

    virtual void takeDamage(int damage, bool crit = false);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:

private:
    ShopExterior* _shopExt;
};

#endif