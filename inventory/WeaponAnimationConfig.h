#ifndef _WEAPON_ANIMATION_CONFIG_H
#define _WEAPON_ANIMATION_CONFIG_H

struct WeaponAnimationConfig {
    WeaponAnimationConfig(const unsigned int itemId, const unsigned int ticksPerFrame, const unsigned int frameCount) :
    itemId(itemId), ticksPerFrame(ticksPerFrame), frameCount(frameCount)
    {}

    unsigned int itemId;
    unsigned int ticksPerFrame;
    unsigned int frameCount;
};

#endif