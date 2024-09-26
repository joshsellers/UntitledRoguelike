#ifndef _TREE_BOSS_H
#define _TREE_BOSS_H

#include "Boss.h"

class TreeBoss :public Boss {
public:
    TreeBoss(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    void subUpdate();
    void onStateChange(const BossState previousState, const BossState newState);
    void runCurrentState();

    enum BEHAVIOR_STATE {
        CHASE,
        SHOOT_LOGS_0,
        SHOOT_LOGS_1
    };
private:
    sf::Sprite _wavesSprite;

    long long _lastContactDamageTimeMillis = 0LL;

    const long long _fireRateMillis = 450LL;
    long long _lastFireTimeMillis = 0LL;
};

#endif