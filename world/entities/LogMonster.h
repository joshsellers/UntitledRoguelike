#ifndef _LOG_MONSTER_H
#define _LOG_MONSTER_H

#include "Boss.h"

class LogMonster : public Boss {
public:
    LogMonster(sf::Vector2f pos);

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
    const long long _fireRateMillis = 250LL;
    long long _lastFireTimeMillis = 0LL;

    long long _lastContactDamageTimeMillis = 0LL;
};

#endif