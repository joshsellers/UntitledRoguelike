#ifndef _FALLING_BOMB_H
#define _FALLING_BOMB_H

#include "Entity.h"

class FallingBomb : public Entity {
public:
    FallingBomb(sf::Vector2f targetPos);

    void update();
    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

private:
    bool _landed = false;

    const sf::Vector2f _targetPos;
};

#endif