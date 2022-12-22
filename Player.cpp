#include "Player.h"

Player::Player(sf::Vector2f pos) {
    _pos = pos;

    tempSprite.setFillColor(sf::Color(0xAAAA00FF));
}

void Player::update() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        move(0, -2);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        move(0, 2);
    } 
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        move(-2, 0);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        move(2, 0);
    }

    tempSprite.setPosition(_pos);
}

void Player::draw(sf::RenderTexture& surface) {
    surface.draw(tempSprite);
}

void Player::move(int xa, int ya) {
    _pos.x += xa;
    _pos.y += ya;
}

sf::Vector2f Player::getPosition() {
    return _pos;
}
