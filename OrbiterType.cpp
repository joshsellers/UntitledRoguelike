#include "OrbiterType.h"
#include "Entity.h"

const OrbiterType OrbiterType::SLIME_BALL(0, "Slime Ball", sf::IntRect(6, 4, 1, 1), 2, 32, 
    OrbiterAttackMethod::PROJECTILE, 1000LL, 0, "NONE", Item::DATA_PROJECTILE_SLIME_BALL);

const OrbiterType OrbiterType::BOWLING_BALL(1, "Bowling Ball", sf::IntRect(4, 10, 1, 1), 3.5f, 48,
    OrbiterAttackMethod::CONTACT, 500LL, 5);


std::vector<const OrbiterType*> OrbiterType::ORBITER_TYPES;

OrbiterType::OrbiterType(const unsigned int id, const std::string name, const sf::IntRect textureRect,
    const float orbitSpeed, const float orbitRadius, const OrbiterAttackMethod attackMethod, const long long attackFrequency,
    const int contactDamage, const std::string attackSoundName, const ProjectileData projectileData) :
    _id(id), _name(name), _textureRect(textureRect), _orbitSpeed(orbitSpeed), _contactDamage(contactDamage),
    _orbitRadius(orbitRadius), _attackMethod(attackMethod), _attackFrequency(attackFrequency),
    _attackSoundName(attackSoundName), _projectileData(projectileData)
{
    ORBITER_TYPES.push_back(this);
}

const unsigned int OrbiterType::getId() const {
    return _id;
}

const std::string OrbiterType::getName() const {
    return _name;
}

const sf::IntRect OrbiterType::getTextureRect() const {
    return _textureRect;
}

const float OrbiterType::getOrbitSpeed() const {
    return _orbitSpeed;
}

const float OrbiterType::getOrbitRadius() const {
    return _orbitRadius;
}

const OrbiterAttackMethod OrbiterType::getAttackMethod() const {
    return _attackMethod;
}

const long long OrbiterType::getAttackFrequency() const {
    return _attackFrequency;
}

const int OrbiterType::getContactDamage() const {
    return _contactDamage;
}

const std::string OrbiterType::getAttackSoundName() const {
    return _attackSoundName;
}

const ProjectileData OrbiterType::getProjectileData() const {
    return _projectileData;
}
