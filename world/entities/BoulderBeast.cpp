#include "BoulderBeast.h"
#include "../../core/Util.h"
#include "../World.h"

BoulderBeast::BoulderBeast(sf::Vector2f pos) : Boss(BOULDER_BEAST, pos, 2.5, TILE_SIZE * 3, TILE_SIZE * 3,
    {
        BossState(BEHAVIOR_STATE::CHASE, 4000LL, 8000LL),
        BossState(BEHAVIOR_STATE::TARGETED_FIRE, 5000LL, 12000LL)
    }) {
    setMaxHitPoints(110);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 2) / 2;
    _hitBoxYOffset = TILE_SIZE / 2;
    _hitBox.width = TILE_SIZE * 2;
    _hitBox.height = TILE_SIZE * 2;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;

    _entityType = "boulderbeast";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(0, 5);
    if (pennyAmount > 0) getInventory().addItem(Item::PENNY.getId(), pennyAmount);

    deactivateBossMode();
}

void BoulderBeast::draw(sf::RenderTexture& surface) {
    int xOffset = 0;
    int yOffset = ((_numSteps >> _animSpeed) & 7) * TILE_SIZE * 3;

    _sprite.setTextureRect(sf::IntRect(
        1296 + xOffset, 832 + yOffset, TILE_SIZE * 3, TILE_SIZE * 3
    ));

    surface.draw(_sprite);
}

void BoulderBeast::subUpdate() {
    _numSteps++;

    constexpr long long contactDamageRateMillis = 300LL;
    if (currentTimeMillis() - _lastContactDamageTimeMillis >= contactDamageRateMillis 
        && getWorld()->getPlayer()->isActive()
        && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())) {
        getWorld()->getPlayer()->takeDamage(12);
        _lastContactDamageTimeMillis = currentTimeMillis();
    }
}

void BoulderBeast::onStateChange(const BossState previousState, const BossState newState) {
    switch (newState.stateId) {
        case BEHAVIOR_STATE::CHASE:
            _baseSpeed = 4.f;
            break;
        case BEHAVIOR_STATE::TARGETED_FIRE:
            _baseSpeed = 1.f;
            break;
    }
}

void BoulderBeast::runCurrentState() {
    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 3);
    sf::Vector2f goalPos(cLoc);

    switch (_currentState.stateId) {
        case CHASE:
        {
            goalPos = playerPos;
            break;
        }
        case TARGETED_FIRE:
        {
            float dist = std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2);
            float desiredDist = 128.f * 128.f;
            float distanceRatio = desiredDist / dist;

            goalPos = sf::Vector2f((1.f - distanceRatio) * playerPos.x + distanceRatio * cLoc.x, (1.f - distanceRatio) * playerPos.y + distanceRatio * cLoc.y);

            if (currentTimeMillis() - _lastFireTimeMillis >= _fireRateMillis) {
                if (!_fireDiagonal) {
                    fireTargetedProjectile(0.f * ((float)PI / 180.f), Item::DATA_PROJECTILE_ROCK, "NONE", true, false, { 0, 0 }, false);
                    fireTargetedProjectile(90.f * ((float)PI / 180.f), Item::DATA_PROJECTILE_ROCK, "NONE", true, false, { 0, 0 }, false);
                    fireTargetedProjectile(180.f * ((float)PI / 180.f), Item::DATA_PROJECTILE_ROCK, "NONE", true, false, { 0, 0 }, false);
                    fireTargetedProjectile(270.f * ((float)PI / 180.f), Item::DATA_PROJECTILE_ROCK, "NONE", true, false, { 0, 0 }, false);
                    _fireDiagonal = true;
                } else {
                    fireTargetedProjectile(45.f * ((float)PI / 180.f), Item::DATA_PROJECTILE_ROCK, "NONE", true, false, { 0, 0 }, false);
                    fireTargetedProjectile(135.f * ((float)PI / 180.f), Item::DATA_PROJECTILE_ROCK, "NONE", true, false, { 0, 0 }, false);
                    fireTargetedProjectile(225.f * ((float)PI / 180.f), Item::DATA_PROJECTILE_ROCK, "NONE", true, false, { 0, 0 }, false);
                    fireTargetedProjectile(315.f * ((float)PI / 180.f), Item::DATA_PROJECTILE_ROCK, "NONE", true, false, { 0, 0 }, false);
                    _fireDiagonal = false;
                }
                _lastFireTimeMillis = currentTimeMillis();
            }
            break;
        }
    }

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

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void BoulderBeast::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(1296, 832, TILE_SIZE * 3, TILE_SIZE * 3));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 3.f / 2.f, 0);
}
