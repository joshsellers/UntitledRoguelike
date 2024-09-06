#ifndef _TULIP_MONSTER_H
#define _TULIP_MONSTER_H

#include "Entity.h"

constexpr long long TULIP_MONSTER_FIRE_RATE = 500LL;

class TulipMonster : public Entity {
public:
    TulipMonster(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
private:
    sf::Sprite _wavesSprite;

    long long _lastFireTime = 0;
};

#endif