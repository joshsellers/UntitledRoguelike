#ifndef _PROJECTILE_DATA_H
#define _PROJECTILE_DATA_H

#include <SFML/Graphics/Rect.hpp>
#include "ProjectileDataManager.h"

struct ProjectileData {
    ProjectileData(const unsigned int itemId, const float baseVelocity, const sf::IntRect hitBox, const bool rotateSprite, const bool onlyHitEnemies = false,
        const long long lifeTime = 5000LL,
        const bool isAnimated = false, const int animationFrames = 0, const int animationSpeed = 0, const bool dropOnExpire = false) :
        itemId(itemId), baseVelocity(baseVelocity), hitBox(hitBox), rotateSprite(rotateSprite), onlyHitEnemies(onlyHitEnemies), lifeTime(lifeTime),
        isAnimated(isAnimated), animationFrames(animationFrames), animationSpeed(animationSpeed), dropOnExpire(dropOnExpire) {

        ProjectileDataManager::addData(*this);
    }

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
};

#endif 
