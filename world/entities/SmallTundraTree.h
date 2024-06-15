#ifndef _SMALL_TUNDRA_TREE_H
#define _SMALL_TUNDRA_TREE_H

#include "Entity.h"

constexpr int S_TUNDRA_TREE_SPRITE_POS_X = 19;
constexpr int S_TUNDRA_TREE_SPRITE_POS_Y = 8;

class SmallTundraTree : public Entity {
public:
    SmallTundraTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
};

#endif