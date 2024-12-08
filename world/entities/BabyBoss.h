#ifndef _BABY_BOSS_H
#define _BABY_BOSS_H

#include "Boss.h"

class BabyBoss : public Boss {
public:
    BabyBoss(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    void subUpdate();
    void onStateChange(const BossState previousState, const BossState newState);
    void runCurrentState();

    enum BEHAVIOR_STATE {
        REST,
        SPAM_BOMBS,
        DROP_NUKES,
        SPAWN_SOLDIERS
    };
private:
    sf::Sprite _wavesSprite;

    long long _lastContactDamageTimeMillis = 0LL;

    float _fireAngle = 0;
    long long _lastFireTimeMillis = 0;

    int _soldiersToSpawn = 0;
    int _soldiersSpawned = 0;
    long long _timeToNextSoldierSpawn = 0;
    long long _lastSoldierSpawnTime = 0;

    bool _spawnedNukes = false;

    void blink();
    bool _isBlinking = false;
    long long _blinkStartTime = 0LL;
};

#endif