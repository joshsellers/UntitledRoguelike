#ifndef _ENTITY_H
#define _ENTITY_H

#include <SFML/Graphics.hpp>
#include "Inventory.h"

constexpr int SPRITE_SHEET_SHIFT = 4;
constexpr int TILE_SIZE = 16;

constexpr int DEFAULT_DORMANCY_TIMEOUT = 60 * 30;
constexpr int DEFAULT_MAX_TIME_OUT_OF_CHUNK = 60 * 15;

enum MOVING_DIRECTION {
    UP = 2,
    DOWN = 0,
    LEFT = 1,
    RIGHT = 3
};

class Entity {
public:
    Entity(sf::Vector2f pos, float baseSpeed, const int spriteWidth, const int spriteHeight, const bool isProp);

    virtual void update() = 0;

    virtual void draw(sf::RenderTexture& surface) = 0;

    virtual void move(float xa, float ya);
    bool isMoving() const;
    MOVING_DIRECTION getMovingDir() const;

    void setBaseSpeed(float speed);
    float getBaseSpeed() const;

    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet) = 0;

    bool isMob() const;
    bool isProp() const;
    bool canPickUpItems() const;

    sf::Sprite getSprite() const;

    bool displayBottom() const;

    sf::Vector2i getSpriteSize() const;

    Inventory& getInventory();

    void setWorld(World* world);
    World* getWorld() const;

    bool isActive() const;
    void deactivate();

    bool isDormant() const;
    void setDormant(bool dormant);
    int getDormancyTimeout() const;
    int getDormancyTime() const;
    void resetDormancyTimer();
    void incrementDormancyTimer();

    int getMaxTimeOutOfChunk() const;
    int getTimeOutOfChunk() const;
    void resetOutOfChunkTimer();
    void incrementOutOfChunkTimer();

    bool isDamageable() const;

    void setMaxHitPoints(int maxHitPoints);
    int getMaxHitPoints() const;
    int& getMaxHitPointsRef();
    virtual void damage(int damage);
    void heal(int hitPoints);
    int getHitPoints() const;
    int& getHitPointsRef();

    sf::FloatRect getHitBox() const;

    sf::Vector2f getCalculatedBarrelPos() const;
    sf::Vector2f getTargetPos() const;

protected:
    const int _spriteWidth, _spriteHeight;

    bool _displayBottom = false;

    sf::Sprite _sprite;

    float _baseSpeed = 0;

    sf::Vector2f _pos;
    sf::Vector2f _velocity;
    int _numSteps = 0;
    int _animSpeed = 3;

    bool _isMoving = false;
    unsigned int _movingDir = DOWN;

    bool _isMob = false;
    const bool _isProp = false;
    bool _canPickUpItems = false;

    Inventory _inventory = Inventory(this);

    World* _world = nullptr;

    bool _isActive = true;
    bool _isDormant = false;

    int _dormancyTimer = 0;
    int _dormancyTimeout = DEFAULT_DORMANCY_TIMEOUT;

    int _outOfChunkTimer = 0;
    int _maxTimeOutOfChunk = DEFAULT_MAX_TIME_OUT_OF_CHUNK;

    sf::Vector2f _calculatedBarrelPos;
    sf::Vector2f _targetPos;

    sf::FloatRect _hitBox;
    float _hitBoxXOffset = 0;
    float _hitBoxYOffset = 0;

    int _hitPoints = 0;

private:
    int _maxHitPoints = 0;
};
#endif // !_ENTITY_H
