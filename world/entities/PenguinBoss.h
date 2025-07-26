#ifndef _PENGUIN_BOSS_H
#define _PENGUIN_BOSS_H

#include "Boss.h"

class PenguinBoss : public Boss {
public:
    PenguinBoss(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    void subUpdate();
    void onStateChange(const BossState previousState, const BossState newState);
    void runCurrentState();

    enum BEHAVIOR_STATE {
        PSYCH_LASER,
        HOMING_SNOWBALLS,
        SPAWN_PENGUINS
    };
private:
    sf::Sprite _wavesSprite;

    long long _lastContactDamageTimeMillis = 0LL;

    long long _lastFireTimeMillis = 0LL;

    unsigned int _penguinsSpawned = 0;

    bool _isChargingLaser = false;
    bool _firedLaser = false;
    long long _laserChargeStartTime = 0LL;
    sf::Sprite _laserEyesSprite;
    int _animCounter = 0;
};

#endif