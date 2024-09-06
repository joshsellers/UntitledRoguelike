#include "TulipMonster.h"
#include "../../core/Util.h"
#include "../World.h"

TulipMonster::TulipMonster(sf::Vector2f pos) : Entity(TULIP_MONSTER, pos, 2, TILE_SIZE * 3, TILE_SIZE * 3, false) {
    setMaxHitPoints(105);
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

    _entityType = "tulipmonster";

    srand(currentTimeNano());
    const int hasPennyChance = 5;
    unsigned int pennyAmount = randomInt(0, 505);
    if (pennyAmount >= hasPennyChance) getInventory().addItem(Item::PENNY.getId(), pennyAmount - hasPennyChance);
}

void TulipMonster::update() {
    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + (float)TILE_SIZE * 3.f / 2.f);

    float dist = std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2);
    float desiredDist = 128.f * 128.f;
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

    hoardMove(xa, ya, true);

    _sprite.setPosition(getPosition());

    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE;
        _hitBox.height = (float)TILE_SIZE * 3 / 2;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 3;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    if (currentTimeMillis() - _lastFireTime >= TULIP_MONSTER_FIRE_RATE) {
        fireTargetedProjectile(playerPos, Item::DATA_PROJECTILE_POLLEN, "NONE", true);
        _lastFireTime = currentTimeMillis();
    }
}

void TulipMonster::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 3;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 3.f / 2.f));

        int xOffset = ((_numSteps >> _animSpeed) & 1) * 16;

        _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, 16, 16));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - (float)TILE_SIZE / 3, getPosition().y + ((float)TILE_SIZE * 3) - 6));
        surface.draw(_wavesSprite);
    }

    int xOffset = getMovingDir() * TILE_SIZE * 3;
    int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * 3 : 0;

    _sprite.setTextureRect(sf::IntRect(
        1232 + xOffset, 640 + yOffset, TILE_SIZE * 3, isSwimming() ? (float)TILE_SIZE * 3.f / 2.f : TILE_SIZE * 3
    ));

    surface.draw(_sprite);
}

void TulipMonster::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(1232, 640, TILE_SIZE * 3, TILE_SIZE * 3));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 3.f / 2.f, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 3.f / 2.f));
}

void TulipMonster::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}
