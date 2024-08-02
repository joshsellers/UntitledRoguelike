#ifndef _PROJECTILE_H
#define _PROJECTILE_H

#include "../Entity.h"

class Projectile : public Entity {
public:
    Projectile(sf::Vector2f pos, Entity* parent, float directionAngle, float velocity, const ProjectileData data, bool onlyHitPlayer = false, int damageBoost = 0);

    void update();
    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    virtual std::string getSaveData() const;

    bool onlyDamagePlayer = false;

    int passThroughCount = 1;

private:
    Entity* _parent;

    const sf::Vector2f _originalPos;

    float _directionAngle;
    float _velocity;
    sf::Vector2f _velocityComponents;

    const unsigned int _itemId;

    long long _spawnTime;
    long long _lifeTime;

    float _currentTime = 0;

    const ProjectileData _data;

    int _animationTime = 0;

    int _damageBoost = 0;

    int _entitiesPassedThrough = 0;
    std::vector<std::string> _hitEntities;
};

#endif 
