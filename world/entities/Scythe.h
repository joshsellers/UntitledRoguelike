#ifndef _SCYTHE_H
#define _SCYTHE_H

#include "Entity.h"

class Scythe : public Entity {
public:
    Scythe(sf::Vector2f pos);

    void update();
    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:

private:
    sf::Vector2f _targetPos;

    long long _lastDamageTimeMillis = 0;
};

#endif