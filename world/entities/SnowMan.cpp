#include "SnowMan.h"
#include "../World.h"

SnowMan::SnowMan(sf::Vector2f pos) : Entity(SNOWMAN, pos, 2, TILE_SIZE, TILE_SIZE * 2, false) {
    setMaxHitPoints(45);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE / 2;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE;
    _hitBox.height = TILE_SIZE * 2;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;
    _isMob = true;
    _isEnemy = true;

    _entityType = "snowman";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(0, 5);
    if (pennyAmount > 0) getInventory().addItem(Item::PENNY.getId(), pennyAmount);
}

void SnowMan::update() {
    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + 32);

    float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
    float desiredDist = 128.f;
    float distanceRatio = desiredDist / dist;

    sf::Vector2f goalPos((1.f - distanceRatio) * playerPos.x + distanceRatio * cLoc.x, (1.f - distanceRatio) * playerPos.y + distanceRatio * cLoc.y);

    float xa = 0.f, ya = 0.f;
    if (goalPos.y < cLoc.y) {
        ya--;
    } else if (goalPos.y > cLoc.y) {
        ya++;
    }

    if (goalPos.x < cLoc.x) {
        xa--;
    } else if (goalPos.x > cLoc.x) {
        xa++;
    }

    if (xa && ya) {
        xa *= 0.785398;
        ya *= 0.785398;
    }

    hoardMove(xa, ya, false);

    _sprite.setPosition(getPosition());

    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE;
        _hitBox.height = TILE_SIZE * 2 / 2;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 2;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    if (currentTimeMillis() - _lastFireTime >= SNOWMAN_FIRE_RATE) {
        fireTargetedProjectile(playerPos, Item::DATA_PROJECTILE_SNOW_BALL, "NONE", true);
        _lastFireTime = currentTimeMillis();
    }
}

void SnowMan::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 2;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2));

        int xOffset = ((_numSteps >> _animSpeed) & 1) * 16;

        _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, 16, 16));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - TILE_SIZE / 2, getPosition().y + (TILE_SIZE * 2) - 6));
        surface.draw(_wavesSprite);
    }

    int xOffset = getMovingDir() * TILE_SIZE;
    int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * 2 : 0;

    _sprite.setTextureRect(sf::IntRect(
        28 * TILE_SIZE + xOffset, 21 * TILE_SIZE, TILE_SIZE, isSwimming() ? TILE_SIZE * 2 / 2 : TILE_SIZE * 2
    ));

    surface.draw(_sprite);
}

void SnowMan::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(28 * TILE_SIZE, 21 * TILE_SIZE, TILE_SIZE, TILE_SIZE * 2));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE / 2.f, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + 32 / 2));
}

void SnowMan::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}
