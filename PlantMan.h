#ifndef _PLANT_MAN_H
#define _PLANT_MAN_H

#include "Entity.h"
#include <boost/random/mersenne_twister.hpp>


class PlantMan : public Entity {
public:
    PlantMan(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void damage(int damage);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:
    sf::Sprite _wavesSprite;

    int _currentPathIndex = 1;

    boost::random::mt19937 _gen = boost::random::mt19937();
};

#endif