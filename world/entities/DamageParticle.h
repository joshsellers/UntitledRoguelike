#ifndef _DAMAGE_PARTICLE_H
#define _DAMAGE_PARTICLE_H

#include "Entity.h"

constexpr long long DAMAGE_PARTICLE_LIFETIME = 500LL;

class DamageParticle : public Entity {
public:
    DamageParticle(sf::Vector2f pos, unsigned int damageAmount);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

private:
    long long _spawnTimeMillis;
    const unsigned int _amount;

    std::vector<sf::Sprite> _sprites;
};

#endif
