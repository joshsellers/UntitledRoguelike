#ifndef _BOULDER_BEAST_H
#define _BOULDER_BEAST_H

#include "Boss.h"

class BoulderBeast : public Boss {
public:
    BoulderBeast(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    void subUpdate();
    void onStateChange(const BossState previousState, const BossState newState);
    void runCurrentState();

    enum BEHAVIOR_STATE {
        CHASE,
        TARGETED_FIRE
    };
private:
    const long long _fireRateMillis = 1000LL;
    long long _lastFireTimeMillis = 0LL;

    long long _lastContactDamageTimeMillis = 0LL;

    bool _fireDiagonal = false;
};

#endif