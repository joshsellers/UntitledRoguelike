#include "Cactoid.h"
#include "../World.h"

Cactoid::Cactoid(sf::Vector2f pos) :
Entity(CACTOID, pos, 3.5, 2, 2, false) {
    setMaxHitPoints(35);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE + 4;
    _hitBoxYOffset = 3;
    _hitBox.width = TILE_SIZE * 2 - 7;
    _hitBox.height = TILE_SIZE * 2 - 3;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _isMob = true;
    _isEnemy = true; // had this as false because it's not immeditatly aggressive but then slimeball wouldn't shoot it
    _entityType = "cactoid";
    _isInitiallyDocile = true;

    srand(currentTimeNano());
    const int hasPennyChance = 10;
    unsigned int pennyAmount = randomInt(0, 510);
    if (pennyAmount >= hasPennyChance) getInventory().addItem(Item::PENNY.getId(), pennyAmount - hasPennyChance);
}

void Cactoid::update() {
    bool playerIsDead = _world->getPlayer()->getHitPoints() <= 0;

    if (getWorld()->getPlayer()->isActive() && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())) {
        getWorld()->getPlayer()->takeDamage(8);
        getWorld()->getPlayer()->knockBack(15.f, getMovingDir());
    }

    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 2);

    const float AGGRO_DIST = 90.f;
    if (!playerIsDead && !_isAggro && std::sqrt(std::pow(playerPos.x - cLoc.x, 2) + std::pow(playerPos.y - cLoc.y, 2)) < AGGRO_DIST) {
        _isAggro = true;
    } else if (playerIsDead && _isAggro) _isAggro = false;
    
    if (_isAggro) {
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
    }

    _sprite.setPosition(getPosition());

    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE;
        _hitBox.height = TILE_SIZE;
    } else {
        _hitBoxYOffset = 3;
        _hitBox.height = TILE_SIZE * 2 - 5;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _isHostile = _isAggro;
}

void Cactoid::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 2;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(feetPos);
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));

        int xOffset = ((_numSteps >> _animSpeed) & 1) * 16;

        _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, 16, 16));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - TILE_SIZE / 2, getPosition().y + (TILE_SIZE) + 8));
        surface.draw(_wavesSprite);
    }

    int xOffset = getMovingDir() == UP ? TILE_SIZE * 2 : 0;
    int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * 2 : 0;

    if (isMoving() && _isAggro) {
        _sprite.setTextureRect(sf::IntRect(
            28 * TILE_SIZE + xOffset, 13 * TILE_SIZE + yOffset, TILE_SIZE * 2, isSwimming() ? TILE_SIZE : TILE_SIZE * 2
        ));
        if (getMovingDir() == LEFT) _sprite.setScale(-1, 1);
        else _sprite.setScale(1, 1);
    } else {
        _sprite.setTextureRect(sf::IntRect(
            4 * TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE * 2, TILE_SIZE * 2
        ));
    }

    surface.draw(_sprite);
}

void Cactoid::damage(int damage) {
    _hitPoints -= damage;
    _isAggro = true;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}

void Cactoid::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(4 * TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE * 2, TILE_SIZE * 2));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE, 0); 
    
    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}

std::string Cactoid::getSaveData() const {
    return (_isAggro ? "1" : "0");
}
