#ifndef _DEV_BOSS_H
#define _DEV_BOSS_H

#include "Boss.h"
#include "DevBossCmdLine.h"

struct DevBossCommand {
    DevBossCommand(std::string text, bool onlyRunOnce = false) : text(text), onlyRunOnce(onlyRunOnce) {}

    const std::string text;
    const bool onlyRunOnce;
};

class DevBoss : public Boss {
public:
    DevBoss(sf::Vector2f pos);

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    virtual void setWorld(World* world);
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

    long long _lastProjectileFireTimeMillis = 0LL;

    long long _lastPackSpawnTime = 0LL;
    int _maxPackAmount = 5;
    int _numPacksSpawned = 0;
    void spawnPack();

    DevBossCmdLine _cmdLine;
    enum COMMAND {
        NONE = -1,
        SPAWN_ENEMIES,
        RESEED,
        SLOW_BULLETS,
        ANTIHOMING_BULLETS
    } _currentCommand = NONE;

    const std::map<COMMAND, DevBossCommand> _commands = {
        {SPAWN_ENEMIES, {"summonEnemies();"}},
        {RESEED, {"world.resetSeed();"}},
        {SLOW_BULLETS, {"player.projectiles.setVelocity(1.0);"}},
        {ANTIHOMING_BULLETS, {"player.projectiles.avoidTarget = true;"}}
    };

    bool _ranCommand = false;
    void runCommand(COMMAND cmd);
};

#endif