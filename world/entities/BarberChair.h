#ifndef _BARBER_CHAIR_H
#define _BARBER_CHAIR_H

#include "Entity.h"

class BarberChair : public Entity {
public:
    BarberChair(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
};

#endif
