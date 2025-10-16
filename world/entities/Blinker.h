#ifndef _BLINKER_H
#define _BLINKER_H

#include "Bouncer.h"

class Blinker : public Bouncer {
public:
    Blinker(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    void preupdate();

    void damage(int damage);
private:
    bool _isBlinking = false;

    long long _spawnTimeMillis = 0LL;
    bool _spawnCooldownActive = true;
};

#endif