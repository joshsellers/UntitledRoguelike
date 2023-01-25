#include "PlantMan.h"
#include <boost/random/uniform_int_distribution.hpp>
#include "World.h"
#include "Util.h"

PlantMan::PlantMan(sf::Vector2f pos) :
    Entity(pos, 1, 1, 1, false) {
    _gen.seed(currentTimeNano());

    setMaxHitPoints(75);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE / 2;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE;
    _hitBox.height = TILE_SIZE * 3;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = true;
    _isMob = true;
}

void PlantMan::update() {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 3;

    wander(feetPos, _gen);

    _sprite.setPosition(getPosition());
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void PlantMan::draw(sf::RenderTexture& surface) {
    int xOffset = getMovingDir() * TILE_SIZE;
    int yOffset = isMoving() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * 3: 0;

    _sprite.setTextureRect(sf::IntRect(
        24 * TILE_SIZE + xOffset, 13 * TILE_SIZE + yOffset, TILE_SIZE, TILE_SIZE * 3
    ));

    surface.draw(_sprite);
}

void PlantMan::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}

void PlantMan::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(24 * TILE_SIZE, 13 * TILE_SIZE, TILE_SIZE, TILE_SIZE * 3));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE / 2, 0);
}
