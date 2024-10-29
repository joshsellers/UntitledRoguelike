#ifndef _SHOP_KEEP_CORPSE
#define _SHOP_KEEP_CORPSE

#include "Entity.h"

class ShopKeepCorpse : public Entity {
public:
    ShopKeepCorpse(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

private:

};

#endif