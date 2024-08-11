#include "FleshChicken.h"
#include "../../core/Util.h"
#include "../World.h"

FleshChicken::FleshChicken(sf::Vector2f pos) : Entity(FLESH_CHICKEN, pos, 3.5f, TILE_SIZE * 2, TILE_SIZE * 2, false) {
    setMaxHitPoints(30);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -14;
    _hitBoxYOffset = 10;
    _hitBox.width = 28;
    _hitBox.height = TILE_SIZE * 2 - 10;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;
    _isMob = true;
    _isEnemy = true;

    _entityType = "fleshchicken";

    srand(currentTimeNano());
    const int hasPennyChance = 10;
    unsigned int pennyAmount = randomInt(0, 510);
    if (pennyAmount >= hasPennyChance) getInventory().addItem(Item::PENNY.getId(), pennyAmount - hasPennyChance);
}

void FleshChicken::update() {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 2;

    if (getWorld()->getPlayer()->isActive() && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())) {
        getWorld()->getPlayer()->takeDamage(5);
        getWorld()->getPlayer()->knockBack(20.f, getMovingDir());
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
        _hitBox.height = TILE_SIZE;
    } else {
        _hitBoxYOffset = 10;
        _hitBox.height = TILE_SIZE * 2 - 10;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void FleshChicken::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 2;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2));

        int xOffset = ((_numSteps >> _animSpeed) & 1) * TILE_SIZE;

        _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, TILE_SIZE, TILE_SIZE));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - TILE_SIZE / 2, getPosition().y + (TILE_SIZE)+9));
        surface.draw(_wavesSprite);
    }

    int xOffset = getMovingDir() * TILE_SIZE * 2;
    int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 1) * TILE_SIZE * 2 : 0;

    _sprite.setTextureRect(sf::IntRect(
        36 * TILE_SIZE + xOffset, 17 * TILE_SIZE + yOffset, TILE_SIZE * 2, isSwimming() ? TILE_SIZE : TILE_SIZE * 2
    ));

    surface.draw(_sprite);
}

void FleshChicken::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(36 * TILE_SIZE, 17 * TILE_SIZE, TILE_SIZE * 2, TILE_SIZE * 2));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, TILE_SIZE, TILE_SIZE));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}

void FleshChicken::damage(int damage) {
}
