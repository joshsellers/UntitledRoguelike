#include "Item.h"
#include "World.h"
#include "Projectile.h"
#include <iostream>
#include "SoundManager.h"

const Item Item::TOP_HAT(0, "Top hat", sf::IntRect(0, 13, 1, 1), false, 0, false,
    "A fancy hat",
    EQUIPMENT_TYPE::CLOTHING_HEAD, 0, 0, 0, sf::Vector2f(), false
); 

const Item Item::TUX_VEST(1, "Tuxedo Vest", sf::IntRect(0, 26, 1, 1), false, 0, false,
    "A tuxedo vest\nFor festive occasions",
    EQUIPMENT_TYPE::CLOTHING_BODY, 0, 0, 0, sf::Vector2f(), false
);

const Item Item::TUX_PANTS(2, "Tuxedo Pants", sf::IntRect(4, 26, 1, 1), false, 0, false,
    "Tuxedo pants, for a tuxedo\nFor festive occasions",
    EQUIPMENT_TYPE::CLOTHING_LEGS, 0, 0, 0, sf::Vector2f(), false
);

const Item Item::DRESS_SHOES(3, "Dress Shoes", sf::IntRect(8, 26, 1, 1), false, 0, false,
    "Some nice shoes\nFor festive occasions",
    EQUIPMENT_TYPE::CLOTHING_FEET, 0, 0, 0, sf::Vector2f(), false
);

const Item Item::SOMBRERO(4, "Sombrero", sf::IntRect(12, 13, 1, 1), false, 0, false,
    "A nice hat to keep the\nsun out of your eyes",
    EQUIPMENT_TYPE::CLOTHING_HEAD, 0, 0, 0, sf::Vector2f(), false
);

const Item Item::AXE(5, "Axe", sf::IntRect(18, 3, 1, 1), false, 0, false,
    "A large, formidible axe",
    EQUIPMENT_TYPE::TOOL, 5, 18.f, 20, sf::Vector2f(), false
);

const Item Item::DAGGER(6, "Dagger", sf::IntRect(18, 0, 1, 1), false, 0, false,
    "Careful! It's sharp",
    EQUIPMENT_TYPE::TOOL, 3, 12.f, 5, sf::Vector2f(), false
);

const Item Item::BULLET_455(7, "Pistol Round", sf::IntRect(22, 3, 1, 1), true, 9999, false,
    "A centrefire black powder cartridge\nFor use with revolvers and\nother handguns",
    EQUIPMENT_TYPE::AMMO, 10, 0, 0, sf::Vector2f(), false
);
const ProjectileData Item::DATA_B455(Item::BULLET_455.getId(), 5, sf::IntRect(6, 8, 4, 4), true);

const Item Item::HOWDAH(8, "Howdah Pistol", sf::IntRect(22, 0, 1, 1), false, BULLET_455.getId(), false,
    "A large handgun",
    EQUIPMENT_TYPE::TOOL, 10, 0, 0, sf::Vector2f(20, 6), true,
    [](Entity* parent) {
        fireTargetedProjectile(DATA_B455.baseVelocity, parent, DATA_B455, "revolver");
        return false;
    }, 8, false, 0, 200
);

const Item Item::POD(9, "Pod", sf::IntRect(29, 3, 1, 1), true, 9999, false,
    "A large pod\nAmmunition for the Pod Launcher",
    EQUIPMENT_TYPE::AMMO, 50, 0, 0, sf::Vector2f(), false
);
const ProjectileData Item::DATA_POD(Item::POD.getId(), 3, sf::IntRect(4, 8, 8, 8), true);

const Item Item::POD_LAUNCHER(10, "Pod Launcher", sf::IntRect(29, 0, 1, 1), false, POD.getId(), false,
    "Don't vape, kids",
    EQUIPMENT_TYPE::TOOL, 10, 0, 0, sf::Vector2f(30, 0), true,
    [](Entity* parent) {
        fireTargetedProjectile(DATA_POD.baseVelocity, parent, DATA_POD, "slip");
        return false;
    }, 1, false, 0, 350
);

const Item Item::WIFE_BEATER(11, "Wife Beater", sf::IntRect(12, 26, 1, 1), false, 0, false,
    "It's dirty",
    EQUIPMENT_TYPE::CLOTHING_BODY, 0, 0, 0, sf::Vector2f(), false
);

const Item Item::JORTS(12, "Jorts", sf::IntRect(16, 26, 1, 1), false, 0, false,
    "Don't wear these",
    EQUIPMENT_TYPE::CLOTHING_LEGS, 0, 0, 0, sf::Vector2f(), false
);

const Item Item::WHITE_TENNIS_SHOES(13, "White Tennis Shoes", sf::IntRect(20, 26, 1, 1), false, 0, false,
    "These help you go a little faster",
    EQUIPMENT_TYPE::CLOTHING_FEET, 0, 0, 0, sf::Vector2f(), false
);

const Item Item::APPLE(14, "Apple", sf::IntRect(2, 10, 1, 1), true, 32, true, "Something something an apple a day",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false,
    [](Entity* parent) {
        if (parent->getHitPoints() < parent->getMaxHitPoints()) {
            parent->heal(5);
            return true;
        }
        return false;
    }
);

const Item Item::RED_TEE_SHIRT(15, "Red T-Shirt", sf::IntRect(24, 26, 1, 1), false, 0, false, 
    "Might be comfy",
    EQUIPMENT_TYPE::CLOTHING_BODY, 0, 0, 0, sf::Vector2f(), false
);

const Item Item::OVERALLS(16, "Overalls", sf::IntRect(28, 26, 1, 1), false, 0, false,
    "For when you're overall the bullshit",
    EQUIPMENT_TYPE::CLOTHING_LEGS, 0, 0, 0, sf::Vector2f(), false
);

const Item Item::BOOTS(17, "Boots", sf::IntRect(32, 26, 1, 1), false, 0, false,
    "If I can't wear my cowboy boots, I ain't goin",
    EQUIPMENT_TYPE::CLOTHING_FEET, 0, 0, 0, sf::Vector2f(), false
);

const Item Item::RIFLE_ROUND(18, "Rifle Round", sf::IntRect(36, 3, 1, 1), true, 9999, false,
    "Heavier than a pistol round\nFor use with rifles",
    EQUIPMENT_TYPE::AMMO, 6, 0, 0, sf::Vector2f(), false
);
const ProjectileData Item::DATA_RIFLE_ROUND(Item::RIFLE_ROUND.getId(), 8, sf::IntRect(6, 8, 4, 4), true);

const Item Item::ASSAULT_RIFLE(19, "Assault Rifle", sf::IntRect(36, 0, 1, 1), false, RIFLE_ROUND.getId(), false,
    "Big scary shoots fast",
    EQUIPMENT_TYPE::TOOL, 6, 0, 0, sf::Vector2f(20, 3), true,
    [](Entity* parent) {
        fireTargetedProjectile(DATA_RIFLE_ROUND.baseVelocity, parent, DATA_RIFLE_ROUND, "ar");
        return false;
    }, 30, true, 150, 2000
);

const Item Item::PENNY(20, "Penny", sf::IntRect(3, 10, 1, 1), true, 1000000000, false,
    "Use these to buy stuff", 
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false
);

const Item Item::LIGHT_LASER_CHARGE(21, "Light Laser Charge", sf::IntRect(43, 3, 1, 1), true, 9999, false,
    "Ammo for a laser pistol", 
    EQUIPMENT_TYPE::AMMO, 12, 0, 0, sf::Vector2f(), false
);

const Item Item::_PROJECTILE_LIGHT_LASER_CHARGE(22, "_LIGHT_LASER_PROJECTILE", sf::IntRect(43, 4, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 12, 0, 0, sf::Vector2f(), false
);
const ProjectileData Item::DATA_PROJECTILE_LIGHT_LASER_CHARGE(Item::_PROJECTILE_LIGHT_LASER_CHARGE.getId(), 10, sf::IntRect(6, 8, 4, 4), true);

const Item Item::LASER_PISTOL(23, "Laser Pistol", sf::IntRect(43, 0, 1, 1), false, LIGHT_LASER_CHARGE.getId(), false,
    "Pew Pew",
    EQUIPMENT_TYPE::TOOL, 12, 0, 0, sf::Vector2f(14, 4), true,
    [](Entity* parent) {
        fireTargetedProjectile(DATA_PROJECTILE_LIGHT_LASER_CHARGE.baseVelocity, parent, DATA_PROJECTILE_LIGHT_LASER_CHARGE, "laser_pistol");
        return false;
    }, 12, false, 0, 200
);

const Item Item::PROPANE(24, "Propane", sf::IntRect(50, 3, 1, 1), true, 999999, false,
    "\"Propane may be a clean burning fuel,\nbut she can also be a dirty girl.\"",
    EQUIPMENT_TYPE::AMMO, 3, 0, 0, sf::Vector2f(), false
);

const Item Item::_PROJECTILE_PROPANE(25, "_PROPANE_PROJECTILE", sf::IntRect(50, 4, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 3, 0, 0, sf::Vector2f(), false
);
const ProjectileData Item::DATA_PROJECTILE_PROPANE(Item::_PROJECTILE_PROPANE.getId(), 10, sf::IntRect(0, 0, 16, 16), true, 5, true, 3, 0);

const Item Item::BLOW_TORCH(26, "Blow Torch", sf::IntRect(50, 0, 1, 1), false, PROPANE.getId(), false,
    "It's a blow torch, but it\nseems like something's broken...",
    EQUIPMENT_TYPE::TOOL, 3, 0, 0, sf::Vector2f(12, 12), true,
    [](Entity* parent) {
        fireTargetedProjectile(DATA_PROJECTILE_PROPANE.baseVelocity, parent, DATA_PROJECTILE_PROPANE, "NONE");
        if (SoundManager::getStatus("blowtorch") != sf::SoundSource::Playing) SoundManager::playSound("blowtorch");
        return false;
    }, 750, true, 20, 2750
);;

std::vector<const Item*> Item::ITEMS;

Item::Item(const unsigned int id, const std::string name, const sf::IntRect textureRect, const bool isStackable,
    const unsigned int stackLimit, const bool isConsumable,
    std::string description, EQUIPMENT_TYPE equipType, const int damage, const float hitBoxPos,
    const int hitBoxSize, const sf::Vector2f barrelPos, const bool isGun, const std::function<bool(Entity*)> use, const int magazineSize,
    const bool isAutomatic, const unsigned int fireRateMilliseconds, const unsigned int reloadTimeMilliseconds) :
    _id(id), _name(name), _textureRect(
        sf::IntRect(
            textureRect.left << SPRITE_SHEET_SHIFT, 
            textureRect.top << SPRITE_SHEET_SHIFT, 
            textureRect.width << SPRITE_SHEET_SHIFT,
            textureRect.height << SPRITE_SHEET_SHIFT
        )), 
    _isStackable(isStackable), _stackLimit(stackLimit), 
    _isConsumable(isConsumable), _use(use), _description(description), 
    _equipType(equipType), _damage(damage), _hitBoxSize(hitBoxSize), _hitBoxPos(hitBoxPos), _barrelPos(barrelPos),
    _isGun(isGun), _magazineSize(magazineSize), _isAutomatic(isAutomatic), _fireRateMilliseconds(fireRateMilliseconds),
    _reloadTimeMilliseconds(reloadTimeMilliseconds) {

    ITEMS.push_back(this);
}

void Item::fireTargetedProjectile(const float velocity, Entity* parent, const ProjectileData projData, std::string soundName) {
    const unsigned int ammoId = ITEMS[parent->getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getAmmoId();
    if (parent->getHitPoints() > 0 && parent->getMagazineAmmoType() == ammoId && parent->getMagazineContents() > 0) {
        sf::Vector2f cBarrelPos = parent->getCalculatedBarrelPos();
        sf::Vector2f spawnPos(cBarrelPos.x, cBarrelPos.y);

        double x = (double)(parent->getTargetPos().x - cBarrelPos.x);
        double y = (double)(parent->getTargetPos().y - cBarrelPos.y);

        float angle = (float)((std::atan2(y, x)));

        std::shared_ptr<Projectile> proj = std::shared_ptr<Projectile>(new Projectile(
            spawnPos, parent, angle, velocity, projData
        ));
        proj->loadSprite(parent->getWorld()->getSpriteSheet());
        proj->setWorld(parent->getWorld());
        parent->getWorld()->addEntity(proj);

        parent->decrementMagazine();

        if (soundName != "NONE") SoundManager::playSound(soundName);
    }
}

unsigned int Item::getId() const {
    return _id;
}

std::string Item::getName() const {
    return _name;
}

std::string Item::getDescription() const {
    return _description;
}

int Item::getDamage() const {
    return _damage;
}

float Item::getHitBoxPos() const {
    return _hitBoxPos;
}

int Item::getHitBoxSize() const {
    return _hitBoxSize;
}

bool Item::isMeleeWeapon() const {
    return getHitBoxSize() > 0;
}

sf::Vector2f Item::getBarrelPos() const {
    return _barrelPos;
}

bool Item::isGun() const {
    return _isGun;
}

const int Item::getMagazineSize() const {
    return _magazineSize;
}

bool Item::isAutomatic() const {
    return _isAutomatic;
}

const unsigned int Item::getFireRateMilliseconds() const {
    return _fireRateMilliseconds;
}

const unsigned int Item::getReloadTimeMilliseconds() const {
    return _reloadTimeMilliseconds;
}

sf::IntRect Item::getTextureRect() const {
    return _textureRect;
}

bool Item::isStackable() const {
    return _isStackable;
}

/**
* FOR PROJECTILE-FIRING WEAPONS, THIS RETURNS AMMO ITEM ID
*/
unsigned int Item::getStackLimit() const {
    return _stackLimit;
}

unsigned int Item::getAmmoId() const {
    return getStackLimit();
}

bool Item::isConsumable() const {
    return _isConsumable;
}

/**
* For consumable items, this returns whether or not one of the item
* should be removed from the inventory upon use.
*/
bool Item::use(Entity* parent) const {
    return _use(parent);
}

EQUIPMENT_TYPE Item::getEquipmentType() const {
    return _equipType;
}