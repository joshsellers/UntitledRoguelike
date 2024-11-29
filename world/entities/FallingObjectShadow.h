#ifndef _FALLING_OBJECT_SHADOW_H
#define _FALLING_OBJECT_SHADOW_H

#include "Entity.h"

class FallingObjectShadow : public Entity {
public:
    FallingObjectShadow(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
};

#endif