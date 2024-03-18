#define _USE_MATH_DEFINES

#include "Orbiter.h"
#include <math.h>
#include "SoundManager.h"
#include "Projectile.h"
#include "World.h"

Orbiter::Orbiter(sf::Vector2f pos, float distance, float speed, Entity* parent) : Entity(pos, speed, 16, 16, false) {
    _distance = distance;
    _speed = speed;
    _parent = parent;
}

void Orbiter::update() {
    const sf::Vector2f centerPoint(_parent->getPosition().x + 8, _parent->getPosition().y + 16);

    _pos.x = centerPoint.x + getDistance() * std::cos(_angle * (M_PI / 180.f));
    _pos.y = centerPoint.y + getDistance() * std::sin(_angle * (M_PI / 180.f));

    _angle += getSpeed();
    if (getSpeed() > 0 && getAngle() > 360) _angle = 0;
    else if (getSpeed() < 0 && getAngle() < 0) _angle = 360;

    subUpdate();
    _sprite.setPosition(getPosition());
}

void Orbiter::fireTargetedProjectile(sf::Vector2f targetPos, const ProjectileData projData, std::string soundName) {
    const sf::Vector2f centerPoint(getPosition().x, getPosition().y);
    sf::Vector2f spawnPos(centerPoint.x - 8, centerPoint.y);

    double x = (double)(targetPos.x - centerPoint.x);
    double y = (double)(targetPos.y - centerPoint.y);

    float angle = (float)((std::atan2(y, x)));

    std::shared_ptr<Projectile> proj = std::shared_ptr<Projectile>(new Projectile(
        spawnPos, this, angle, projData.baseVelocity, projData
    ));
    proj->loadSprite(getWorld()->getSpriteSheet());
    proj->setWorld(getWorld());
    getWorld()->addEntity(proj);

    if (soundName != "NONE") SoundManager::playSound(soundName);
}

float Orbiter::getAngle() {
    return _angle;
}

float Orbiter::getDistance() {
    return _distance;
}

float Orbiter::getSpeed() {
    return _speed;
}

Entity* Orbiter::getParent() {
    return _parent;
}
