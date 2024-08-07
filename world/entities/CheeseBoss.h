#ifndef _CHEESE_BOSS_H
#define _CHEESE_BOSS_H

#include "Boss.h"

class CheeseBoss : public Boss {
public:
    CheeseBoss(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    void subUpdate();
    void onStateChange(const BossState previousState, const BossState newState);
    void runCurrentState();

    enum BEHAVIOR_STATE {
        REST,
        RING_OF_CHEESE,
        RAPID_FIRE
    };
private:
    sf::Sprite _wavesSprite;

    long long _lastContactDamageTimeMillis = 0LL;

    float _fireAngle = 0.f;
    const long long _fireRateMillis = 250LL;
    long long _lastFireTimeMillis = 0LL;

    void blink();
    bool _isBlinking = false;
    long long _blinkStartTime = 0LL;
};

#endif

