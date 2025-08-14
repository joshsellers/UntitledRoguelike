#ifndef _DEV_BOSS_H
#define _DEV_BOSS_H

#include "Boss.h"

class DevBoss : public Boss {
public:
    DevBoss(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    void subUpdate();
    void onStateChange(const BossState previousState, const BossState newState);
    void runCurrentState();

    enum BEHAVIOR_STATE {
        RUN_COMMAND,
        FIRE_PROJECILE
    };
private:
    enum ANIMATION_STATE {
        WALKING,
        TYPING,
        HANDS_UP
    } _animationState = WALKING;

    sf::Sprite _headSprite;
    sf::Sprite _bodySprite;

    void blink();
    bool _isBlinking = false;
    long long _blinkStartTime = 0LL;
};

#endif