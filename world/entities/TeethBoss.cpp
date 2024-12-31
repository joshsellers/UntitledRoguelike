#include "TeethBoss.h"
#include "../World.h"
#include "../../core/Viewport.h"

TeethBoss::TeethBoss(sf::Vector2f pos) : Boss(TEETH_BOSS, pos, 1.f, TILE_SIZE * 8, TILE_SIZE * 8,
    {
        BossState(CHARGE, 5000, 8000)
    }
) {
    setMaxHitPoints(30000);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 8) / 2;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 8;
    _hitBox.height = TILE_SIZE * 6;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;

    _entityType = "teethboss";
    _displayName = "Danny Smiles";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(6500, 7000);
    getInventory().addItem(Item::PENNY.getId(), pennyAmount);

    _animSpeed = 1;
}

void TeethBoss::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 8;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 4));

        const int yOffset = ((_numSteps >> 3) & 1) * TILE_SIZE;

        _wavesSprite.setTextureRect(sf::IntRect(400, 688 + yOffset, TILE_SIZE * 5, TILE_SIZE));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - (float)TILE_SIZE * 8.f / 2.f + 24.f, getPosition().y + (TILE_SIZE * 8)));
        surface.draw(_wavesSprite);
    }

    const int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 7) * TILE_SIZE * 8 : 0;
    const int xOffset = isMoving() || isSwimming() ? 0 : TILE_SIZE * 8;

    _sprite.setTextureRect(sf::IntRect(
        (134 << SPRITE_SHEET_SHIFT) + xOffset, (40 << SPRITE_SHEET_SHIFT) + yOffset, TILE_SIZE * 8, isSwimming() ? TILE_SIZE * 4 : TILE_SIZE * 8
    ));

    surface.draw(_sprite);
}

void TeethBoss::subUpdate() {
    if (_currentState.stateId != CHARGE) {
        sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
        sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 8);

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
    }

    _sprite.setPosition(getPosition());

    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE * 4;
        _hitBox.height = TILE_SIZE * 4;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 8;
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

void TeethBoss::onStateChange(const BossState previousState, const BossState newState) {
    if (newState.stateId == CHARGE) {
        resetChargeTarget();
    }
}

void TeethBoss::runCurrentState() {
    switch (_currentState.stateId) {
        case CHARGE:
        {
            const sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 4);

            const float dist = std::sqrt(std::pow(_chargeTarget.x - cLoc.x, 2) + std::pow(_chargeTarget.y - cLoc.y, 2));

            constexpr float speed = 8.f;
            const float targetDist = speed;
            const float distRat = targetDist / dist;

            //_pos = sf::Vector2f(((1 - distRat) * cLoc.x + distRat * _chargeTarget.x), ((1 - distRat) * cLoc.y + distRat * _chargeTarget.y));
            const float dx = _chargeTarget.x - cLoc.x;
            const float dy = _chargeTarget.y - cLoc.y;
            const float angle = std::atan2(dy, dx);
            const float xa = speed * std::cos(angle);
            const float ya = speed * std::sin(angle);

            move(xa, ya);

            if (distRat > 1.f) resetChargeTarget();

            break;
        }
    }
}

void TeethBoss::resetChargeTarget() {
    const sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);
    const sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 4);

    const float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
    const float targetDist = dist + 350.f;
    const float distRat = targetDist / dist;

    _chargeTarget = sf::Vector2f(((1 - distRat) * cLoc.x + distRat * playerPos.x), ((1 - distRat) * cLoc.y + distRat * playerPos.y));
}

void TeethBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(142 << SPRITE_SHEET_SHIFT, 40 << SPRITE_SHEET_SHIFT, TILE_SIZE * 8, TILE_SIZE * 8));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 8 / 2, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));

}