#ifndef _PROJECTILE_DATA_H
#define _PROJECTILE_DATA_H

#include <SFML/Graphics/Rect.hpp>
#include "ProjectileDataManager.h"
#include "ExplosionBehavior.h"

struct ProjectileData {
    ProjectileData(const unsigned int itemId, const float baseVelocity, const sf::IntRect hitBox, const bool rotateSprite, const bool onlyHitEnemies = false,
        const long long lifeTime = 5000LL,
        const bool isAnimated = false, const int animationFrames = 0, const int animationSpeed = 0, const bool dropOnExpire = false, const bool noCollide = false,
        const bool useDamageMultiplier = true, const EXPLOSION_BEHAVIOR explosionBehavior = EXPLOSION_BEHAVIOR::NOT_EXPLOSIVE) :
        itemId(itemId), baseVelocity(baseVelocity), hitBox(hitBox), rotateSprite(rotateSprite), onlyHitEnemies(onlyHitEnemies), lifeTime(lifeTime),
        isAnimated(isAnimated), animationFrames(animationFrames), animationSpeed(animationSpeed), dropOnExpire(false), noCollide(noCollide),
        useDamageMultiplier(useDamageMultiplier), explosionBehavior(explosionBehavior)
    {

        ProjectileDataManager::addData(*this);
    }

    ProjectileData()
        : itemId(0), baseVelocity(0), hitBox(sf::IntRect()), rotateSprite(false), onlyHitEnemies(true), lifeTime(0),
          isAnimated(false), animationFrames(0), animationSpeed(0), dropOnExpire(false), noCollide(false), useDamageMultiplier(false), 
          explosionBehavior(EXPLOSION_BEHAVIOR::NOT_EXPLOSIVE)
    { }

    unsigned int itemId;
    float baseVelocity;
    sf::IntRect hitBox;
    bool rotateSprite;
    bool onlyHitEnemies;
    long long lifeTime;
    bool isAnimated;
    int animationFrames;
    int animationSpeed;
    bool dropOnExpire; 
    bool noCollide;
    bool useDamageMultiplier;
    EXPLOSION_BEHAVIOR explosionBehavior;
};

#endif 
