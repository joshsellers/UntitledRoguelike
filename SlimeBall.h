#ifndef _SLIME_BALL_H
#define _SLIME_BALL_H

#include "Orbiter.h"

class SlimeBall : public Orbiter {
public:
    SlimeBall(sf::Vector2f pos, Entity* parent);


    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void subUpdate();

private:
    long long _lastFireTime = 0LL;
};

#endif
