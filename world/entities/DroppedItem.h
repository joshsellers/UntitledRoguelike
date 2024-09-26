#ifndef _DROPPED_ITEM_H
#define _DROPPED_ITEM_H

#include "Entity.h"

class DroppedItem : public Entity {
public:
    DroppedItem(sf::Vector2f pos, float originOffset, unsigned int itemId, unsigned int amount, sf::IntRect textureRect, bool droppedByPlayer = false);

    void update();
    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    virtual std::string getSaveData() const;

private:
    const unsigned int _itemId;
    unsigned int _amount;
    const sf::IntRect _textureRect;

    const float _hoverDist = 5.f;
    const float _minY;
    const float _originalY;
    bool _down = true;
    int _animCounter = 0;

    void moveTowardPlayer();
};

#endif