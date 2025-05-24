#define _USE_MATH_DEFINES

#include "OrbiterType.h"
#include "../Entity.h"
#include "Orbiter.h"
#include "../../World.h"
#include "../projectiles/ProjectileDataManager.h"
#include "../BeeFamiliar.h"

const OrbiterType OrbiterType::SLIME_BALL(0, "Slime Ball", sf::IntRect(6, 4, 1, 1), 2, 32, 
    OrbiterAttackMethod::PROJECTILE_CLOSEST_ENEMY, 1000LL, 0, false, "NONE", Item::DATA_PROJECTILE_SLIME_BALL);

const OrbiterType OrbiterType::BOWLING_BALL(1, "Bowling Ball", sf::IntRect(4, 10, 1, 1), 3.5f, 48,
    OrbiterAttackMethod::CONTACT, 500LL, 5);

const OrbiterType OrbiterType::CHEESE_SLICE(2, "Cheese Slice", sf::IntRect(3, 11, 1, 1), 1.f, 48,
    OrbiterAttackMethod::FIRE_ON_TIMEOUT, 3000LL, 0, false, "NONE", Item::DATA_PROJECTILE_CHEESE_SLICE);

const OrbiterType OrbiterType::EYE_BALL(3, "Eye", sf::IntRect(3, 12, 1, 1), 1.5f, 64,
    OrbiterAttackMethod::CUSTOM, 100LL, 0, true, "NONE", Item::DATA_PROJECTILE_TEAR_DROP,
    [](Orbiter* orbiterInstance) {
        constexpr float fireRange = 350.f;
        bool enemyWithinRange = false;
        for (auto& entity : orbiterInstance->getWorld()->getEnemies()) {
            if (entity->isEnemy() && entity->isActive() && (!entity->isInitiallyDocile() || entity->isHostile())) {
                float dist = std::sqrt(std::pow(orbiterInstance->_pos.x - entity->getPosition().x, 2) + std::pow(orbiterInstance->_pos.y - entity->getPosition().y, 2));
                if (dist <= fireRange) {
                    enemyWithinRange = true;
                    break;
                }
            }
        }

        if (enemyWithinRange) {
            float fireAngle = orbiterInstance->getAngle();
            fireAngle = fireAngle * (M_PI / 180.f);
            orbiterInstance->fireTargetedProjectile(
                fireAngle, orbiterInstance->_orbiterType->getProjectileData(), orbiterInstance->_orbiterType->getAttackSoundName()
            );
            orbiterInstance->_lastFireTime = currentTimeMillis();
        }
    }
);

const OrbiterType OrbiterType::BLOOD_BALL(4, "Blood Ball", sf::IntRect(4, 4, 1, 1), 3, 64,
    OrbiterAttackMethod::CUSTOM, 6100LL, 0, false, "NONE", Item::DATA_PROJECTILE_BLOOD_BALL,
    [](Orbiter* orbiterInstance) {
        float fireAngle = randomInt(0, 360);
        constexpr int angleCount = 8;
        for (int i = 0; i < angleCount; i++) {
            fireAngle += (360.f / (float)angleCount) * i;
            float finalFireAngle = fireAngle;
            if (finalFireAngle >= 360.f) finalFireAngle -= 360.f;
            finalFireAngle = finalFireAngle * (M_PI / 180.f);

            orbiterInstance->fireTargetedProjectile(
                finalFireAngle, orbiterInstance->_orbiterType->getProjectileData(), orbiterInstance->_orbiterType->getAttackSoundName()
            );
        }

        orbiterInstance->deactivate();
    }, true
);

const OrbiterType OrbiterType::SPIKE_BALL(5, "Spike Ball", sf::IntRect(6, 37, 2, 2), 2, 64,
    OrbiterAttackMethod::CONTACT, 500LL, 7
);

const OrbiterType OrbiterType::CREAM_CLOCKWISE_OUT(6, "CREAM_CO", sf::IntRect(60, 105, 1, 1), 1, 0,
    OrbiterAttackMethod::CUSTOM, 4000LL, 0, false, "NONE", {},
    [](Orbiter* orbiterInstance) {
        const float angle = orbiterInstance->getAngle();
        orbiterInstance->fireTargetedProjectile(
            degToRads(angle), ProjectileDataManager::getData("_PROJECTILE_CREAM_ORB"), orbiterInstance->_orbiterType->getAttackSoundName()
        );
        orbiterInstance->deactivate();
    }, true
);

const OrbiterType OrbiterType::CREAM_COUNTERCLOCKWISE_OUT(7, "CREAM_CCO", sf::IntRect(60, 105, 1, 1), -1, 0,
    OrbiterAttackMethod::CUSTOM, 4000LL, 0, false, "NONE", {},
    [](Orbiter* orbiterInstance) {
        const float angle = orbiterInstance->getAngle();
        orbiterInstance->fireTargetedProjectile(
            degToRads(angle), ProjectileDataManager::getData("_PROJECTILE_CREAM_ORB"), orbiterInstance->_orbiterType->getAttackSoundName()
        );
        orbiterInstance->deactivate();
    }, true
);

const OrbiterType OrbiterType::CREAM_CLOCKWISE_IN(8, "CREAM_CI", sf::IntRect(60, 105, 1, 1), 1, 0,
    OrbiterAttackMethod::CUSTOM, 4000LL, 0, false, "NONE", {},
    [](Orbiter* orbiterInstance) {
        float angle = orbiterInstance->getAngle();
        angle += 180.f;
        if (angle >= 360.f) angle -= 360.f;
        orbiterInstance->fireTargetedProjectile(
            degToRads(angle), ProjectileDataManager::getData("_PROJECTILE_CREAM_ORB"), orbiterInstance->_orbiterType->getAttackSoundName()
        );
        orbiterInstance->deactivate();
    }, true
);

const OrbiterType OrbiterType::CREAM_COUNTERCLOCKWISE_IN(9, "CREAM_CCI", sf::IntRect(60, 105, 1, 1), -1, 0,
    OrbiterAttackMethod::CUSTOM, 4000LL, 0, false, "NONE", {},
    [](Orbiter* orbiterInstance) {
        float angle = orbiterInstance->getAngle();
        angle += 180.f;
        if (angle >= 360.f) angle -= 360.f;
        orbiterInstance->fireTargetedProjectile(
            degToRads(angle), ProjectileDataManager::getData("_PROJECTILE_CREAM_ORB"), orbiterInstance->_orbiterType->getAttackSoundName()
        );
        orbiterInstance->deactivate();
    }, true
);

const OrbiterType OrbiterType::PIZZA_CHEFBOSS(10, "PIZZA_CHEFBOSS", sf::IntRect(13, 13, 1, 1), 1.5f, 75.f,
    OrbiterAttackMethod::CUSTOM, 4000LL, 0, true, "NONE", {},
    [](Orbiter* orbiterInstance) {
        float fireAngle = orbiterInstance->getAngle();
        fireAngle += 180.f;
        if (fireAngle >= 360.f) fireAngle -= 360.f;
        orbiterInstance->fireTargetedProjectile(
            degToRads(fireAngle), ProjectileDataManager::getData("_PROJECTILE_CHEFBOSS_PIZZA"), orbiterInstance->_orbiterType->getAttackSoundName(),
            {8.f, 0.f}
        );
        orbiterInstance->deactivate();
    }, true
);

const OrbiterType OrbiterType::BUBBLE(11, "Bubble", sf::IntRect(19, 13, 1, 1), 1.f, 24,
    OrbiterAttackMethod::CUSTOM, 750LL, 0, false, "NONE", {},
    [](Orbiter* orbiterInstance) {
        constexpr float fireRange = 350.f * 350.f;
        bool enemyWithinRange = false;
        for (auto& entity : orbiterInstance->getWorld()->getEnemies()) {
            if (entity->isEnemy() && entity->isActive() && (!entity->isInitiallyDocile() || entity->isHostile())) {
                float dist = std::pow(orbiterInstance->_pos.x - entity->getPosition().x, 2) + std::pow(orbiterInstance->_pos.y - entity->getPosition().y, 2);
                if (dist <= fireRange) {
                    enemyWithinRange = true;
                    break;
                }
            }
        }
        if (!enemyWithinRange) return;

        constexpr int projCount = 8;
        constexpr float angleIncrement = 360.f / projCount;
        for (int i = 0; i < projCount; i++) {
            orbiterInstance->fireTargetedProjectile(
                degToRads(angleIncrement * i), ProjectileDataManager::getData("_PROJECTILE_BUBBLE"), orbiterInstance->_orbiterType->getAttackSoundName()
            );
        }
        orbiterInstance->_lastFireTime = currentTimeMillis();
    }, false, true
);

const OrbiterType OrbiterType::BEE(12, "Bee", sf::IntRect(24, 13, 1, 1), 2.f, 16.f,
    OrbiterAttackMethod::CUSTOM, 50LL, 0, false, "NONE", {},
    [](Orbiter* orbiterInstance) {
        orbiterInstance->_lastFireTime = currentTimeMillis();

        constexpr float fireRange = 350.f * 350.f;
        bool enemyWithinRange = false;
        for (auto& entity : orbiterInstance->getWorld()->getEnemies()) {
            if (entity->isEnemy() && entity->isActive() && (!entity->isInitiallyDocile() || entity->isHostile())) {
                float dist = std::pow(orbiterInstance->_pos.x - entity->getPosition().x, 2) + std::pow(orbiterInstance->_pos.y - entity->getPosition().y, 2);
                if (dist <= fireRange) {
                    enemyWithinRange = true;
                    break;
                }
            }
        }
        if (!enemyWithinRange) return;

        const auto bee = std::shared_ptr<BeeFamiliar>(new BeeFamiliar(orbiterInstance->getPosition(), orbiterInstance->getAngle(), orbiterInstance->getDistance()));
        bee->setWorld(orbiterInstance->getWorld());
        bee->loadSprite(orbiterInstance->getWorld()->getSpriteSheet());
        orbiterInstance->getWorld()->addEntity(bee);
        orbiterInstance->deactivate();
    }
);

const OrbiterType OrbiterType::FROG(13, "Frog", sf::IntRect(104, 37, 1, 1), 2.f, 64.f,
    OrbiterAttackMethod::CUSTOM, 0LL, 0, true, "NONE", {},
    [](Orbiter* orbiterInstance) {
        float fireAngle = orbiterInstance->getAngle();
        fireAngle += 180.f;
        if (fireAngle >= 360.f) fireAngle -= 360.f;
        orbiterInstance->fireTargetedProjectile(
            degToRads(fireAngle), ProjectileDataManager::getData("_PROJECTILE_ORBITAL_FROG"), orbiterInstance->_orbiterType->getAttackSoundName(),
            { 8.f, 0.f }
        );
        orbiterInstance->deactivate();
    }, true
);


std::vector<const OrbiterType*> OrbiterType::ORBITER_TYPES;

OrbiterType::OrbiterType(const unsigned int id, const std::string name, const sf::IntRect textureRect,
    const float orbitSpeed, const float orbitRadius, const OrbiterAttackMethod attackMethod, const long long attackFrequency,
    const int contactDamage, const bool rotateSprite, const std::string attackSoundName, const ProjectileData projectileData,
    const std::function<void(Orbiter* orbiterInstance)> attack, const bool onlyHitPlayer, const bool addVelocityToProjectiles) :
    _id(id), _name(name), _textureRect(textureRect), _orbitSpeed(orbitSpeed), _contactDamage(contactDamage),
    _orbitRadius(orbitRadius), _attackMethod(attackMethod), _attackFrequency(attackFrequency),
    _attackSoundName(attackSoundName), _projectileData(projectileData), _attack(attack), _rotateSprite(rotateSprite), _onlyHitPlayer(onlyHitPlayer),
    _addVelocityToProjectiles(addVelocityToProjectiles)
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

const bool OrbiterType::rotateSprite() const {
    return _rotateSprite;
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

void OrbiterType::attack(Orbiter* orbiterInstance) const {
    return _attack(orbiterInstance);
}

const bool OrbiterType::onlyHitPlayer() const {
    return _onlyHitPlayer;
}

const bool OrbiterType::addVelocityToProjectiles() const {
    return _addVelocityToProjectiles;
}
