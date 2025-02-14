#ifndef _MUSHROOM_BOSS_H
#define _MUSHROOM_BOSS_H

#include "Boss.h"

class MushroomBoss : public Boss {
public:
    MushroomBoss(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    void subUpdate();
    void onStateChange(const BossState previousState, const BossState newState);
    void runCurrentState();

    enum BEHAVIOR_STATE {
        SPAWN_SHROOMS,
        RAPID_FIRE_SHROOMS,
        BIGSHROOM_SNIPE
    };
private:
    sf::Sprite _wavesSprite;

    long long _lastContactDamageTimeMillis = 0LL;

    long long _lastFireTimeMillis = 0LL;
    float _fireAngle;

    long long _lastShroomSpawnTime = 0LL;
    int _numShroomsSpawned = 0;
};

#endif