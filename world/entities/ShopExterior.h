#ifndef _SHOP_EXTERIOR_H
#define _SHOP_EXTERIOR_H

#include "Entity.h"

class ShopExterior : public Entity {
public:
    ShopExterior(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet, bool doorBlownOpen = false);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    void blowOpenDoor();

    virtual void deactivate();
protected:
    virtual void damage(int damage);
private:
    sf::Sprite _closedSprite;
    sf::Sprite _blownUpSprite;

    bool _doorBlownOpen = false;

    std::shared_ptr<Entity> _doorHitDetector = nullptr;
};

#endif
