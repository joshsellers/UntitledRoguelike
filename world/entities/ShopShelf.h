#ifndef _SHOP_SHELF_H
#define _SHOP_SHELF_H

#include "Entity.h"

class ShopShelf : public Entity {
public:
    ShopShelf(sf::Vector2f pos);

    void update();
    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    virtual void setWorld(World* world);
private:
    sf::Sprite _itemSprite;
    std::vector<int> _items;
};

#endif