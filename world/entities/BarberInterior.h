#ifndef _BARBER_INTERIOR_H
#define _BARBER_INTERIOR_H

#include "Entity.h"

constexpr int BARBER_INTERIOR_WIDTH = 128;
constexpr int BARBER_INTERIOR_HEIGHT = 112;

class BarberInterior : public Entity {
public:
    BarberInterior(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    sf::Vector2f getEntrancePos();
};
#endif