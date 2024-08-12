#ifndef _CANNON_BOSS_H
#define _CANNON_BOSS_H

#include "Boss.h"

class CannonBoss : public Boss {
public:
    CannonBoss(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    void subUpdate();
    void onStateChange(const BossState previousState, const BossState newState);
    void runCurrentState();

    enum BEHAVIOR_STATE {
        REST,
        RING_OF_BLOOD,
        TARGETED_FIRE
    };
private:
    sf::Sprite _wavesSprite;

    long long _lastContactDamageTimeMillis = 0LL;

    const long long _fireRateMillis = 150LL;
    long long _lastFireTimeMillis = 0LL;

    int _currentOrbiterCount = 0;
    long long _lastOrbiterSpawnTime = 0LL;
};

#endif