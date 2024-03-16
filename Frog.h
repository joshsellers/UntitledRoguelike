#ifndef _FROG_H
#define _FROG_H

#include "Entity.h"

class Frog : public Entity {
public:
    Frog(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void damage(int damage);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:

    boost::random::mt19937 _gen = boost::random::mt19937();
};

#endif
