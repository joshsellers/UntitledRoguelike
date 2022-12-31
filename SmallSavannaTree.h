#ifndef _SMALL_SAVANNA_TREE
#define _SMALL_SAVANNA_TREE

#include "Entity.h"

constexpr int SMALL_SAVANNA_TREE_SPRITE_POS_X = 8 << SPRITE_SHEET_SHIFT;
constexpr int SMALL_SAVANNA_TREE_SPRITE_POS_Y = 0;

class SmallSavannaTree : public Entity {
public:
    SmallSavannaTree(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
};

#endif 