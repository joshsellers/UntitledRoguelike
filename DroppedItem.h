#ifndef _DROPPED_ITEM_H
#define _DROPPED_ITEM_H

#include "Entity.h"

class DroppedItem : public Entity {
public:
    DroppedItem(sf::Vector2f pos, unsigned int itemId, sf::IntRect textureRect);

    void update();
    void draw(sf::RenderTexture& surface);

    void move(float xa, float ya);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

private:
    const unsigned int _itemId;
    const sf::IntRect _textureRect;
};

#endif