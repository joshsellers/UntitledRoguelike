#ifndef _PROJECTILE_H
#define _PROJECTILE_H

#include "../Entity.h"
#include "ExplosionBehavior.h"

class Projectile : public Entity {
public:
    Projectile(sf::Vector2f pos, Entity* parent, float directionAngle, float velocity, const ProjectileData data, bool onlyHitPlayer = false, 
        int damageBoost = 0, bool addParentVelocity = true, EXPLOSION_BEHAVIOR explosionBehavior = EXPLOSION_BEHAVIOR::DEFER_TO_DATA);

    virtual void update();
    void draw(sf::RenderTexture& surface);

    void setSplitInto(const std::string projectileDataIdentifier, const bool splitOnHit, const bool splitOnDecay, const unsigned int projectileCount);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    virtual std::string getSaveData() const;

    bool onlyDamagePlayer = false;

    bool optimizedExplosions = false;

    int passThroughCount = 1;

    bool bounceOffViewport = false;

    void setCrit(const bool crit);

    friend class ProjectilePoolManager;
protected:
    Entity* _parent;

    sf::Vector2f _originalPos;

    float _directionAngle;
    float _velocity;
    sf::Vector2f _velocityComponents;

    unsigned int _itemId;

    long long _spawnTime;
    long long _lifeTime;

    int _currentTime = 0;

    ProjectileData _data;
    EXPLOSION_BEHAVIOR _explosionBehavior;

    void spawnExplosion() const;

    int _animationTime = 0;

    int _damageBoost = 0;

    int _entitiesPassedThrough = 0;
    std::vector<std::string> _hitEntities;

    void split() const;
    std::string _splitProjectileDataIdentifer = "";
    bool _splitOnHit = false;
    bool _splitOnDecay = false;
    unsigned int _splitProjectileCount = 0;

    bool _criticalHit = false;

    void reset(sf::Vector2f pos, Entity* parent, float directionAngle, float velocity, const ProjectileData data, bool onlyHitPlayer = false, 
        int damageBoost = 0, bool addParentVelocity = true, int passThroughCount = 1, EXPLOSION_BEHAVIOR explosionBehavior = EXPLOSION_BEHAVIOR::DEFER_TO_DATA);
};

#endif 
