#ifndef _FINGER_TREE_H
#define _FINGER_TREE_H

#include "Entity.h"

class FingerTree : public Entity {
public:
    FingerTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
};
#endif