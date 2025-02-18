#ifndef _SMALL_MUSHROOM_H
#define _SMALL_MUSHROOM_H

#include "Entity.h"

class SmallMushroom : public Entity {
public:
    SmallMushroom(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
};

#endif