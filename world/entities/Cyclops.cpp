#include "Cyclops.h"
#include "../../core/Util.h"
#include "../World.h"

Cyclops::Cyclops(sf::Vector2f pos) : Entity(CYCLOPS, pos, 1, TILE_SIZE * 3, TILE_SIZE * 4, false) {
    setMaxHitPoints(215);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 3) / 2;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 3;
    _hitBox.height = TILE_SIZE * 4;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;
    _isMob = true;
    _isEnemy = true;

    _entityType = "cyclops";

    srand(currentTimeNano());
    constexpr int hasPennyChance = 30;
    unsigned int pennyAmount = randomInt(0, 930);
    if (pennyAmount >= hasPennyChance) getInventory().addItem(Item::PENNY.getId(), pennyAmount - hasPennyChance);

    constexpr int hasEyeChance = 1000;
    if (randomInt(0, hasEyeChance) == 0) getInventory().addItem(Item::CYCLOPS_EYE.getId(), 1);
}

void Cyclops::update() {
    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + 64);

    float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
    float distanceRatio = _desiredDist / dist;

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

    hoardMove(xa, ya, false, 64.f);

    _sprite.setPosition(getPosition());

    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE * 2;
        _hitBox.height = TILE_SIZE * 4 / 2;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 4;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    if (getWorld()->getPlayer()->isActive() && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())) {
        getWorld()->getPlayer()->takeDamage(15);
        getWorld()->getPlayer()->knockBack(4, getMovingDir());
    }

    _desiredDist = std::max(1.f, _desiredDist - 0.05f);
}

void Cyclops::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 4;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 4 / 2));

        int xOffset = ((_numSteps >> _animSpeed) & 1) * 16;

        _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, 16, 16));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - 6, getPosition().y + 64 - 3));
        surface.draw(_wavesSprite);
    }

    int xOffset = getMovingDir() == UP ? TILE_SIZE * 3: 0;
    int yOffset = isMoving() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * 4 : 0;

    _sprite.setTextureRect(sf::IntRect(
        30 * TILE_SIZE + xOffset, 30 * TILE_SIZE + yOffset, TILE_SIZE * 3, TILE_SIZE * (isSwimming() ? 2 : 4)
    ));
    if (getMovingDir() == LEFT) _sprite.setScale(-1, 1);
    else _sprite.setScale(1, 1);

    surface.draw(_sprite);
}

void Cyclops::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(30 * TILE_SIZE, 30 * TILE_SIZE, TILE_SIZE * 3, TILE_SIZE * 4));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((TILE_SIZE * 3) / 2, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x - 8, getPosition().y + 64));
}

void Cyclops::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}
