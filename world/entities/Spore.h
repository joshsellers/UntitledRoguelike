#ifndef _SPORE_H
#define _SPORE_H

#include "Entity.h"

class Spore : public Entity {
public:
    Spore(sf::Vector2f pos);

    void update();
    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:
    const long long _spawnTime;
    const long long _lifeTime;

    sf::Vector2f _goalPos;
};

#endif