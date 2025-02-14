#ifndef _SHOP_ATM_H
#define _SHOP_ATM_H

#include "Entity.h"

class ShopATM : public Entity {
public:
    ShopATM (sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

private:
    static const unsigned int _numDigits = 3;
    sf::Sprite _digitSprites[_numDigits];

    unsigned int _lastATMValue = 0;

    void setDigits();

    long long _lastTouchTime = 0LL;
};

#endif