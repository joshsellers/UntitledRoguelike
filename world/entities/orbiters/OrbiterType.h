#ifndef _ORBITER_TYPE_H
#define _ORBITER_TYPE_H

#include <vector>
#include <string>
#include <SFML/Graphics/Rect.hpp>
#include "../projectiles/ProjectileData.h"
#include <functional>

enum class OrbiterAttackMethod {
    CONTACT,
    PROJECTILE_CLOSEST_ENEMY,
    FIRE_ON_TIMEOUT,
    CUSTOM
};

class Orbiter;

class OrbiterType {
public:
    static const OrbiterType SLIME_BALL;
    static const OrbiterType BOWLING_BALL;
    static const OrbiterType CHEESE_SLICE;

    static std::vector<const OrbiterType*> ORBITER_TYPES;

    OrbiterType(const unsigned int id, const std::string name, const sf::IntRect textureRect, const float orbitSpeed, const float orbitRadius, 
        const OrbiterAttackMethod attackMethod, const long long attackFrequency, const int contactDamage = 0,
        const std::string attackSoundName = "NONE", const ProjectileData projectileData = {0, 0, sf::IntRect(), false}, 
        const std::function<void(Orbiter*)> attack = [](Orbiter* orbiterInstance) {});

    const unsigned int getId() const;
    const std::string getName() const;

    const sf::IntRect getTextureRect() const;

    const float getOrbitSpeed() const;
    const float getOrbitRadius() const;
    
    const OrbiterAttackMethod getAttackMethod() const;
    const long long getAttackFrequency() const;

    const int getContactDamage() const;

    const std::string getAttackSoundName() const;
    const ProjectileData getProjectileData() const;

    void attack(Orbiter* orbiterInstance) const;

private:
    const unsigned int _id;
    const std::string _name;

    const sf::IntRect _textureRect;

    const float _orbitSpeed;
    const float _orbitRadius;

    const OrbiterAttackMethod _attackMethod;
    const long long _attackFrequency;

    const int _contactDamage;

    const std::string _attackSoundName;
    const ProjectileData _projectileData;

    const std::function<void(Orbiter* orbiterInstance)> _attack;
};

#endif