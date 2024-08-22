#include "Penguin.h"
#include <boost/random/uniform_int_distribution.hpp>
#include "../World.h"

Penguin::Penguin(sf::Vector2f pos) :
Entity(PENGUIN, pos, 0.5, 1, 1, false) {
    _gen.seed(currentTimeNano());

    setMaxHitPoints(20);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE / 2 + 3;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE - 6;
    _hitBox.height = TILE_SIZE;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;
    _isMob = true;

    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE;
    _wanderTargetPos = feetPos;
}

void Penguin::update() {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE;

    wander(feetPos, _gen);

    _sprite.setPosition(getPosition());
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void Penguin::draw(sf::RenderTexture& surface) {
    int xOffset = getMovingDir() == UP ? TILE_SIZE : 0;
    int yOffset = isMoving() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE : 0;

    _sprite.setTextureRect(sf::IntRect(
        8 * TILE_SIZE + xOffset, 4 * TILE_SIZE + yOffset, TILE_SIZE, TILE_SIZE
    ));
    if (getMovingDir() == LEFT) _sprite.setScale(-1, 1);
    else _sprite.setScale(1, 1);

    surface.draw(_sprite);
}

void Penguin::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}

void Penguin::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(8 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE / 2, 0);
}
