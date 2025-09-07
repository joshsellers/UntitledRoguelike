#ifndef _FROG_BOSS_H
#define _FROG_BOSS_H

#include "Boss.h"

class FrogBoss : public Boss {
public:
    FrogBoss(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    void subUpdate();
    void onStateChange(const BossState previousState, const BossState newState);
    void runCurrentState();

    enum BEHAVIOR_STATE {
        REST,
        JUMP,
        SPAWN_ORBITERS,
        SHOOT_FROGS
    };
private:
    sf::Sprite _wavesSprite;
    sf::Sprite _headSprite;
    int _headFrame = 0;
    int _animCounter = 0;

    long long _lastContactDamageTimeMillis = 0LL;

    float _fireAngle = 0;
    long long _lastFireTimeMillis = 0;

    enum JUMP_STATE {
        CHARGING,
        RISING,
        FALLING,
        LANDED
    };

    JUMP_STATE _jumpState = CHARGING;
    int _jumpFrame = 0;
    sf::Vector2f _landingTarget;

    bool _orbitersSpawned = false;
};

#endif