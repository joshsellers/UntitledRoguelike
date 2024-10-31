#include "BurgerBeast.h"
#include "../World.h"

BurgerBeast::BurgerBeast(sf::Vector2f pos) : Entity(BURGER_BEAST, pos, 3, TILE_SIZE * 2, TILE_SIZE * 2, false) {
    setMaxHitPoints(200);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE + 4;
    _hitBoxYOffset = 7;
    _hitBox.width = 24;
    _hitBox.height = 22;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;
    _isMob = true;
    _isEnemy = true;

    _entityType = "burgerbeast";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(0, 2);
    if (pennyAmount > 0) getInventory().addItem(Item::PENNY.getId(), pennyAmount); 
}

void BurgerBeast::update() {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 2;

    if (getWorld()->getPlayer()->isActive() && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())) {
        getWorld()->getPlayer()->takeDamage(30);
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
        _hitBoxYOffset = 7 + TILE_SIZE;
        _hitBox.height = 22 - TILE_SIZE;
    } else {
        _hitBoxYOffset = 7;
        _hitBox.height = 22;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void BurgerBeast::draw(sf::RenderTexture& surface) {
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

    int xOffset = getMovingDir() == UP ? TILE_SIZE * 2 : 0;
    int yOffset = isMoving() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * 2: 0;

    _sprite.setTextureRect(sf::IntRect(
        77 * TILE_SIZE + xOffset, 76 * TILE_SIZE + yOffset, TILE_SIZE * 2, TILE_SIZE * (isSwimming() ? 1 : 2)
    ));
    if (getMovingDir() == LEFT) _sprite.setScale(-1, 1);
    else _sprite.setScale(1, 1);

    surface.draw(_sprite);
}

void BurgerBeast::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}

void BurgerBeast::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(77 * TILE_SIZE, 76 * TILE_SIZE, TILE_SIZE * 2, TILE_SIZE * 2));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, TILE_SIZE, TILE_SIZE));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}