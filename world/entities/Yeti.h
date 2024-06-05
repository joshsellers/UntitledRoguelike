#ifndef _YETI_H
#define _YETI_H

#include "Entity.h"

class Yeti : public Entity {
public:
    Yeti(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:
    sf::Sprite _wavesSprite;
};

#endif

