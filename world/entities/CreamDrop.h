#ifndef _CREAM_DROP_H
#define _CREAM_DROP_H

#include "Entity.h"

class CreamDrop : public Entity {
public:
    CreamDrop(sf::Vector2f playerPos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:
    sf::Sprite _dropSprite;

    sf::Vector2f _impactPos;
};

#endif