#ifndef _CREAM_BOSS_H
#define _CREAM_BOSS_H

#include "Boss.h"

class CreamBoss : public Boss {
public:
    CreamBoss(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    void subUpdate();
    void onStateChange(const BossState previousState, const BossState newState);
    void runCurrentState();

    enum BEHAVIOR_STATE {
        REST,
        CREAM_DROP,
        CREAM_RING_IN,
        CREAM_RING_OUT
    };
private:
    sf::Sprite _wavesSprite;

    long long _lastContactDamageTimeMillis = 0LL;

    const int _maxLayers = 10;
    int _currentLayerCount = 0;
    long long _lastLayerSpawnTime = 0LL;
};

#endif