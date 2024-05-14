#include "PlantMan.h"
#include <boost/random/uniform_int_distribution.hpp>
#include "World.h"
#include "Util.h"

PlantMan::PlantMan(sf::Vector2f pos) :
    Entity(PLANTMAN, pos, 3, TILE_SIZE, TILE_SIZE * 3, false) {
    _gen.seed(currentTimeNano());

    setMaxHitPoints(40);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE / 2;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE;
    _hitBox.height = TILE_SIZE * 3;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;
    _isMob = true;
    _isEnemy = true;

    _entityType = "plantman";

    srand(currentTimeNano());
    const int hasPennyChance = 10;
    unsigned int pennyAmount = randomInt(0, 510);
    if (pennyAmount >= hasPennyChance) getInventory().addItem(Item::PENNY.getId(), pennyAmount - hasPennyChance);
}

void PlantMan::update() {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 3;

    for (auto& entity : getWorld()->getEntities()) {
        if (!entity->isProp() && entity->isActive() && !entity->isMob() && entity->isDamageable() && !entity->compare(this) 
            && entity->getHitBox().intersects(getHitBox()) && entity->getEntityType() == "player") {
            entity->takeDamage(5);
            entity->knockBack(20.f, getMovingDir());
            break;
        }
    }

    sf::Vector2f goalPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + 48);

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
        _hitBox.height = TILE_SIZE * 3 / 2;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 3;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void PlantMan::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 3;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2));

        int xOffset = ((_numSteps >> _animSpeed) & 1) * 16;

        _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, 16, 16));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - TILE_SIZE / 2, getPosition().y + (TILE_SIZE * 3) / 2 + 9));
        surface.draw(_wavesSprite);
    }

    int xOffset = getMovingDir() * TILE_SIZE;
    int yOffset = isMoving() || isSwimming()  ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * 3 : 0;

    _sprite.setTextureRect(sf::IntRect(
        24 * TILE_SIZE + xOffset, 13 * TILE_SIZE + yOffset, TILE_SIZE, isSwimming() ? TILE_SIZE * 3 / 2 : TILE_SIZE * 3
    ));

    surface.draw(_sprite);
}

void PlantMan::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}

void PlantMan::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(24 * TILE_SIZE, 13 * TILE_SIZE, TILE_SIZE, TILE_SIZE * 3));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE / 2, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + 48 / 2));
}
