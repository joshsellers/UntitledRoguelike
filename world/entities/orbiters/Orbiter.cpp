#define _USE_MATH_DEFINES

#include "Orbiter.h"
#include <math.h>
#include "../../World.h"
#include "../projectiles/Projectile.h"
#include "../../../core/SoundManager.h"
#include "../projectiles/ProjectilePoolManager.h"

Orbiter::Orbiter(float angle, const unsigned int orbiterTypeId, Entity* parent) : _orbiterTypeId(orbiterTypeId), _angle(angle), 
Entity(ORBITER, parent->getPosition(), OrbiterType::ORBITER_TYPES.at(orbiterTypeId)->getOrbitSpeed(), 16, 16, false) {
    _orbiterType = OrbiterType::ORBITER_TYPES.at(_orbiterTypeId);
    _distance = _orbiterType->getOrbitRadius();
    _speed = _orbiterType->getOrbitSpeed();

    _parent = parent;

    if (_orbiterType->getAttackMethod() == OrbiterAttackMethod::CONTACT) {
        _hitBox.width = _orbiterType->getTextureRect().width * TILE_SIZE;
        _hitBox.height = _orbiterType->getTextureRect().height * TILE_SIZE;
        _hitBox.left = _pos.x - _hitBox.width / 2;
        _hitBox.top = _pos.y - _hitBox.width / 2;

        setMaxHitPoints(999999);
        heal(getMaxHitPoints());
    }

    _entityType = "dontblockplayershots";
    _isOrbiter = true;

    _lastFireTime = currentTimeMillis();
}

void Orbiter::update() {
    const sf::Vector2f centerPoint(
        _parent->getPosition().x + (_centerPointOffsetWasReset ? _centerPointOffset.x : (_parent->getSpriteSize().x * TILE_SIZE) / 2), 
        _parent->getPosition().y + (_centerPointOffsetWasReset ? _centerPointOffset.y : (_parent->getSpriteSize().y * TILE_SIZE) / 2)
    );

    _pos.x = centerPoint.x + getDistance() * std::cos(_angle * (M_PI / 180.f));
    _pos.y = centerPoint.y + getDistance() * std::sin(_angle * (M_PI / 180.f));

    _angle += getSpeed();
    if (getSpeed() > 0 && getAngle() > 360) _angle = 0;
    else if (getSpeed() < 0 && getAngle() < 0) _angle = 360;

    attack();
    _sprite.setPosition(getPosition());

    if (_orbiterType->getAttackMethod() == OrbiterAttackMethod::CONTACT) {
        _hitBox.left = getPosition().x - _hitBox.width / 2;
        _hitBox.top = getPosition().y - _hitBox.width / 2;
    }
}

void Orbiter::attack() {
    if (currentTimeMillis() - _lastFireTime >= _orbiterType->getAttackFrequency() + _attackFreqOffset) {
        switch (_orbiterType->getAttackMethod()) {
            case OrbiterAttackMethod::CONTACT:
                contactAttack();
                break;
            case OrbiterAttackMethod::PROJECTILE_CLOSEST_ENEMY:
                projectileAttack();
                break;
            case OrbiterAttackMethod::FIRE_ON_TIMEOUT:
                projectileAttack();
                deactivate();
                break;
            case OrbiterAttackMethod::CUSTOM:
                _orbiterType->attack(this);
                break;
        }
    }

    if (!getParent()->isActive()) deactivate();
}

void Orbiter::projectileAttack() {
    if (_orbiterType->getAttackMethod() == OrbiterAttackMethod::PROJECTILE_CLOSEST_ENEMY) {
        const float fireRange = 350.f;
        float closestDistance = fireRange;
        std::shared_ptr<Entity> closestEnemy = nullptr;
        for (auto& entity : getWorld()->getEnemies()) {
            if (entity->isEnemy() && entity->isActive() && (!entity->isInitiallyDocile() || entity->isHostile())) {
                float dist = std::sqrt(std::pow(_pos.x - entity->getPosition().x, 2) + std::pow(_pos.y - entity->getPosition().y, 2));
                if (dist <= fireRange && dist < closestDistance) {
                    closestDistance = dist;
                    closestEnemy = entity;
                }
            }
        }

        if (closestEnemy != nullptr) {
            fireTargetedProjectile(
                sf::Vector2f(closestEnemy->getPosition().x, closestEnemy->getPosition().y + closestEnemy->getSpriteSize().y / 2),
                _orbiterType->getProjectileData(), _orbiterType->getAttackSoundName()
            );
            _lastFireTime = currentTimeMillis();
        }
    } else if (_orbiterType->getAttackMethod() == OrbiterAttackMethod::FIRE_ON_TIMEOUT) {
        float fireAngle = _angle;
        //if (fireAngle < 0) fireAngle += 360;
        fireAngle = fireAngle * (M_PI / 180.f);
        fireTargetedProjectile(fireAngle, _orbiterType->getProjectileData(), _orbiterType->getAttackSoundName());
    }
}

void Orbiter::contactAttack() {
    for (auto& entity : getWorld()->getNearbyEntites(getPosition(), true)) {
        if (entity->isActive() && !entity->isDormant() 
            && !entity->compare(this) && !entity->compare(getParent()) && entity->isDamageable() && entity->getEntityType() != "dontblockplayershots"
            && entity->getHitBox().intersects(getHitBox())) {
            entity->takeDamage(_orbiterType->getContactDamage());
            _lastFireTime = currentTimeMillis();
            break;
        }
    }
}

void Orbiter::fireTargetedProjectile(sf::Vector2f targetPos, const ProjectileData projData, std::string soundName) {
    const sf::Vector2f centerPoint(getPosition().x, getPosition().y);

    double x = (double)(targetPos.x - centerPoint.x);
    double y = (double)(targetPos.y - centerPoint.y);

    float angle = (float)((std::atan2(y, x)));

    fireTargetedProjectile(angle, projData, soundName);
}

void Orbiter::fireTargetedProjectile(float angle, const ProjectileData projData, std::string soundName, sf::Vector2f centerOffset) {
    const sf::Vector2f centerPoint(getPosition().x, getPosition().y);
    sf::Vector2f spawnPos(centerPoint.x - 8 + centerOffset.x, centerPoint.y + centerOffset.y);

    bool addParentVelocity = (_orbiterType->getAttackMethod() != OrbiterAttackMethod::FIRE_ON_TIMEOUT && _orbiterType->getAttackMethod() != OrbiterAttackMethod::CUSTOM)
        || _orbiterType->addVelocityToProjectiles();
    
    ProjectilePoolManager::addProjectile(spawnPos, this, angle, projData.baseVelocity, projData, 
        _orbiterType->getId() == OrbiterType::CHEESE_SLICE.getId() || _orbiterType->onlyHitPlayer(), 0, addParentVelocity);

    if (soundName != "NONE") SoundManager::playSound(soundName);
}

void Orbiter::draw(sf::RenderTexture& surface) {
    if (_orbiterType->rotateSprite()) _sprite.setRotation(_angle);
    surface.draw(_sprite);
}

const unsigned int Orbiter::getOrbiterTypeId() const {
    return _orbiterTypeId;
}

float Orbiter::getAngle() const {
    return _angle;
}

void Orbiter::setDistance(float distance) {
    _distance = distance;
}

float Orbiter::getDistance() const {
    return _distance;
}

float Orbiter::getSpeed() const {
    return _speed;
}

void Orbiter::setCenterPointOffset(float xOffset, float yOffset) {
    setCenterPointOffset(sf::Vector2f(xOffset, yOffset));
}

void Orbiter::setCenterPointOffset(sf::Vector2f offset) {
    _centerPointOffset = offset;
    _centerPointOffsetWasReset = true;
}

void Orbiter::setAttackFrequencyOffset(long long attackFreqOffset) {
    _attackFreqOffset = attackFreqOffset;
}

Entity* Orbiter::getParent() const {
    return _parent;
}

std::string Orbiter::getSaveData() const {
    return std::to_string(_orbiterTypeId) + ":" + getParent()->getUID() + ":" + std::to_string(_angle) + ":" + std::to_string(_distance);
}

void Orbiter::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    sf::IntRect tRect = _orbiterType->getTextureRect();
    _sprite.setTextureRect(sf::IntRect(tRect.left * TILE_SIZE, tRect.top * TILE_SIZE, tRect.width * TILE_SIZE, tRect.height * TILE_SIZE));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(_sprite.getTextureRect().width / 2, _sprite.getTextureRect().height / 2);
}
