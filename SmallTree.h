#ifndef _SMALL_TREE_H
#define _SMALL_TREE_H

#include "Entity.h"

constexpr int STREE_SPRITE_POS_X = 4 << SPRITE_SHEET_SHIFT;
constexpr int STREE_SPRITE_POS_Y = 0;

class SmallTree : public Entity {
public:
    SmallTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    void damage(int damage);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
};

#endif