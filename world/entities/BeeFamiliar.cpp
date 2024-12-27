#include "BeeFamiliar.h"
#include "../World.h"
#include "orbiters/Orbiter.h"

BeeFamiliar::BeeFamiliar(sf::Vector2f pos, float orbiterAngle, float orbiterDistance) : Entity(BEE_FAMILIAR, pos, 1.f, TILE_SIZE, TILE_SIZE, false),
_orbiterAngle(orbiterAngle), _orbiterDistance(orbiterDistance)
{
    setMaxHitPoints(1000000000);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -4;
    _hitBoxYOffset = 6;
    _hitBox.width = 10;
    _hitBox.height = 7;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _entityType = "beefamiliar";
}

void BeeFamiliar::update() {
    if (_enemyTarget == nullptr || !_enemyTarget->isActive()) {
        _hasSelectedEnemy = false;
        _enemyTarget = nullptr;
    }

    sf::Vector2f targetPos;
    const sf::Vector2f cLoc(getPosition().x, getPosition().y + (float)TILE_SIZE / 2.f);

    if (!_hasSelectedEnemy) {
        if (_hasSelectedEnemy = searchForEnemies()) return;

        constexpr float targetPlayerDistance = 16.f * 16.f;
        const sf::Vector2f playerPos(getWorld()->getPlayer()->getPosition().x, getWorld()->getPlayer()->getPosition().y + TILE_SIZE);

        const float dist = std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2);

        if (dist <= targetPlayerDistance) {
            const auto orbiter = std::shared_ptr<Orbiter>(new Orbiter(_orbiterAngle, OrbiterType::BEE.getId(), getWorld()->getPlayer().get()));
            orbiter->setDistance(_orbiterDistance);
            orbiter->setWorld(getWorld());
            orbiter->loadSprite(getWorld()->getSpriteSheet());
            getWorld()->addEntity(orbiter);
            deactivate();
            return;
        } else {
            targetPos = playerPos;
        }
    } else {
        targetPos = sf::Vector2f(_enemyTarget->getPosition().x, _enemyTarget->getPosition().y + _enemyTarget->getSpriteSize().y / 2.f);

        constexpr long long damageRate = 500LL;
        if (_hitBox.intersects(_enemyTarget->getHitBox()) && currentTimeMillis() - _lastDamageTime >= damageRate) {
            _enemyTarget->takeDamage(2);
            _lastDamageTime = currentTimeMillis();
        }
    }

    const float dx = targetPos.x - cLoc.x;
    const float dy = targetPos.y - cLoc.y;
    const float angle = std::atan2(dy, dx);
    constexpr float speed = 2.f;
    const float xa = speed * std::cos(angle);
    const float ya = speed * std::sin(angle);

    if (xa > 0) _movingDir = RIGHT;
    else if (xa < 0) _movingDir = LEFT;

    move(xa, ya); 
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _sprite.setPosition(getPosition());
}

bool BeeFamiliar::searchForEnemies() {
    if (getWorld()->getEnemies().size() > 0) {
        _enemyTarget = getWorld()->getEnemies().at((size_t)randomInt(0, getWorld()->getEnemies().size() - 1));
        if (_enemyTarget->getSaveId() == CACTOID && !_enemyTarget->isHostile()) return false;
        return true;
    }

    return false;
}

void BeeFamiliar::draw(sf::RenderTexture& surface) {
    const int xOffset = ((_numSteps >> _animSpeed) & 1) * TILE_SIZE;
    _sprite.setTextureRect(sf::IntRect(24 * TILE_SIZE + xOffset, 13 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
    if (_movingDir == RIGHT) _sprite.setScale(-1.f, 1.f);
    else _sprite.setScale(1.f, 1.f);

    surface.draw(_sprite);
}

void BeeFamiliar::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(24 * TILE_SIZE, 13 * TILE_SIZE, TILE_SIZE * 1, TILE_SIZE * 1));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE / 2.f, 0);
}

std::string BeeFamiliar::getSaveData() const {
    return std::to_string(_orbiterAngle) + ":" + std::to_string(_orbiterDistance);
}
