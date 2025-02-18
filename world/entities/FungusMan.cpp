#include "FungusMan.h"
#include "../World.h"

FungusMan::FungusMan(sf::Vector2f pos) : Entity(FUNGUS_MAN, pos, 2, TILE_SIZE * 2, TILE_SIZE * 3, false) {
    setMaxHitPoints(105);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE;
    _hitBoxYOffset = 13;
    _hitBox.width = TILE_SIZE * 2;
    _hitBox.height = 36;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;
    _isMob = true;
    _isEnemy = true;

    _entityType = "fungusman";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(0, 5);
    if (pennyAmount > 0) getInventory().addItem(Item::PENNY.getId(), pennyAmount);
}

void FungusMan::update() {
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
        _hitBoxYOffset = TILE_SIZE * 2;
        _hitBox.height = (float)TILE_SIZE * 3 / 2;
    } else {
        _hitBoxYOffset = 13;
        _hitBox.height = 36;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;


    constexpr long long fireRate = 750LL;
    if (currentTimeMillis() - _lastFireTime >= fireRate) {
        fireTargetedProjectile(playerPos, ProjectileDataManager::getData("_PROJECTILE_SPORE_SHOT"), "NONE", true);
        _lastFireTime = currentTimeMillis();
    }
}

void FungusMan::draw(sf::RenderTexture& surface) {
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
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - (float)TILE_SIZE / 2, getPosition().y + ((float)TILE_SIZE * 3)));
        surface.draw(_wavesSprite);
    }

    int xOffset = /*getMovingDir() == UP ? TILE_SIZE * 2 : */0;
    int yOffset = isMoving() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * 3 : 0;

    _sprite.setTextureRect(sf::IntRect(
        85 * TILE_SIZE + xOffset, 76 * TILE_SIZE + yOffset, TILE_SIZE * 2, TILE_SIZE * (isSwimming() ? 2 : 3)
    ));

    surface.draw(_sprite);
}

void FungusMan::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(85 * TILE_SIZE, 76 * TILE_SIZE, TILE_SIZE * 2, TILE_SIZE * 3));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 2.f / 2.f, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 3.f / 2.f));
}

void FungusMan::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}
