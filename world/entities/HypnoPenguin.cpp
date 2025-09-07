#include "HypnoPenguin.h"
#include "../World.h"

HypnoPenguin::HypnoPenguin(sf::Vector2f pos) :
    Entity(HYPNO_PENGUIN, pos, 3.f, 1 * TILE_SIZE, 1 * TILE_SIZE, false) {
    setMaxHitPoints(200);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE / 2 + 3;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE - 6;
    _hitBox.height = TILE_SIZE;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;
    _isMob = true;
    _isEnemy = true;

    _entityType = "hypnopenguin";
}

void HypnoPenguin::update() {
    const sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);
    const sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + (float)TILE_SIZE / 2.f);

    float xa = 0.f, ya = 0.f;
    
    if (playerPos.y < cLoc.y) {
        ya--;
        _movingDir = UP;
    } else if (playerPos.y > cLoc.y) {
        ya++;
        _movingDir = DOWN;
    }

    if (playerPos.x < cLoc.x) {
        xa--;
        _movingDir = LEFT;
    } else if (playerPos.x > cLoc.x) {
        xa++;
        _movingDir = RIGHT;
    }

    if (xa && ya) {
        xa *= 0.785398;
        ya *= 0.785398;
    }

    //if (playerPos.x < cLoc.x) _movingDir = LEFT;
    //else if (playerPos.x > cLoc.x) _movingDir = RIGHT;

    hoardMove(xa, ya, true, 16.f);

    _sprite.setPosition(getPosition());
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    if (getHitBox().intersects(getWorld()->getPlayer()->getHitBox())) {
        getWorld()->getPlayer()->takeDamage(15);
        getWorld()->getPlayer()->knockBack(5.f, (MOVING_DIRECTION)_movingDir);
    }
}

void HypnoPenguin::draw(sf::RenderTexture& surface) {
    int xOffset = getMovingDir() == UP ? TILE_SIZE : 0;
    int yOffset = isMoving() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE : 0;

    _sprite.setTextureRect(sf::IntRect(
        167 * TILE_SIZE + xOffset, 20 * TILE_SIZE + yOffset, TILE_SIZE, TILE_SIZE
    ));
    if (getMovingDir() == LEFT) _sprite.setScale(-1, 1);
    else _sprite.setScale(1, 1);

    surface.draw(_sprite);
}

void HypnoPenguin::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}

void HypnoPenguin::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(167 * TILE_SIZE, 20 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE / 2, 0);
}
