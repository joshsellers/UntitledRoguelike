#ifndef _HYPNO_PENGUIN_H
#define _HYPNO_PENGUIN_H

#include "Entity.h"

class HypnoPenguin : public Entity {
public:
    HypnoPenguin(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
};

#endif