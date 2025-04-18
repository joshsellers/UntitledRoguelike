#ifndef _TEETH_BOSS_H
#define _TEETH_BOSS_H

#include "Boss.h"

class TeethBoss : public Boss {
public:
    TeethBoss(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    void subUpdate();
    void onStateChange(const BossState previousState, const BossState newState);
    void runCurrentState();

    enum BEHAVIOR_STATE {
        CHARGE,
        EXPLODING_TEETH,
        TEETH_LAZER
    };
private:
    sf::Sprite _wavesSprite;

    long long _lastContactDamageTimeMillis = 0LL;

    long long _lastFireTimeMillis = 0LL;
    float _fireAngle;

    sf::Vector2f _chargeTarget;
    void resetChargeTarget();
};

#endif