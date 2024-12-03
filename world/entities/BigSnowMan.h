#ifndef _BIG_SNOWMAN
#define _BIG_SNOWMAN

#include "Entity.h"

class BigSnowMan : public Entity {
public:
    BigSnowMan(sf::Vector2f pos);

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