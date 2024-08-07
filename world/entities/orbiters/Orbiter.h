#ifndef _ORBITER_H
#define _ORBITER_H

#include <memory>
#include "../Entity.h"
#include "OrbiterType.h"

class Orbiter : public Entity {
public:
    Orbiter(float angle, const unsigned int orbiterTypeId, Entity* parent);

    virtual void update();

    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    const unsigned int getOrbiterTypeId() const;

    float getAngle() const;
    float getDistance() const;
    float getSpeed() const;

    void setCenterPointOffset(float xOffset, float yOffset);
    void setCenterPointOffset(sf::Vector2f offset);

    Entity* getParent() const;

    virtual std::string getSaveData() const;

    friend class OrbiterType;
protected:
    void attack();
    void projectileAttack();
    void contactAttack();

    const unsigned int _orbiterTypeId;
    const OrbiterType* _orbiterType;

    float _angle = 0;
    float _distance;
    float _speed;

    sf::Vector2f _centerPointOffset;
    bool _centerPointOffsetWasReset = false;

    void fireTargetedProjectile(sf::Vector2f targetPos, const ProjectileData projData, std::string soundName);
    void fireTargetedProjectile(float angle, const ProjectileData projData, std::string soundName);
    long long _lastFireTime = 0LL;
private:
    Entity* _parent;
};

#endif

