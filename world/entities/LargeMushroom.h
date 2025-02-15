#ifndef _LARGE_MUSHROOM_H
#define _LARGE_MUSHROOM_H

#include "Entity.h"

class LargeMushroom : public Entity {
public:
    LargeMushroom(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
};

#endif