#include "Shark.h"
#include "../World.h"

Shark::Shark(sf::Vector2f pos) : Entity(SHARK, pos, 1.f, TILE_SIZE * 2, TILE_SIZE * 2, false) {
    _gen.seed(currentTimeNano());

    setMaxHitPoints(35);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 2;
    _hitBox.height = TILE_SIZE * 2;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _isMob = true;
    _isEnemy = true;
    _entityType = "shark";
    _isInitiallyDocile = true;

    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE;
    _wanderTargetPos = feetPos;
}

void Shark::update() {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE;

    if (!getWorld()->getPlayer()->isInBoat() && getWorld()->getPlayer()->isActive() && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())) {
        getWorld()->getPlayer()->takeDamage(8);
        getWorld()->getPlayer()->knockBack(15.f, getMovingDir());
    }

    bool playerIsInWater = getWorld()->getPlayer()->isSwimming() && !getWorld()->getPlayer()->isInBoat();
    _isHostile = playerIsInWater;

    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 2);
    const float AGGRO_DIST = 300.f;

    if (playerIsInWater && std::sqrt(std::pow(playerPos.x - cLoc.x, 2) + std::pow(playerPos.y - cLoc.y, 2)) < AGGRO_DIST) {
        int x = (int)cLoc.x;
        int y = (int)cLoc.y;
        int px = (int)playerPos.x;
        int py = (int)playerPos.y;
        bool canGetToPlayerThroughWater = true;
        while (x != px || y != py) {
            if (x < px) x++;
            else if (x > px) x--;
            if (y < py) y++;
            else if (y > py) y--;

            if (getWorld()->getTerrainDataAt(sf::Vector2f(x, y)) != TERRAIN_TYPE::WATER) {
                canGetToPlayerThroughWater = false;
                feetPos = getPosition();
                feetPos.y += TILE_SIZE;
                _wanderTargetPos = feetPos;
                break;
            }
        }

        if (canGetToPlayerThroughWater) {
            float xa = 0.f, ya = 0.f;
            if (playerPos.y < cLoc.y) {
                ya--;
            } else if (playerPos.y > cLoc.y) {
                ya++;
            }

            if (playerPos.x < cLoc.x) {
                xa--;
            } else if (playerPos.x > cLoc.x) {
                xa++;
            }

            if (xa && ya) {
                xa *= 0.785398;
                ya *= 0.785398;
            }

            hoardMove(xa, ya, true); 
            feetPos = getPosition();
            feetPos.y += TILE_SIZE;
            _wanderTargetPos = feetPos;
        } else {
            swimWander(feetPos, _gen);
        }
    } else {
        swimWander(feetPos, _gen);
    }

    _sprite.setPosition(getPosition()); 
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void Shark::draw(sf::RenderTexture& surface) {
    int xOffset = getMovingDir() * TILE_SIZE * 2;
    int yOffset = ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * 2;

    _sprite.setTextureRect(sf::IntRect(
        36 * TILE_SIZE + xOffset, 29 * TILE_SIZE + yOffset, TILE_SIZE * 2, TILE_SIZE * 2
    ));

    surface.draw(_sprite);
}

void Shark::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(36 * TILE_SIZE, 29 * TILE_SIZE, TILE_SIZE * 2, TILE_SIZE * 2));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE, 0);

    _sprite.setColor(sf::Color(0xFFFFFF88));
}

void Shark::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}
