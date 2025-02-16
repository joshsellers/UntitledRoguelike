#ifndef _BOUNCER_H
#define _BOUNCER_H

#include "Entity.h"

class Bouncer : public Entity {
public:
    Bouncer(sf::Vector2f pos);

    void update();
    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:

};

#endif