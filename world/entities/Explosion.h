#ifndef _EXPLOSION_H
#define _EXPLOSION_H

#include "Entity.h"

class Explosion : public Entity {
public:
    Explosion(sf::Vector2f pos, bool onlyDamagePlayer = false, bool onlyDamageEnemies = false);
    Explosion(sf::Vector2f pos, int damage, bool onlyDamagePlayer = false, bool onlyDamageEnemies = false);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:
    void init(sf::Vector2f pos, int damage);

    int _damage = 0;

    int _animCounter = 0;
    int _currentFrame = 0;

    const bool _onlyDamagePlayer;
    const bool _onlyDamageEnemies;
};

#endif