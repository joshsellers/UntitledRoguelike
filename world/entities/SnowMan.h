#ifndef _SNOWMAN_H
#define _SNOWMAN_H
#include "Entity.h"

constexpr long long SNOWMAN_FIRE_RATE = 1000LL;

class SnowMan : public Entity {
public:
    SnowMan(sf::Vector2f pos);

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

