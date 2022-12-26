#ifndef _PLAYER_H
#define _PLAYER_H

#include <SFML/Graphics.hpp>
#include "Entity.h"

constexpr int PLAYER_WIDTH = 16;
constexpr int PLAYER_HEIGHT = 32;

constexpr float BASE_PLAYER_SPEED = 1;

class World;

class Player : public Entity {
public:
    Player(sf::Vector2f pos);

    virtual void update();

    virtual void draw(sf::RenderTexture& surface);

    virtual void move(float xa, float ya);

    sf::Vector2f getPosition() const;

    void setWorld(World* world);

    bool isSwimming() const;
    bool isDodging() const;

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

private:
    World* _world;

    sf::Sprite _wavesSprite;

    bool _isSwimming = false;

    float _sprintMultiplier = 2;

    float _dodgeMultiplier(int dodgeTimer) const {
        return (0.075 * (float)(std::pow((float)dodgeTimer, 2) - 4) + (float)2);
    }

    bool _isDodging = false;
    int _dodgeTimer = 0;
    int _maxDodgeTime = 10;
    float _dodgeSpeedMultiplier = 1.f;
    bool _dodgeKeyReleased = true;
};

#endif