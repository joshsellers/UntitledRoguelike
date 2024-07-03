#ifndef _BARBER_COUNTER_H
#define _BARBER_COUNTER_H

#include "Entity.h"

class BarberCounter : public Entity {
public:
    BarberCounter(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
};

#endif

