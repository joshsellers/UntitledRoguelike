#ifndef _LARGE_SAVANNA_TREE_H
#define _LARGE_SAVANNA_TREE_H

#include "Entity.h"

constexpr int LARGE_SAVANNA_TREE_SPRITE_POS_X = 11 << SPRITE_SHEET_SHIFT;
constexpr int LARGE_SAVANNA_TREE_SPRITE_POS_Y = 0;

class LargeSavannaTree : public Entity {
public:
    LargeSavannaTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
};

#endif