#ifndef _ORBITER_H
#define _ORBITER_H

#include <memory>
#include "Entity.h"
#include "OrbiterType.h"

class Orbiter : public Entity {
public:
    Orbiter(sf::Vector2f pos, const unsigned int orbiterTypeId, Entity* parent);

    virtual void update();

    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    const unsigned int getOrbiterTypeId() const;

    float getAngle() const;
    float getDistance() const;
    float getSpeed() const;

    Entity* getParent() const;
protected:
    void attack();
    void projectileAttack();
    void contactAttack();

    const unsigned int _orbiterTypeId;
    const OrbiterType* _orbiterType;

    float _angle = 0;
    float _distance;
    float _speed;

    void fireTargetedProjectile(sf::Vector2f targetPos, const ProjectileData projData, std::string soundName);
    long long _lastFireTime = 0LL;
private:
    Entity* _parent;
};

#endif

