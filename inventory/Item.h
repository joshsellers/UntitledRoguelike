#ifndef _ITEM_H
#define _ITEM_H

#include <string>
#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include <functional>
#include "EquipmentType.h"
#include <memory>
#include "../world/entities/projectiles/ProjectileData.h"
#include <map>
#include "WeaponAnimationConfig.h"

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
    static const Item _PROJECTILE_SLIME_BALL;
    static const ProjectileData DATA_PROJECTILE_SLIME_BALL;
    static const Item SLIME_BALL;
    static const Item BANANA;
    static const Item BOWLING_BALL;
    static const Item WOOD;
    static const Item _PROJECTILE_SNOW_BALL;
    static const ProjectileData DATA_PROJECTILE_SNOW_BALL;
    static const Item STEROIDS;
    static const Item PROTEIN_SHAKE;
    static const Item BOTTLE_OF_MILK;
    static const Item WOOD_BOAT;
    static const Item LIQUID_NAP;
    static const Item LOCUS_LIFT;
    static const Item BONE;
    static const Item COIN_MAGNET;
    static const Item SCYTHE;
    static const Item MATMURA_HELMET;
    static const Item MATMURA_CHESTPLATE;
    static const Item MATMURA_LEGGINGS;
    static const Item MATMURA_BOOTS;
    static const Item BROADSWORD;
    static const Item ENERGY_DRINK;
    static const Item AUTOLASER;
    static const Item RAILGUN_DART;
    static const Item _PROJECTILE_RAILGUN_DART;
    static const ProjectileData DATA_PROJECTILE_RAILGUN_DART;
    static const Item RAILGUN;
    static const Item GASOLINE;
    static const Item _PROJECITLE_CHAINSAW;
    static const ProjectileData DATA_PROJECTILE_CHAINSAW;
    static const Item CHAINSAW;
    static const Item _PROJECTILE_CHEESE_SLICE;
    static const ProjectileData DATA_PROJECTILE_CHEESE_SLICE;
    static const Item ARROW;
    static const ProjectileData DATA_PROJECTILE_ARROW;
    static const Item BOW;
    static const Item CHEESE_SLICE;
    static const Item _PROJECTILE_TEAR_DROP;
    static const ProjectileData DATA_PROJECTILE_TEAR_DROP;
    static const Item CYCLOPS_EYE;
    static const Item LIQUID_EXERCISE;
    static const Item CACTUS_FLESH;
    static const Item _PROJECTILE_BLOOD_BALL;
    static const ProjectileData DATA_PROJECTILE_BLOOD_BALL;
    static const Item _PROJECTILE_LARGE_BLOOD_BALL;
    static const ProjectileData DATA_PROJECTILE_LARGE_BLOOD_BALL;
    static const Item FINGER_NAIL;
    static const Item BAD_VIBES_POTION;
    static const Item SPIKE_BALL;
    static const Item _PROJECTILE_THORN;
    static const ProjectileData DATA_PROJECTILE_THORN;
    static const Item _PROJECTILE_ROCK;
    static const ProjectileData DATA_PROJECTILE_ROCK;
    static const Item _PROJECTILE_POLLEN;
    static const ProjectileData DATA_PROJECTILE_POLLEN;
    static const Item HEALING_MIST;

    static std::vector<std::shared_ptr<Item>> ITEMS;

    static void fireTargetedProjectile(Entity* parent, const ProjectileData projData, std::string soundName = "NONE", int passThroughCount = 1);

    Item(const unsigned int id, const std::string name, const sf::IntRect textureRect, 
        const bool isStackable, const unsigned int stackLimit, const bool isConsumable, 
        const std::string description, EQUIPMENT_TYPE equipType, const int damage,
        const float hitBoxPos, const int hitBoxSize, const sf::Vector2f barrelPos, const bool isGun, const int value = 1, const bool isBuyable = true,
        const std::function<bool(Entity*)> use = [](Entity* parent) { return false; }, 
        const int magazineSize = 0, const bool isAutomatic = false, const unsigned int fireRateMilliseconds = 0,
        const unsigned int reloadTimeMilliseconds = 0, const bool isStartingItem = false, const bool isCustomItem = false, const std::string functionName = "NONE");

    static void createItem(const unsigned int id, const std::string name, const sf::IntRect textureRect,
        const bool isStackable, const unsigned int stackLimit, const bool isConsumable,
        const std::string description, EQUIPMENT_TYPE equipType, const int damage,
        const float hitBoxPos, const int hitBoxSize, const sf::Vector2f barrelPos, const bool isGun, const int value = 1, const bool isBuyable = true,
        const std::function<bool(Entity*)> use = [](Entity* parent) { return false; },
        const int magazineSize = 0, const bool isAutomatic = false, const unsigned int fireRateMilliseconds = 0,
        const unsigned int reloadTimeMilliseconds = 0, const bool isStartingItem = false, const bool isCustomItem = false, const std::string functionName = "NONE");

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

    const int getValue() const;
    const bool isBuyable() const;

    bool use(Entity* parent) const;

    EQUIPMENT_TYPE getEquipmentType() const;

    bool isUnlocked(unsigned int waveNumber) const;
    unsigned int getRequiredWave() const;

    bool isStartingItem() const;

    bool isAnimated() const;
    WeaponAnimationConfig getAnimationConfig() const;

    static void checkForIncompleteItemConfigs();

    friend class ModManager;
private:
    //static std::vector<Item> _builtInItems;

    static std::map<unsigned int, unsigned int> ITEM_UNLOCK_WAVE_NUMBERS;
    static std::map<unsigned int, WeaponAnimationConfig> ANIMATION_CONFIGS;

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

    const int _value;
    const bool _isBuyable;

    const bool _isStartingItem;

    const std::function<bool(Entity*)> _use;
    const bool _isCustomItem;
    const std::string _functionName;

    const EQUIPMENT_TYPE _equipType;
};

#endif