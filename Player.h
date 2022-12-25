#ifndef _PLAYER_H
#define _PLAYER_H

#include <SFML/Graphics.hpp>

constexpr int PLAYER_WIDTH = 16;
constexpr int PLAYER_HEIGHT = 32;

constexpr float BASE_PLAYER_SPEED = 1;

class World;

class Player {
public:
    Player(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void move(float xa, float ya);

    sf::Vector2f getPosition() const;

    void setWorld(World* world);

    bool isSwimming() const;
    bool isMoving() const;
    bool isDodging() const;

private:
    World* _world;

    sf::Vector2f _pos;

    sf::Texture _texture;
    sf::Sprite _sprite;

    bool _isSwimming;

    float _sprintMultiplier = 2;
    float _dodgeMultiplier(int dodgeTimer) const {
        //return (1.075 * (float)std::pow((float)dodgeTimer - 5.f, 2) + (float)0.5);
        return (0.075 * (float)(std::pow((float)dodgeTimer, 2) - 4) + (float)2);
    }

    bool _isDodging = false;
    int _dodgeTimer = 0;
    int _maxDodgeTime = 10;
    float _dodgeSpeedMultiplier = 1.f;
    bool _dodgeKeyReleased = true;

    int _numSteps = 0;
    int _animSpeed = 3;

    unsigned int _movingDir = 0;
    bool _isMoving = false;
};

#endif