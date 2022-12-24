#include "Player.h"
#include "World.h"
#include <iostream>

Player::Player(sf::Vector2f pos) {
    _pos = pos;
    
    _texture.create(64, 128);
    if (!_texture.loadFromFile("res/url_guy_walking-Sheet.png")) {
        std::cout << "failed to load player texture" << std::endl;
    }
    _sprite.setTexture(_texture);
    _sprite.setTextureRect(sf::IntRect(0, 0, 16, 32));
    _sprite.setPosition(pos);
}

void Player::update() {
    float xa = 0, ya = 0;
    //if (!_isDodging) {
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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            xa *= _sprintMultiplier;
            ya *= _sprintMultiplier;
        }
    //}

    if (!_isDodging && sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && _dodgeKeyReleased) {
        _isDodging = true;
        _dodgeTimer++;
    } else if (_isDodging && _dodgeTimer < _maxDodgeTime) {
        _dodgeSpeedMultiplier = _dodgeMultiplier(_dodgeTimer);
        _dodgeTimer++;
    } else if (_isDodging && _dodgeTimer >= _maxDodgeTime) {
        _isDodging = false;
        _dodgeTimer = 0;
        _dodgeSpeedMultiplier = 1.f;
    }

    _dodgeKeyReleased = !sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

    move(xa * _dodgeSpeedMultiplier, ya * _dodgeSpeedMultiplier);

    _sprite.setPosition(getPosition());
}

void Player::draw(sf::RenderTexture& surface) {
    TERRAIN_TYPE terrainType = /*_world->getTerrainDataAt(_world->getCurrentChunk(), getPosition())*/ TERRAIN_TYPE::NOT_WATER;

    int yOffset = _isMoving ? ((_numSteps >> _animSpeed) & 3) * 32 : 0;
    _sprite.setTextureRect(sf::IntRect(16 * _movingDir, 0 + yOffset, 16, terrainType == TERRAIN_TYPE::WATER ? 16 : 32));
    surface.draw(_sprite);
}

void Player::move(int xa, int ya) {
    _pos.x += xa;
    _pos.y += ya;
    if (std::abs(xa) > 0 || std::abs(ya) > 0) {
        _numSteps++;
        _isMoving = true;
    } else _isMoving = false;
}

sf::Vector2f Player::getPosition() {
    return _pos;
}

void Player::setWorld(World* world) {
    _world = world;
}
