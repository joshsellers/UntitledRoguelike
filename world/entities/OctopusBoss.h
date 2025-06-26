#ifndef _OCTOPUS_BOSS_H
#define _OCTOPUS_BOSS_H

#include "Boss.h"

class OctopusBoss : public Boss {
public:
    OctopusBoss(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    void subUpdate();
    void onStateChange(const BossState previousState, const BossState newState);
    void runCurrentState();

    enum BEHAVIOR_STATE {
        REST,
        INK_BUBBLE,
        INK_RAY
    };
private:
    int _animCounter = 0;

    long long _lastContactDamageTimeMillis = 0LL;

    long long _lastFireTimeMillis = 0LL;
};

#endif