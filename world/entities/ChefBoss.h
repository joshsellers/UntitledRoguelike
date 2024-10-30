#ifndef _CHEF_BOSS_H
#define _CHEF_BOSS_H

#include "Boss.h"

class ChefBoss : public Boss {
public:
    ChefBoss(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    void subUpdate();
    void onStateChange(const BossState previousState, const BossState newState);
    void runCurrentState();

    enum BEHAVIOR_STATE {
        REST,
        PIZZA_RING,
        KNIFE_ATTACK,
        SPAWN_BURGERS
    };
private:
    sf::Sprite _tearsSprite;

    long long _lastContactDamageTimeMillis = 0LL;

    int _animCounter = 0;

    int _orbiterCount = 0;
    long long _lastOrbiterSpawnTime = 0;

    float _fireAngle = 0;
    long long _lastFireTimeMillis = 0;

    int _burgersToSpawn = 0;
    int _burgersSpawned = 0;
    long long _timeToNextBurgerSpawn = 0;
    long long _lastBurgerSpawnTime = 0;
};

#endif