#ifndef _CHEESE_BOSS_H
#define _CHEESE_BOSS_H

#include "Entity.h"

class CheeseBoss : public Entity {
public:
    CheeseBoss(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
private:
    sf::Sprite _wavesSprite;

    void blink();
    bool _isBlinking = false;
    long long _blinkStartTime = 0LL;
};

#endif

