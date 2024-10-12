#ifndef _LIGHTNING_H
#define _LIGHTNING_H

#include "Entity.h"

class Lightning : public Entity {
public:
    Lightning(sf::Vector2f pos);
    Lightning(sf::Vector2f pos, int damage, float damageApothem);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:
    void init(sf::Vector2f pos, int damage, float damageApothem);

    long long _lifeTime = 0;

    long long _spawnTime = 0;
    long long _lastRedrawTime = 0;

    boost::random::mt19937 _rng = boost::random::mt19937();
    unsigned int _seed = 0;

    sf::FloatRect _hitBox;
    int _damage = 0;
    int _damageApothem = 0;

    long long _lastHitTime = 0;
};

#endif