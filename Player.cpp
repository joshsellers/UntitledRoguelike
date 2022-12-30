#include "Player.h"
#include "World.h"
#include <iostream>

Player::Player(sf::Vector2f pos) : Entity(pos, BASE_PLAYER_SPEED) {
    _pos = pos;
}

void Player::update() {
    float xa = 0, ya = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        ya = -BASE_PLAYER_SPEED;
        _movingDir = 2;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        ya = BASE_PLAYER_SPEED;
        _movingDir = 0;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        xa = -BASE_PLAYER_SPEED;
        _movingDir = 1;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        xa = BASE_PLAYER_SPEED;
        _movingDir = 3;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && !isDodging() && (!isSwimming() || NO_MOVEMENT_RESTRICIONS)) {
        xa *= _sprintMultiplier;
        ya *= _sprintMultiplier;
        _animSpeed = 2;
    } else if (!isSwimming() && isMoving()) {
        _animSpeed = 3;
    } else if (isSwimming() && !NO_MOVEMENT_RESTRICIONS) {
        _animSpeed = 4;
        xa /= 2;
        ya /= 2;
    }

    if ((!isSwimming() || NO_MOVEMENT_RESTRICIONS) && !isDodging() && sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && _dodgeKeyReleased) {
        _isDodging = true;
        _dodgeTimer++;
    } else if (isDodging() && _dodgeTimer < _maxDodgeTime) {
        _dodgeSpeedMultiplier = _dodgeMultiplier(_dodgeTimer) * _sprintMultiplier;
        _dodgeTimer++;
    } else if (isDodging() && _dodgeTimer >= _maxDodgeTime) {
        _isDodging = false;
        _dodgeTimer = 0;
        _dodgeSpeedMultiplier = 1.f;
    }

    _dodgeKeyReleased = !sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

    xa += 15;

    move(xa * _dodgeSpeedMultiplier, ya * _dodgeSpeedMultiplier);

    _sprite.setPosition(getPosition());
}

void Player::draw(sf::RenderTexture& surface) {
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        _world->getCurrentChunk(), sf::Vector2f(((int)_pos.x + PLAYER_WIDTH / 2), ((int)_pos.y + PLAYER_HEIGHT))
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2));

        int xOffset = ((_numSteps >> _animSpeed) & 1) * 16;

        _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, 16, 16));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2 + 8));
        surface.draw(_wavesSprite);
    }

    int xOffset = isDodging() ? ((_numSteps >> (_animSpeed / 2)) & 3) * 16 : 0;
    int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * 32 : 0;
    _sprite.setTextureRect(sf::IntRect(
        isDodging() && isMoving() ? xOffset : 16 * _movingDir, 
        isDodging() && isMoving() ? 128 : 0 + yOffset, 
        16, 
        terrainType == TERRAIN_TYPE::WATER ? 16 : 32)
    );
    surface.draw(_sprite);
}

void Player::move(float xa, float ya) {
    if (std::abs(xa) > 0 || std::abs(ya) > 0) {
        _numSteps++;
        _isMoving = true;
    } else if (isSwimming()) {
        _numSteps++;
        _isMoving = false;
    } else _isMoving = false;

    _pos.x += xa;
    _pos.y += ya;
}

sf::Vector2f Player::getPosition() const {
    return _pos;
}

void Player::setWorld(World* world) {
    _world = world;
}

bool Player::isSwimming() const {
    return _isSwimming;
}

bool Player::isDodging() const {
    return _isDodging;
}

void Player::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(0, 0, 16, 32));
    _sprite.setPosition(getPosition());

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2));
}
