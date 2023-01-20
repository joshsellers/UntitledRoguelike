#include "Penguin.h"
#include <boost/random/uniform_int_distribution.hpp>
#include "World.h"

Penguin::Penguin(sf::Vector2f pos) :
Entity(pos, 0.5, 1, 1, false) {
    _targetPos = _pos;
}

void Penguin::update() {
    float xa = 0, ya = 0;
    boost::random::uniform_int_distribution<> shouldMove(0, 100);
    if (_targetPos == getPosition() && shouldMove(_gen) == 0) {
        const int maxDist = 100;
        boost::random::uniform_int_distribution<> xDist(-maxDist, maxDist);
        boost::random::uniform_int_distribution<> yDist(-maxDist, maxDist);
        _targetPos.x = getPosition().x + xDist(_gen);
        _targetPos.y = getPosition().y + yDist(_gen);
    } else if (getPosition().x < _targetPos.x) {
        xa += getBaseSpeed();
        _movingDir = RIGHT;
    } else if (getPosition().x > _targetPos.x) {
        xa -= getBaseSpeed();
        _movingDir = LEFT;
    } else if (getPosition().y < _targetPos.y) {
        ya += getBaseSpeed();
        _movingDir = DOWN;
    } else if (getPosition().y > _targetPos.y) {
        ya -= getBaseSpeed();
        _movingDir = UP;
    }
    // Add getChuckAt(pos) that returns null if no chunk
    //if (_world->findCurrentChunk)

    move(xa, ya);
    _sprite.setPosition(getPosition());
}

void Penguin::draw(sf::RenderTexture& surface) {
    int xOffset = getMovingDir() == UP ? TILE_SIZE : 0;
    int yOffset = isMoving() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE : 0;
    sf::IntRect oldRect = _sprite.getTextureRect();
    _sprite.setTextureRect(sf::IntRect(
        8 * TILE_SIZE + xOffset, 4 * TILE_SIZE + yOffset, oldRect.width, oldRect.height
    ));
    if (getMovingDir() == LEFT) _sprite.setScale(-1, 1);
    else _sprite.setScale(1, 1);

    surface.draw(_sprite);
}

void Penguin::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(8 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE / 2, 0);
}
