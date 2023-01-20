#ifndef _PROJECTILE_DATA_H
#define _PROJECTILE_DATA_H

#include <SFML/Graphics/Rect.hpp>

struct ProjectileData {
    ProjectileData(const unsigned int itemId, const float baseVelocity, const sf::IntRect hitBox, const bool rotateSprite) :
        itemId(itemId), baseVelocity(baseVelocity), hitBox(hitBox), rotateSprite(rotateSprite) {
    }

    const unsigned int itemId;
    const float baseVelocity;
    const sf::IntRect hitBox;
    const bool rotateSprite;
};

#endif // !_PROJECTILE_DATA_H
