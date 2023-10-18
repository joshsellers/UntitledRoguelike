#include "Turtle.h"

#include <boost/random/uniform_int_distribution.hpp>
#include "World.h"
#include "Util.h"

Turtle::Turtle(sf::Vector2f pos) :
    Entity(pos, 0.25, 1, 1, false) {
    _gen.seed(currentTimeNano());

    setMaxHitPoints(15);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE / 2;
    _hitBoxYOffset = 6;
    _hitBox.width = TILE_SIZE;
    _hitBox.height = 10;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = true;
    _isMob = true;

    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE;
    _wanderTargetPos = feetPos;
}

void Turtle::update() {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE;

    wander(feetPos, _gen);

    _sprite.setPosition(getPosition());
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void Turtle::draw(sf::RenderTexture& surface) {
    int xOffset = getMovingDir() * TILE_SIZE;
    int yOffset = isMoving() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE : 0;

    _sprite.setTextureRect(sf::IntRect(
        8 * TILE_SIZE + xOffset, 8 * TILE_SIZE + yOffset, TILE_SIZE, TILE_SIZE
    ));

    surface.draw(_sprite);
}

void Turtle::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}

void Turtle::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(8 * TILE_SIZE, 8 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE / 2, 0);
}