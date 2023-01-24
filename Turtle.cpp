#include "Turtle.h"

#include <boost/random/uniform_int_distribution.hpp>
#include "World.h"
#include "Util.h"

Turtle::Turtle(sf::Vector2f pos) :
    Entity(pos, 0.25, 1, 1, false) {
    _targetPos = _pos;
    _gen.seed(_pos.x + _pos.y);

    setMaxHitPoints(20);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE / 2;
    _hitBoxYOffset = 6;
    _hitBox.width = TILE_SIZE;
    _hitBox.height = 10;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = true;
    _isMob = true;
}

void Turtle::update() {
    float xa = 0, ya = 0;
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE;

    boost::random::uniform_int_distribution<> shouldMove(0, 100);
    if (_targetPos == feetPos && shouldMove(_gen) == 0 ||
        _world->getTerrainDataAt(_targetPos) == TERRAIN_TYPE::WATER) {
        const int maxDist = _world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER ? 200 : 100;
        boost::random::uniform_int_distribution<> xDist(-maxDist, maxDist);
        boost::random::uniform_int_distribution<> yDist(-maxDist, maxDist);
        _targetPos.x = feetPos.x + xDist(_gen);
        _targetPos.y = feetPos.y + yDist(_gen);
    } else if (feetPos.x < _targetPos.x) {
        if (_world->getTerrainDataAt(feetPos.x + getBaseSpeed(), feetPos.y) == TERRAIN_TYPE::WATER &&
            _world->getTerrainDataAt(feetPos) != TERRAIN_TYPE::WATER) {
            _targetPos = feetPos;
        } else {
            if (_world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER) _targetPos == feetPos;
            xa += getBaseSpeed();
            _movingDir = RIGHT;
        }
    } else if (feetPos.x > _targetPos.x) {
        if (_world->getTerrainDataAt(feetPos.x - getBaseSpeed(), feetPos.y) == TERRAIN_TYPE::WATER &&
            _world->getTerrainDataAt(feetPos) != TERRAIN_TYPE::WATER) {
            _targetPos = feetPos;
        } else {
            if (_world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER) _targetPos == feetPos;
            xa -= getBaseSpeed();
            _movingDir = LEFT;
        }
    } else if (feetPos.y < _targetPos.y) {
        if (_world->getTerrainDataAt(feetPos.x, feetPos.y + getBaseSpeed()) == TERRAIN_TYPE::WATER &&
            _world->getTerrainDataAt(feetPos) != TERRAIN_TYPE::WATER) {
            _targetPos = feetPos;
        } else {
            if (_world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER) _targetPos == feetPos;
            ya += getBaseSpeed();
            _movingDir = DOWN;
        }
    } else if (feetPos.y > _targetPos.y) {
        if (_world->getTerrainDataAt(feetPos.x, feetPos.y - getBaseSpeed()) == TERRAIN_TYPE::WATER &&
            _world->getTerrainDataAt(feetPos) != TERRAIN_TYPE::WATER) {
            _targetPos = feetPos;
        } else {
            if (_world->getTerrainDataAt(feetPos) == TERRAIN_TYPE::WATER) _targetPos == feetPos;
            ya -= getBaseSpeed();
            _movingDir = UP;
        }
    }

    move(xa, ya);
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