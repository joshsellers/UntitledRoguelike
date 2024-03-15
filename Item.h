#ifndef _ITEM_H
#define _ITEM_H

#include <string>
#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include <functional>
#include "EquipmentType.h"
#include "ProjectileData.h"

class Entity;

class Item {
public:
    static const Item TOP_HAT;
    static const Item TUX_VEST;
    static const Item TUX_PANTS;
    static const Item DRESS_SHOES;
    static const Item SOMBRERO;
    static const Item AXE;
    static const Item DAGGER;
    static const Item BULLET_455;
    static const ProjectileData DATA_B455;
    static const Item HOWDAH;
    static const Item POD;
    static const ProjectileData DATA_POD;
    static const Item POD_LAUNCHER;
    static const Item WIFE_BEATER;
    static const Item JORTS;
    static const Item WHITE_TENNIS_SHOES;
    static const Item APPLE;
    static const Item RED_TEE_SHIRT;
    static const Item OVERALLS;
    static const Item BOOTS;
    static const Item RIFLE_ROUND;
    static const ProjectileData DATA_RIFLE_ROUND;
    static const Item ASSAULT_RIFLE;
    static const Item PENNY;
    static const Item LIGHT_LASER_CHARGE;
    static const Item _PROJECTILE_LIGHT_LASER_CHARGE;
    static const ProjectileData DATA_PROJECTILE_LIGHT_LASER_CHARGE;
    static const Item LASER_PISTOL;
    static const Item PROPANE;
    static const Item _PROJECTILE_PROPANE;
    static const ProjectileData DATA_PROJECTILE_PROPANE;
    static const Item BLOW_TORCH;

    static std::vector<const Item*> ITEMS;

    static void fireTargetedProjectile(const float velocity, Entity* parent, const ProjectileData projData, std::string soundName = "NONE");

    Item(const unsigned int id, const std::string name, const sf::IntRect textureRect, 
        const bool isStackable, const unsigned int stackLimit, const bool isConsumable, 
        const std::string description, EQUIPMENT_TYPE equipType, const int damage,
        const float hitBoxPos, const int hitBoxSize, const sf::Vector2f barrelPos, const bool isGun,
        const std::function<bool(Entity*)> use = [](Entity* parent) { return false; }, 
        const int magazineSize = 0, const bool isAutomatic = false, const unsigned int fireRateMilliseconds = 0,
        const unsigned int reloadTimeMilliseconds = 0);

    unsigned int getId() const;
    std::string getName() const;
    std::string getDescription() const;

    int getDamage() const;
    float getHitBoxPos() const;
    int getHitBoxSize() const;
    bool isMeleeWeapon() const;

    sf::Vector2f getBarrelPos() const;
    bool isGun() const;
    const int getMagazineSize() const;
    bool isAutomatic() const;
    const unsigned int getFireRateMilliseconds() const;
    const unsigned int getReloadTimeMilliseconds() const;

    sf::IntRect getTextureRect() const;

    bool isStackable() const;
    unsigned int getStackLimit() const;
    unsigned int getAmmoId() const;

    bool isConsumable() const;

    bool use(Entity* parent) const;

    EQUIPMENT_TYPE getEquipmentType() const;

private:
    const unsigned int _id;
    const std::string _name;
    const std::string _description;

    const int _damage;
    const float _hitBoxPos;
    const int _hitBoxSize;

    const sf::Vector2f _barrelPos;
    const bool _isGun;
    const int _magazineSize;
    const bool _isAutomatic;
    const unsigned int _fireRateMilliseconds;
    const unsigned int _reloadTimeMilliseconds;

    const sf::IntRect _textureRect;

    const bool _isStackable;
    const unsigned int _stackLimit;

    const bool _isConsumable;

    const std::function<bool(Entity*)> _use;

    const EQUIPMENT_TYPE _equipType;
};

#endif