#ifndef _PROJECTILE_DATA_H
#define _PROJECTILE_DATA_H

#include <SFML/Graphics/Rect.hpp>

struct ProjectileData {
    ProjectileData(const unsigned int itemId, const float baseVelocity, const sf::IntRect hitBox, const bool rotateSprite, const bool onlyHitEnemies = false,
        const long long lifeTime = 5000LL,
        const bool isAnimated = false, const int animationFrames = 0, const int animationSpeed = 0) :
        itemId(itemId), baseVelocity(baseVelocity), hitBox(hitBox), rotateSprite(rotateSprite), onlyHitEnemies(onlyHitEnemies), lifeTime(lifeTime),
        isAnimated(isAnimated), animationFrames(animationFrames), animationSpeed(animationSpeed) {
    }

    const unsigned int itemId;
    const float baseVelocity;
    const sf::IntRect hitBox;
    const bool rotateSprite;
    const bool onlyHitEnemies;
    const long long lifeTime;
    const bool isAnimated;
    const int animationFrames;
    const int animationSpeed;
};

#endif // !_PROJECTILE_DATA_H
