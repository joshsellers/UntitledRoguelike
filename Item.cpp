#include "Item.h"
#include "World.h"
#include "Projectile.h"
#include <iostream>
#include "SoundManager.h"
#include "Orbiter.h"

const Item Item::TOP_HAT(0, "Top hat", sf::IntRect(0, 13, 1, 1), false, 0, false,
    "A fancy hat",
    EQUIPMENT_TYPE::CLOTHING_HEAD, 0, 0, 0, sf::Vector2f(), false, 210
); 

const Item Item::TUX_VEST(1, "Tuxedo Vest", sf::IntRect(0, 26, 1, 1), false, 0, false,
    "A tuxedo vest\nFor festive occasions",
    EQUIPMENT_TYPE::CLOTHING_BODY, 0, 0, 0, sf::Vector2f(), false, 210
);

const Item Item::TUX_PANTS(2, "Tuxedo Pants", sf::IntRect(4, 26, 1, 1), false, 0, false,
    "Tuxedo pants, for a tuxedo\nFor festive occasions",
    EQUIPMENT_TYPE::CLOTHING_LEGS, 0, 0, 0, sf::Vector2f(), false, 200
);

const Item Item::DRESS_SHOES(3, "Dress Shoes", sf::IntRect(8, 26, 1, 1), false, 0, false,
    "Some nice shoes\nFor festive occasions",
    EQUIPMENT_TYPE::CLOTHING_FEET, 0, 0, 0, sf::Vector2f(), false, 140
);

const Item Item::SOMBRERO(4, "Sombrero", sf::IntRect(12, 13, 1, 1), false, 0, false,
    "A nice hat to keep the\nsun out of your eyes",
    EQUIPMENT_TYPE::CLOTHING_HEAD, 0, 0, 0, sf::Vector2f(), false, 90
);

const Item Item::AXE(5, "Axe", sf::IntRect(18, 4, 1, 1), false, 0, false,
    "A large, formidible axe",
    EQUIPMENT_TYPE::TOOL, 5, 18.f, 20, sf::Vector2f(), false, 50
);

const Item Item::DAGGER(6, "Dagger", sf::IntRect(18, 0, 1, 1), false, 0, false,
    "Careful! It's sharp",
    EQUIPMENT_TYPE::TOOL, 3, 12.f, 5, sf::Vector2f(), false, 20
);

const Item Item::BULLET_455(7, "Pistol Round", sf::IntRect(22, 3, 1, 1), true, 9999, false,
    "A centrefire black powder cartridge\nFor use with revolvers and\nother handguns",
    EQUIPMENT_TYPE::AMMO, 10, 0, 0, sf::Vector2f(), false, 50
);
const ProjectileData Item::DATA_B455(Item::BULLET_455.getId(), 5, sf::IntRect(6, 8, 4, 4), true);

const Item Item::HOWDAH(8, "Howdah Pistol", sf::IntRect(22, 0, 1, 1), false, BULLET_455.getId(), false,
    "A large handgun",
    EQUIPMENT_TYPE::TOOL, 10, 0, 0, sf::Vector2f(20, 6), true, 2000, true,
    [](Entity* parent) {
        fireTargetedProjectile(DATA_B455.baseVelocity, parent, DATA_B455, "revolver");
        return false;
    }, 8, false, 0, 1000
);

const Item Item::POD(9, "Pod", sf::IntRect(29, 3, 1, 1), true, 9999, false,
    "A large pod\nAmmunition for the Pod Launcher",
    EQUIPMENT_TYPE::AMMO, 50, 0, 0, sf::Vector2f(), false, 200
);
const ProjectileData Item::DATA_POD(Item::POD.getId(), 3, sf::IntRect(4, 8, 8, 8), true);

const Item Item::POD_LAUNCHER(10, "Pod Launcher", sf::IntRect(29, 0, 1, 1), false, POD.getId(), false,
    "Don't vape, kids",
    EQUIPMENT_TYPE::TOOL, 10, 0, 0, sf::Vector2f(30, 0), true, 200000, true,
    [](Entity* parent) {
        fireTargetedProjectile(DATA_POD.baseVelocity, parent, DATA_POD, "slip");
        return false;
    }, 1, false, 0, 3500
);

const Item Item::WIFE_BEATER(11, "Wife Beater", sf::IntRect(12, 26, 1, 1), false, 0, false,
    "It's dirty",
    EQUIPMENT_TYPE::CLOTHING_BODY, 0, 0, 0, sf::Vector2f(), false, 80
);

const Item Item::JORTS(12, "Jorts", sf::IntRect(16, 26, 1, 1), false, 0, false,
    "Don't wear these",
    EQUIPMENT_TYPE::CLOTHING_LEGS, 0, 0, 0, sf::Vector2f(), false, 120
);

const Item Item::WHITE_TENNIS_SHOES(13, "White Tennis Shoes", sf::IntRect(20, 26, 1, 1), false, 0, false,
    "These help you go a little faster",
    EQUIPMENT_TYPE::CLOTHING_FEET, 0, 0, 0, sf::Vector2f(), false, 150
);

const Item Item::APPLE(14, "Apple", sf::IntRect(2, 10, 1, 1), true, 32, true, 
    "Something something an apple a day",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 40, true,
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
    EQUIPMENT_TYPE::CLOTHING_BODY, 0, 0, 0, sf::Vector2f(), false, 120
);

const Item Item::OVERALLS(16, "Overalls", sf::IntRect(28, 26, 1, 1), false, 0, false,
    "For when you're overall the bullshit",
    EQUIPMENT_TYPE::CLOTHING_LEGS, 0, 0, 0, sf::Vector2f(), false, 200
);

const Item Item::BOOTS(17, "Boots", sf::IntRect(32, 26, 1, 1), false, 0, false,
    "If I can't wear my cowboy boots, I ain't goin",
    EQUIPMENT_TYPE::CLOTHING_FEET, 0, 0, 0, sf::Vector2f(), false, 190
);

const Item Item::RIFLE_ROUND(18, "Rifle Round", sf::IntRect(36, 3, 1, 1), true, 9999, false,
    "Heavier than a pistol round\nFor use with rifles",
    EQUIPMENT_TYPE::AMMO, 6, 0, 0, sf::Vector2f(), false, 70
);
const ProjectileData Item::DATA_RIFLE_ROUND(Item::RIFLE_ROUND.getId(), 8, sf::IntRect(6, 8, 4, 4), true);

const Item Item::ASSAULT_RIFLE(19, "Assault Rifle", sf::IntRect(36, 0, 1, 1), false, RIFLE_ROUND.getId(), false,
    "Big scary shoots fast",
    EQUIPMENT_TYPE::TOOL, 6, 0, 0, sf::Vector2f(20, 3), true, 15000, true,
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
    EQUIPMENT_TYPE::AMMO, 12, 0, 0, sf::Vector2f(), false, 150
);

const Item Item::_PROJECTILE_LIGHT_LASER_CHARGE(22, "_LIGHT_LASER_PROJECTILE", sf::IntRect(43, 4, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 12, 0, 0, sf::Vector2f(), false
);
const ProjectileData Item::DATA_PROJECTILE_LIGHT_LASER_CHARGE(Item::_PROJECTILE_LIGHT_LASER_CHARGE.getId(), 10, sf::IntRect(6, 8, 4, 4), true);

const Item Item::LASER_PISTOL(23, "Laser Pistol", sf::IntRect(43, 0, 1, 1), false, LIGHT_LASER_CHARGE.getId(), false,
    "Pew Pew",
    EQUIPMENT_TYPE::TOOL, 12, 0, 0, sf::Vector2f(14, 4), true, 7000, true,
    [](Entity* parent) {
        fireTargetedProjectile(DATA_PROJECTILE_LIGHT_LASER_CHARGE.baseVelocity, parent, DATA_PROJECTILE_LIGHT_LASER_CHARGE, "laser_pistol");
        return false;
    }, 12, false, 0, 200
);

const Item Item::PROPANE(24, "Propane", sf::IntRect(50, 3, 1, 1), true, 999999, false,
    "\"Propane may be a clean burning fuel,\nbut she can also be a dirty girl.\"",
    EQUIPMENT_TYPE::AMMO, 3, 0, 0, sf::Vector2f(), false, 2
);

const Item Item::_PROJECTILE_PROPANE(25, "_PROPANE_PROJECTILE", sf::IntRect(50, 4, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 3, 0, 0, sf::Vector2f(), false
);
const ProjectileData Item::DATA_PROJECTILE_PROPANE(Item::_PROJECTILE_PROPANE.getId(), 10, sf::IntRect(0, 0, 16, 16), true, false, 5, true, 3, 0);

const Item Item::BLOW_TORCH(26, "Blow Torch", sf::IntRect(50, 0, 1, 1), false, PROPANE.getId(), false,
    "It's a blow torch, but it\nseems like something's broken...",
    EQUIPMENT_TYPE::TOOL, 3, 0, 0, sf::Vector2f(12, 12), true, 10000, true,
    [](Entity* parent) {
        fireTargetedProjectile(DATA_PROJECTILE_PROPANE.baseVelocity, parent, DATA_PROJECTILE_PROPANE, "NONE");
        if (SoundManager::getStatus("blowtorch") != sf::SoundSource::Playing) SoundManager::playSound("blowtorch");
        return false;
    }, 750, true, 20, 2750
);

const Item Item::_PROJECTILE_SLIME_BALL(27, "_SLIMEBALL_PROJECTILE", sf::IntRect(7, 4, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 5, 0, 0, sf::Vector2f(), false);
const ProjectileData Item::DATA_PROJECTILE_SLIME_BALL(Item::_PROJECTILE_SLIME_BALL.getId(), 3, sf::IntRect(5, 5, 6, 6), false, true);

const Item Item::SLIME_BALL(28, "Slime Ball", sf::IntRect(6, 4, 1, 1), false, 0, true, 
    "A ball of slime that\nwill orbit around you and fire\npieces of itself at enemies", 
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 5, 0, 0, sf::Vector2f(), false, 25000, true, 
    [](Entity* parent) {
        std::shared_ptr<Orbiter> slimeBall = std::shared_ptr<Orbiter>(new Orbiter(180, OrbiterType::SLIME_BALL.getId(), parent));
        slimeBall->loadSprite(parent->getWorld()->getSpriteSheet());
        slimeBall->setWorld(parent->getWorld());
        parent->getWorld()->addEntity(slimeBall);
        return true;
    }
);

const Item Item::BANANA(29, "Banana", sf::IntRect(5, 10, 1, 1), true, 32, true,
    "Kinda mushy but it makes me feel good",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 65, true, 
    [](Entity* parent) {
        if (parent->getHitPoints() < parent->getMaxHitPoints()) {
            parent->heal(10);
            return true;
        }
        return false;
    }
);

const Item Item::BOWLING_BALL(30, "Bowling Ball", sf::IntRect(4, 10, 1, 1), false, 0, true,
    "Give it a toss and see where it lands",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 5, 0, 0, sf::Vector2f(), false, 2500, true, 
    [](Entity* parent) {
        std::shared_ptr<Orbiter> bowlingBall = std::shared_ptr<Orbiter>(new Orbiter(180, OrbiterType::BOWLING_BALL.getId(), parent));
        bowlingBall->loadSprite(parent->getWorld()->getSpriteSheet());
        bowlingBall->setWorld(parent->getWorld());
        parent->getWorld()->addEntity(bowlingBall);
        return true;
    }
);

const Item Item::WOOD(31, "Wood", sf::IntRect(6, 10, 1, 1), true, 99, false, 
    "Sturdy\nSell it to the shop keep for some pennies",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0.f, 0, sf::Vector2f(), false, 35, false
);

const Item Item::_PROJECTILE_SNOW_BALL(32, "_SNOWBALL_PROJECTILE", sf::IntRect(32, 21, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 5, 0, 0, sf::Vector2f(), false);
const ProjectileData Item::DATA_PROJECTILE_SNOW_BALL(Item::_PROJECTILE_SNOW_BALL.getId(), 3, sf::IntRect(5, 5, 6, 6), false, false);

const Item Item::STEROIDS(33, "Steroids", sf::IntRect(114 >> SPRITE_SHEET_SHIFT, 161 >> SPRITE_SHEET_SHIFT, 1, 1), true, 5, true,
    "Increases max HP by 25",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 20000, true,
    [](Entity* parent) {
        parent->setMaxHitPoints(parent->getMaxHitPoints() + 25);
        return true;
    }
);

const Item Item::PROTEIN_SHAKE(34, "Protein Shake", sf::IntRect(4, 11, 1, 1), true, 32, true,
    "Increases max HP by 5 and makes you feel good",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 4750, true, 
    [](Entity* parent) {
        parent->setMaxHitPoints(parent->getMaxHitPoints() + 5);
        parent->heal(parent->getMaxHitPoints());
        return true;
    }
);

const Item Item::BOTTLE_OF_MILK(35, "Bottle of Milk", sf::IntRect(5, 11, 1, 1), true, 16, true,
    "Increases max HP by 2 and makes you feel a little better",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 1199, true,
    [](Entity* parent) {
        parent->setMaxHitPoints(parent->getMaxHitPoints() + 2);
        parent->heal(8);
        return true;
    }
);

const Item Item::WOOD_BOAT(36, "Boat", sf::IntRect(12, 31, 1, 1), false, 0, false, 
    "Helps you survive in the sea\nEquip it while in the water",
    EQUIPMENT_TYPE::BOAT, 0, 0, 0, sf::Vector2f(), false, 45000
);

std::vector<const Item*> Item::ITEMS;


Item::Item(const unsigned int id, const std::string name, const sf::IntRect textureRect, const bool isStackable,
    const unsigned int stackLimit, const bool isConsumable,
    std::string description, EQUIPMENT_TYPE equipType, const int damage, const float hitBoxPos,
    const int hitBoxSize, const sf::Vector2f barrelPos, const bool isGun, const int value, const bool isBuyable, const std::function<bool(Entity*)> use, const int magazineSize,
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
    _reloadTimeMilliseconds(reloadTimeMilliseconds), _value(value), _isBuyable(isBuyable) {

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

const int Item::getValue() const {
    return _value;
}

const bool Item::isBuyable() const {
    return _isBuyable;
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