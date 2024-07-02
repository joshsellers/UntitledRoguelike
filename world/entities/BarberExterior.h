#ifndef _BARBER_EXTERIOR_H
#define _BARBER_EXTERIOR_H

#include "Entity.h"

class BarberExterior : public Entity {
public:
    BarberExterior(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
};
#endif