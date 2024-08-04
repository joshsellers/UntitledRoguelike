#ifndef _BOSS_H
#define _BOSS_H

#include "Entity.h"

struct BossState {
    BossState(int stateId, long long minLength, long long maxLength) {
        this->stateId = stateId;
        this->minLength = minLength;
        this->maxLength = maxLength;
    }

    int stateId;
    long long minLength;
    long long maxLength;
};

class Boss : public Entity {
public:
    Boss(ENTITY_SAVE_ID saveId, sf::Vector2f pos, float baseSpeed, const int spriteWidth, const int spriteHeight, std::vector<BossState> bossStates);

    void update();

protected:
    virtual void subUpdate() = 0;
    virtual void onStateChange(const BossState previousState, const BossState newState) = 0;
    virtual void runCurrentState() = 0;

    std::vector<BossState> _bossStates;
    const int _numBossStates;
    BossState _currentState;

    long long _currentStateLength = 1000LL;
    long long _lastStateChangeTime = 0LL;
private:
    void changeState();
};

#endif

