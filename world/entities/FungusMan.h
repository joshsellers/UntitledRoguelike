#ifndef _FUNGUS_MAN_H
#define _FUNGUS_MAN_H

#include "Entity.h"

class FungusMan : public Entity {
public:
    FungusMan(sf::Vector2f pos);

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