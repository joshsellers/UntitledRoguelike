#include "Player.h"
#include <iostream>

Player::Player(sf::Vector2f pos) {
    _pos = pos;
    
    _texture.create(64, 32);
    if (!_texture.loadFromFile("res/url_guy_standing-Sheet.png")) {
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
    _sprite.setTextureRect(sf::IntRect(16 * _movingDir, 0, 16, 32));
    surface.draw(_sprite);
}

void Player::move(int xa, int ya) {
    _pos.x += xa;
    _pos.y += ya;
}

sf::Vector2f Player::getPosition() {
    return _pos;
}
