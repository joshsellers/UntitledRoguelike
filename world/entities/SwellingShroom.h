#ifndef _SWELLING_SHROOM_H
#define _SWELLING_SHROOM_H

#include "Entity.h"

class SwellingShroom : public Entity {
public:
    SwellingShroom(sf::Vector2f pos);

    void update();
    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:
    const long long _spawnTime;
};

#endif