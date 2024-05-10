#ifndef _TURTLE_H
#define _TURTLE_H

#include "Entity.h"
#include <boost/random/mersenne_twister.hpp>

class Turtle : public Entity {
public:
    Turtle(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
private:

    boost::random::mt19937 _gen = boost::random::mt19937();
};

#endif