#ifndef _PENGUIN_H
#define _PENGUIN_H

#include "Entity.h"
#include <boost/random/mersenne_twister.hpp>


class Penguin : public Entity {
public:
    Penguin(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void damage(int damage);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:
    sf::Vector2f _targetPos;

    boost::random::mt19937 _gen = boost::random::mt19937();
};

#endif