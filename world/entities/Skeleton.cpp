#include "Skeleton.h"
#include "../World.h"

Skeleton::Skeleton(sf::Vector2f pos) : Entity(SKELETON, pos, 3, TILE_SIZE, TILE_SIZE * 2, false) {
    setMaxHitPoints(35);
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

    _entityType = "skeleton";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(0, 5);
    if (pennyAmount > 0) getInventory().addItem(Item::PENNY.getId(), pennyAmount);

    constexpr int hasBoneChance = 20;
    bool hasBone = randomInt(0, hasBoneChance) == 0;
    if (hasBone) getInventory().addItem(Item::BONE.getId(), 1);
}

void Skeleton::update() {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 2;

    if (getWorld()->getPlayer()->isActive() && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())) {
        getWorld()->getPlayer()->takeDamage(6);
        getWorld()->getPlayer()->knockBack(18.f, getMovingDir());
    }

    sf::Vector2f goalPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 2);

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

    hoardMove(xa, ya, true);

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
}

void Skeleton::draw(sf::RenderTexture& surface) {
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
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - TILE_SIZE / 2, getPosition().y + (TILE_SIZE * 2) / 2 + 7));
        surface.draw(_wavesSprite);
    }

    int xOffset = getMovingDir() * TILE_SIZE;
    int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * 2 : 0;

    _sprite.setTextureRect(sf::IntRect(
        32 * TILE_SIZE + xOffset, 13 * TILE_SIZE + yOffset, TILE_SIZE, isSwimming() ? TILE_SIZE * 2 / 2 : TILE_SIZE * 2
    ));

    surface.draw(_sprite);
}

void Skeleton::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(32 * TILE_SIZE, 13 * TILE_SIZE, TILE_SIZE, TILE_SIZE * 2));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE / 2, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}

void Skeleton::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}
