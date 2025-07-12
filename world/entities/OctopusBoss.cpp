#include "OctopusBoss.h"
#include "../World.h"
#include "projectiles/Projectile.h"
#include "LaserBeam.h"

OctopusBoss::OctopusBoss(sf::Vector2f pos) : Boss(OCTOPUS_BOSS, pos, 1.0, 12 * TILE_SIZE, 13 * TILE_SIZE, 
    {
        BossState(BEHAVIOR_STATE::REST, 1000, 2000),
        BossState(BEHAVIOR_STATE::INK_BUBBLE, 4000, 6000),
        BossState(BEHAVIOR_STATE::INK_RAY, 4000, 5000)
    }) {
    setMaxHitPoints(80000);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 8) / 2;
    _hitBoxYOffset = TILE_SIZE * 2;
    _hitBox.width = TILE_SIZE * 8;
    _hitBox.height = TILE_SIZE * 10;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;

    _entityType = "octopusboss";
    _displayName = "Amy Eightlegs";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(15000, 17000);
    getInventory().addItem(Item::PENNY.getId(), pennyAmount);
}

void OctopusBoss::draw(sf::RenderTexture& surface) {
    const int yOffset = ((_animCounter >> 1) & 7) * TILE_SIZE * 13;

    _sprite.setTextureRect(sf::IntRect(
        1856, 992 + yOffset, TILE_SIZE * 12, TILE_SIZE * 13
    ));

    surface.draw(_sprite);
}

void OctopusBoss::subUpdate() {
    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 6);

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

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    constexpr long long contactDamageRateMillis = 500LL;
    if (getWorld()->getPlayer()->isActive()
        && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())
        && currentTimeMillis() - _lastContactDamageTimeMillis >= contactDamageRateMillis) {
        getWorld()->getPlayer()->takeDamage(25);
        _lastContactDamageTimeMillis = currentTimeMillis();
    }

    _animCounter++;
}

void OctopusBoss::onStateChange(const BossState previousState, const BossState newState) {
    if (newState.stateId == INK_RAY) {
        const float angleChoices[2] = { 0.f, 22.5f };
        _angleOffset = angleChoices[randomInt(0, 1)];
    }
}

void OctopusBoss::runCurrentState() {
    switch (_currentState.stateId) {
        case INK_BUBBLE:
        {
            const long long fireRateMillis = 750LL;
            if (currentTimeMillis() - _lastFireTimeMillis >= fireRateMillis) {
                //const float angle = degToRads(randomInt(0, 360));
                const sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);
                const auto proj = fireTargetedProjectile(playerPos, ProjectileDataManager::getData("_PROJECTILE_INK_BUBBLE"), "NONE", true, false, {-48.f / 2.f, 48.f}, true);
                proj->setSplitInto("_PROJECTILE_INK_BUBBLE-0", false, true, 4, 4);
                _lastFireTimeMillis = currentTimeMillis();
            }
            break;
        }
        case INK_RAY:
        {
            const long long fireRateMillis = 1250LL;
            if (currentTimeMillis() - _lastFireTimeMillis >= fireRateMillis) {
                const int numLasers = 8;
                for (int i = 0; i < numLasers; i++) {
                    const auto& laser = std::shared_ptr<LaserBeam>(new LaserBeam(this, (360.f / numLasers) * i + _angleOffset, 0xFFFFFFFF, 32, 1000, 10, {0, 156}, true, fireRateMillis));
                    laser->setWorld(getWorld());
                    laser->setTextureRect({2064, 1792, TILE_SIZE * 4, TILE_SIZE * 4}, 3, 12, true);
                    getWorld()->addEntity(laser);
                }
                _lastFireTimeMillis = currentTimeMillis();
                _angleOffset += 22.5f;
            }
            break;
        }
    }
}

void OctopusBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(1856, 992, TILE_SIZE * 12, TILE_SIZE * 13));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 12 / 2, 0);
}