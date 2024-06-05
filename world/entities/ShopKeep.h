#ifndef _SHOP_KEEP_H
#define _SHOP_KEEP_H

#include "Entity.h"
#include "../../inventory/ShopManager.h"

class ShopKeep : public Entity {
public:
    ShopKeep(sf::Vector2f pos, ShopManager* shopManager, std::shared_ptr<sf::Texture> spriteSheet);

    void initInventory();

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    void setPosition(sf::Vector2f pos);

private:
    sf::Sprite _clothingHeadSprite;
    sf::Sprite _clothingBodySprite;
    sf::Sprite _clothingLegsSprite;
    sf::Sprite _clothingFeetSprite;

    void drawApparel(sf::Sprite& sprite, EQUIPMENT_TYPE equipType, sf::RenderTexture& surface);

    ShopManager* _shopManager;
};

#endif