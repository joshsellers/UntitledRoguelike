#include "Mushroid.h"

#include "../World.h"

Mushroid::Mushroid(sf::Vector2f pos) :
    Entity(MUSHROID, pos, 4, 2, 2, false) {
    setMaxHitPoints(35);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE + 12;
    _hitBoxYOffset = 23;
    _hitBox.width = 10;
    _hitBox.height = 10;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _isMob = true;
    _isEnemy = true;
    _entityType = "mushroid";
    _isInitiallyDocile = true;

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(0, 5);
    if (pennyAmount > 0) getInventory().addItem(Item::PENNY.getId(), pennyAmount);
}

void Mushroid::update() {
    bool playerIsDead = _world->getPlayer()->getHitPoints() <= 0;

    if (getWorld()->getPlayer()->isActive() && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())) {
        getWorld()->getPlayer()->takeDamage(8);
        getWorld()->getPlayer()->knockBack(15.f, getMovingDir());
    }

    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 2);

    constexpr float AGGRO_DIST = 80.f;
    if (!playerIsDead && !_isAggro && !_isTransforming && std::sqrt(std::pow(playerPos.x - cLoc.x, 2) + std::pow(playerPos.y - cLoc.y, 2)) < AGGRO_DIST) {
        _isTransforming = true;
    } else if (playerIsDead && (_isAggro || _isTransforming)) {
        _isAggro = false;
        _isTransforming = false;
    }

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
    } else if (_isAggro) {
        _hitBoxXOffset = -TILE_SIZE + 6;
        _hitBoxYOffset = 6;
        _hitBox.width = 23;
        _hitBox.height = 27;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _isHostile = _isAggro;

    if (_isTransforming) _animCounter++;
}

void Mushroid::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 2;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(feetPos);
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));

        int xOffset = ((_numSteps >> _animSpeed) & 1) * 16;

        _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, 16, 16));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - TILE_SIZE / 2, getPosition().y + (TILE_SIZE)+8));
        surface.draw(_wavesSprite);
    }

    int xOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * 2 : 0;

    if (isMoving() && _isAggro) {
        _sprite.setTextureRect(sf::IntRect(
            36 * TILE_SIZE + xOffset, 37 * TILE_SIZE, TILE_SIZE * 2, isSwimming() ? TILE_SIZE : TILE_SIZE * 2
        ));
    } else if (_isTransforming) {
        constexpr int ticksPerFrame = 4;
        constexpr int frameCount = 6;
        const int frame = ((_animCounter / ticksPerFrame) % frameCount);

        _sprite.setTextureRect(sf::IntRect(
            158 * TILE_SIZE, (43 + frame * 2) * TILE_SIZE, TILE_SIZE * 2, TILE_SIZE * 2
        ));

        if (frame == frameCount - 1) {
            _isTransforming = false;
            _isAggro = true;
        }
    } else {
        _sprite.setTextureRect(sf::IntRect(
            158 * TILE_SIZE, 39 * TILE_SIZE, TILE_SIZE * 2, TILE_SIZE * 2
        ));
    }

    surface.draw(_sprite);
}

void Mushroid::damage(int damage) {
    _hitPoints -= damage;
    _isTransforming = true;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}

void Mushroid::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(158 * TILE_SIZE, 43 * TILE_SIZE, TILE_SIZE * 2, TILE_SIZE * 2));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}

std::string Mushroid::getSaveData() const {
    return (_isAggro || _isTransforming ? "1" : "0");
}
