#include "MushroomBoss.h"
#include "../World.h"

MushroomBoss::MushroomBoss(sf::Vector2f pos) : Boss(MUSHROOM_BOSS, pos, 1.f, 8 * TILE_SIZE, 9 * TILE_SIZE, 
    {
        BossState(SPAWN_SHROOMS, 5000, 5000)
    }
) {
    setMaxHitPoints(41500);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 2) - 8;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 4;
    _hitBox.height = TILE_SIZE * 9;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;

    _entityType = "shroomboss";
    _displayName = "Anita Amanita";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(8500, 10000);
    getInventory().addItem(Item::PENNY.getId(), pennyAmount);
}

void MushroomBoss::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 8;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + (float)TILE_SIZE * 4.5f));

        const int yOffset = ((_numSteps >> 3) & 1) * TILE_SIZE;

        _wavesSprite.setTextureRect(sf::IntRect(400, 688 + yOffset, TILE_SIZE * 5, TILE_SIZE));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - (float)TILE_SIZE * 8.f / 2.f + 24.f - 8.f, getPosition().y + (TILE_SIZE * 9)));
        surface.draw(_wavesSprite);
    }

    const int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 7) * TILE_SIZE * 9 : 0;
    const int xOffset = 0;

    _sprite.setTextureRect(sf::IntRect(
        (150 << SPRITE_SHEET_SHIFT) + xOffset, (40 << SPRITE_SHEET_SHIFT) + yOffset, TILE_SIZE * 8, isSwimming() ? (float)TILE_SIZE * 4.5f : TILE_SIZE * 9
    ));

    surface.draw(_sprite);
}

void MushroomBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(150 << SPRITE_SHEET_SHIFT, 40 << SPRITE_SHEET_SHIFT, TILE_SIZE * 8, TILE_SIZE * 9));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 9 / 2, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}

void MushroomBoss::subUpdate() {
    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 9);

    float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
    float desiredDist = 150.f;
    float distanceRatio = desiredDist / dist;

    float xa = 0.f, ya = 0.f;

    sf::Vector2f goalPos((1.f - distanceRatio) * playerPos.x + distanceRatio * cLoc.x, (1.f - distanceRatio) * playerPos.y + distanceRatio * cLoc.y);
    if (goalPos.y < cLoc.y) {
        ya--;
        _movingDir = LEFT;
    } else if (goalPos.y > cLoc.y) {
        ya++;
        _movingDir = DOWN;
    }

    if (goalPos.x < cLoc.x) {
        xa--;
    } else if (goalPos.x > cLoc.x) {
        xa++;
    }

    if (xa && ya) {
        xa *= 0.785398;
        ya *= 0.785398;

        if (ya > 0) _movingDir = DOWN;
        else _movingDir = LEFT;
    }

    if (dist < desiredDist) {
        xa = 0;
        ya = 0;

        if (_movingDir == LEFT) _movingDir = DOWN;
        else if (_movingDir == DOWN) _movingDir = LEFT;
    }

    if (_spawnedWithEnemies) hoardMove(xa, ya, true, 128);
    else move(xa, ya);


    _sprite.setPosition(getPosition());

    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE * 5;
        _hitBox.height = TILE_SIZE * 4;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 9;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    constexpr long long contactDamageRateMillis = 500LL;
    if (getWorld()->getPlayer()->isActive()
        && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())
        && currentTimeMillis() - _lastContactDamageTimeMillis >= contactDamageRateMillis) {
        getWorld()->getPlayer()->takeDamage(30);
        _lastContactDamageTimeMillis = currentTimeMillis();
    }
}

void MushroomBoss::onStateChange(const BossState previousState, const BossState newState) {
}

void MushroomBoss::runCurrentState() {
}
