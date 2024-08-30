#include "Item.h"
#include "../world/World.h"
#include <iostream>
#include "../world/entities/orbiters/Orbiter.h"
#include "../world/entities/projectiles/Projectile.h"
#include "../core/SoundManager.h"
#include "../world/entities/Dog.h"
#include "../core/Tutorial.h"
#include "../statistics/StatManager.h"
#include "abilities/AbilityManager.h"
#include "abilities/Ability.h"

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
    "A large, formidible axe\nGood for cutting down trees, but not much else",
    EQUIPMENT_TYPE::TOOL, 5, 18.f, 20, sf::Vector2f(), false, 50
);

const Item Item::DAGGER(6, "Dagger", sf::IntRect(18, 0, 1, 1), false, 0, false,
    "Careful! It's sharp",
    EQUIPMENT_TYPE::TOOL, 3, 12.f, 5, sf::Vector2f(), false, 20, false
);

const Item Item::BULLET_455(7, "Pistol Round", sf::IntRect(22, 3, 1, 1), true, 9999, false,
    "A centrefire black powder cartridge\nFor use with revolvers and\nother handguns",
    EQUIPMENT_TYPE::AMMO, 10, 0, 0, sf::Vector2f(), false, 50
);
const ProjectileData Item::DATA_B455(Item::BULLET_455.getId(), 5, sf::IntRect(6, 8, 4, 4), true, true);

const Item Item::HOWDAH(8, "Heavy Pistol", sf::IntRect(22, 0, 1, 1), false, BULLET_455.getId(), false,
    "A large handgun",
    EQUIPMENT_TYPE::TOOL, 0, 0, 0, sf::Vector2f(20, 6), true, 2000, true,
    [](Entity* parent) {
        fireTargetedProjectile(parent, DATA_B455, "revolver");
        return false;
    }, 8, false, 0, 1000
);

const Item Item::POD(9, "Pod", sf::IntRect(29, 3, 1, 1), true, 9999, false,
    "A large pod\nAmmunition for the Pod Launcher",
    EQUIPMENT_TYPE::AMMO, 50, 0, 0, sf::Vector2f(), false, 200, true
);
const ProjectileData Item::DATA_POD(Item::POD.getId(), 3, sf::IntRect(4, 8, 8, 8), true);

const Item Item::POD_LAUNCHER(10, "Pod Launcher", sf::IntRect(29, 0, 1, 1), false, POD.getId(), false,
    "Don't vape, kids",
    EQUIPMENT_TYPE::TOOL, 0, 0, 0, sf::Vector2f(30, 0), true, 100000, true,
    [](Entity* parent) {
        fireTargetedProjectile(parent, DATA_POD, "slip");
        return false;
    }, 1, false, 0, 3500
);

const Item Item::WIFE_BEATER(11, "Tank Top", sf::IntRect(12, 26, 1, 1), false, 0, false,
    "It's dirty",
    EQUIPMENT_TYPE::CLOTHING_BODY, 0, 0, 0, sf::Vector2f(), false, 80
);

const Item Item::JORTS(12, "Jorts", sf::IntRect(16, 26, 1, 1), false, 0, false,
    "Don't wear these",
    EQUIPMENT_TYPE::CLOTHING_LEGS, 0, 0, 0, sf::Vector2f(), false, 120
);

const Item Item::WHITE_TENNIS_SHOES(13, "White Tennis Shoes", sf::IntRect(20, 26, 1, 1), false, 0, false,
    "Some new kicks",
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
const ProjectileData Item::DATA_RIFLE_ROUND(Item::RIFLE_ROUND.getId(), 8, sf::IntRect(6, 8, 4, 4), true, true);

const Item Item::ASSAULT_RIFLE(19, "Assault Rifle", sf::IntRect(36, 0, 1, 1), false, RIFLE_ROUND.getId(), false,
    "Big scary shoots fast",
    EQUIPMENT_TYPE::TOOL, 0, 0, 0, sf::Vector2f(20, 3), true, 15000, true,
    [](Entity* parent) {
        fireTargetedProjectile(parent, DATA_RIFLE_ROUND, "ar");
        return false;
    }, 30, true, 150, 2000
);

const Item Item::PENNY(20, "Penny", sf::IntRect(3, 10, 1, 1), true, 1000000000, false,
    "Use these to buy stuff", 
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false
);

const Item Item::LIGHT_LASER_CHARGE(21, "Light Laser Charge", sf::IntRect(43, 3, 1, 1), true, 9999, false,
    "Ammo for a laser pistol", 
    EQUIPMENT_TYPE::AMMO, 8, 0, 0, sf::Vector2f(), false, 150
);

const Item Item::_PROJECTILE_LIGHT_LASER_CHARGE(22, "_LIGHT_LASER_PROJECTILE", sf::IntRect(43, 4, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 8, 0, 0, sf::Vector2f(), false
);
const ProjectileData Item::DATA_PROJECTILE_LIGHT_LASER_CHARGE(Item::_PROJECTILE_LIGHT_LASER_CHARGE.getId(), 10, sf::IntRect(6, 8, 4, 4), true, true);

const Item Item::LASER_PISTOL(23, "Laser Pistol", sf::IntRect(43, 0, 1, 1), false, LIGHT_LASER_CHARGE.getId(), false,
    "Pew Pew",
    EQUIPMENT_TYPE::TOOL, 4, 0, 0, sf::Vector2f(14, 4), true, 7000, true,
    [](Entity* parent) {
        fireTargetedProjectile(parent, DATA_PROJECTILE_LIGHT_LASER_CHARGE, "laser_pistol");
        return false;
    }, 12, false, 0, 200
);

const Item Item::PROPANE(24, "Propane", sf::IntRect(50, 3, 1, 1), true, 999999, false,
    "Clean burnin'\n\nFuel for blow torch",
    EQUIPMENT_TYPE::AMMO, 3, 0, 0, sf::Vector2f(), false, 2
);

const Item Item::_PROJECTILE_PROPANE(25, "_PROPANE_PROJECTILE", sf::IntRect(50, 4, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 3, 0, 0, sf::Vector2f(), false
);
const ProjectileData Item::DATA_PROJECTILE_PROPANE(Item::_PROJECTILE_PROPANE.getId(), 10, sf::IntRect(0, 0, 16, 16), true, true, 83, true, 3, 0);

const Item Item::BLOW_TORCH(26, "Blow Torch", sf::IntRect(50, 0, 1, 1), false, PROPANE.getId(), false,
    "It's a blow torch, but it\nseems like something's broken...",
    EQUIPMENT_TYPE::TOOL, 0, 0, 0, sf::Vector2f(12, 12), true, 10000, true,
    [](Entity* parent) {
        fireTargetedProjectile(parent, DATA_PROJECTILE_PROPANE, "blowtorch");
        return false;
    }, 500, true, 20, 2750
);

const Item Item::_PROJECTILE_SLIME_BALL(27, "_SLIMEBALL_PROJECTILE", sf::IntRect(7, 4, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 5, 0, 0, sf::Vector2f(), false);
const ProjectileData Item::DATA_PROJECTILE_SLIME_BALL(Item::_PROJECTILE_SLIME_BALL.getId(), 3, sf::IntRect(5, 5, 6, 6), false, true);

const Item Item::SLIME_BALL(28, "Slime Ball", sf::IntRect(6, 4, 1, 1), false, 0, true, 
    "A ball of slime that\nwill orbit around you and fire\npieces of itself at enemies", 
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 5, 0, 0, sf::Vector2f(), false, 25000, true, 
    [](Entity* parent) {
        std::shared_ptr<Orbiter> slimeBall = std::shared_ptr<Orbiter>(new Orbiter(90, OrbiterType::SLIME_BALL.getId(), parent));
        slimeBall->loadSprite(parent->getWorld()->getSpriteSheet());
        slimeBall->setWorld(parent->getWorld());
        parent->getWorld()->addEntity(slimeBall);

        if (!Tutorial::isCompleted()) Tutorial::completeStep(TUTORIAL_STEP::EQUIP_SLIMEBALL);
        return true;
    }
);

const Item Item::BANANA(29, "Banana", sf::IntRect(5, 10, 1, 1), true, 32, true,
    "Kinda mushy but it makes me feel good",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 65, true, 
    [](Entity* parent) {
        if (parent->getHitPoints() < parent->getMaxHitPoints()) {
            parent->heal(10);
            parent->restoreStamina(100);
            return true;
        }
        return false;
    }
);

const Item Item::BOWLING_BALL(30, "Bowling Ball", sf::IntRect(4, 10, 1, 1), false, 0, true,
    "Give it a toss and see where it lands",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 5, 0, 0, sf::Vector2f(), false, 2500, true, 
    [](Entity* parent) {
        std::shared_ptr<Orbiter> bowlingBall = std::shared_ptr<Orbiter>(new Orbiter(90, OrbiterType::BOWLING_BALL.getId(), parent));
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
    "Increases max HP by 25\nIncreases max stamina by 100\nIncreases damage multiplier by .25",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 20000, true,
    [](Entity* parent) {
        parent->setMaxHitPoints(parent->getMaxHitPoints() + 25);
        parent->setMaxStamina(parent->getMaxStamina() + 100);
        parent->increaseDamageMultiplier(0.25f);
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
    EQUIPMENT_TYPE::BOAT, 0, 0, 0, sf::Vector2f(), false, 7599
);

const Item Item::LIQUID_NAP(37, "Liquid Nap", sf::IntRect(6, 11, 1, 1), true, 10, true,
    "Makes you feel rested\nIncreases stamina restore rate by 5",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 15000, true,
    [](Entity* parent) {
        parent->increaseStaminaRefreshRate(5);
        return true;
    }
);

const Item Item::LOCUS_LIFT(38, "Multivitamin", sf::IntRect(7, 11, 1, 1), true, 10, true,
    "Increases damage multiplier juuust a little bit",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 599, true,
    [](Entity* parent) {
        parent->increaseDamageMultiplier(0.1f);
        return true;
    }
);

const Item Item::BONE(39, "Bone", sf::IntRect(5, 12, 1, 1), true, 99, true,
    "Give it to a dog and it'll be your friend",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 10, false,
    [](Entity* parent) {
        for (auto& entity : parent->getWorld()->getEntities()) {
            if (entity->isActive() && entity->getEntityType() == "dog") {
                Dog* dog = dynamic_cast<Dog*>(entity.get());
                if (dog->hasParent()) continue;

                sf::Vector2f playerPos((int)parent->getPosition().x + PLAYER_WIDTH / 2, (int)parent->getPosition().y + PLAYER_WIDTH * 2);
                sf::Vector2f cLoc(((int)dog->getPosition().x), ((int)dog->getPosition().y) + TILE_SIZE);

                float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
                if (dist < 100) {
                    dog->setParent(parent);
                    return true;
                }
            }
        }
        return false;
    }
);

const Item Item::COIN_MAGNET(40, "Magnet", sf::IntRect(6, 12, 1, 1), true, 999, true,
    "Bring me coin\nThe more you have, the more you'll pull\nEquip to activate",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 1025, true, 
    [](Entity* parent) {
        if (parent->getEntityType() == "player") {
            Player* player = dynamic_cast<Player*>(parent);
            if (player->getCoinMagnetCount() < 12) {
                player->addCoinMagnet();
            }
            return true;
        }
        return false;
    }
);

const Item Item::SCYTHE(41, "Scythe", sf::IntRect(57, 0, 1, 1), false, 0, false,
    "Swing for slicing",
    EQUIPMENT_TYPE::TOOL, 15, 24, 10, sf::Vector2f(), false, 5999
);

const Item Item::MATMURA_HELMET(42, "Matmura Helmet", sf::IntRect(13, 30, 1, 1), false, 0, false,
    "Mysterious armor\n15 protection",
    EQUIPMENT_TYPE::ARMOR_HEAD, 15, 0, 0, sf::Vector2f(), false, 10000, false
);

const Item Item::MATMURA_CHESTPLATE(43, "Matmura Chestplate", sf::IntRect(13, 43, 1, 1), false, 0, false,
    "Mysterious armor\n15 protection",
    EQUIPMENT_TYPE::ARMOR_BODY, 15, 0, 0, sf::Vector2f(), false, 9000, false
);

const Item Item::MATMURA_LEGGINGS(44, "Matmura Leggings", sf::IntRect(17, 43, 1, 1), false, 0, false,
    "Mysterious armor\n15 protection",
    EQUIPMENT_TYPE::ARMOR_LEGS, 15, 0, 0, sf::Vector2f(), false, 8500, false
);

const Item Item::MATMURA_BOOTS(45, "Matmura Boots", sf::IntRect(21, 43, 1, 1), false, 0, false,
    "Mysterious armor\n15 protection",
    EQUIPMENT_TYPE::ARMOR_FEET, 15, 0, 0, sf::Vector2f(), false, 7000, false
);

const Item Item::BROADSWORD(46, "Broadsword", sf::IntRect(57, 4, 1, 1), false, 0, false,
    "Chop chop",
    EQUIPMENT_TYPE::TOOL, 10, 40, 20, sf::Vector2f(), false, 7999
);

const Item Item::ENERGY_DRINK(47, "Energy Drink", sf::IntRect(112 >> SPRITE_SHEET_SHIFT, 192 >> SPRITE_SHEET_SHIFT, 1, 1), true, 32, true,
    "Increases stamina restore rate",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 3250, true,
    [](Entity* parent) {
        parent->increaseStaminaRefreshRate(1);
        return true;
    }
);

const Item Item::AUTOLASER(48, "Autolaser", sf::IntRect(61, 0, 1, 1), false, LIGHT_LASER_CHARGE.getId(), false,
    "Pewpew\n\nAn automatic laser rifle\nUses light laser charges",
    EQUIPMENT_TYPE::TOOL, 0, 0, 0, sf::Vector2f(22, 1), true, 30000, true,
    [](Entity* parent) {
        fireTargetedProjectile(parent, DATA_PROJECTILE_LIGHT_LASER_CHARGE, "laser_pistol");
        return false;
    }, 20, true, 225, 1200
);

const Item Item::RAILGUN_DART(49, "Railgun Dart", sf::IntRect(68, 3, 1, 1), true, 9999, false,
    "Warning: warm to touch after use",
    EQUIPMENT_TYPE::AMMO, 15, 0, 0, sf::Vector2f(), false, 10, true
);

const Item Item::_PROJECTILE_RAILGUN_DART(50, "_RAILGUN_DART_PROJECTILE", sf::IntRect(68, 4, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 15, 0, 0, sf::Vector2f(), false
);
const ProjectileData Item::DATA_PROJECTILE_RAILGUN_DART(Item::_PROJECTILE_RAILGUN_DART.getId(), 15, sf::IntRect(8, 8, 8, 8), true, true);

const Item Item::RAILGUN(51, "Railgun", sf::IntRect(68, 0, 1, 1), false, RAILGUN_DART.getId(), false,
    "Harness the power of electromagnets\nto solve your problems",
    EQUIPMENT_TYPE::TOOL, 1, 0, 0, sf::Vector2f(40, 1), true, 50000, true,
    [](Entity* parent) {
        constexpr int railgunPassThroughCount = 5;
        fireTargetedProjectile(parent, DATA_PROJECTILE_RAILGUN_DART, "railgun", railgunPassThroughCount);
        return false;
    }, 1, true, 0, 500
);

const Item Item::GASOLINE(52, "Gasoline", sf::IntRect(75, 3, 1, 1), true, 99999, false,
    "Liquid dinosaur bones\nFuel for chainsaw",
    EQUIPMENT_TYPE::AMMO, 2, 0, 0, sf::Vector2f(), false, 5, true
);

const Item Item::_PROJECITLE_CHAINSAW(53, "_CHAINSAW_PROJECTILE", sf::IntRect(75, 4, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 2, 0, 0, sf::Vector2f(), false
);
const ProjectileData Item::DATA_PROJECTILE_CHAINSAW(Item::_PROJECITLE_CHAINSAW.getId(), 0, sf::IntRect(0, 0, 16, 16), false, false, 100);

const Item Item::CHAINSAW(54, "Chainsaw", sf::IntRect(75, 0, 1, 1), false, GASOLINE.getId(), false,
    "What if I attatched a saw blade onto a rope and\nstrung it around some wheels",
    EQUIPMENT_TYPE::TOOL, 0, 0, 0, sf::Vector2f(30, 0), true, 19999, true,
    [](Entity* parent) {
        fireTargetedProjectile(parent, DATA_PROJECTILE_CHAINSAW, "chainsaw");
        return false;
    }, 300, true, 75, 5000
);

const Item Item::_PROJECTILE_CHEESE_SLICE(55, "_CHEESE_SLICE_PROJECTILE", sf::IntRect(2, 11, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 10, 0, 0, sf::Vector2f(), false
);

const ProjectileData Item::DATA_PROJECTILE_CHEESE_SLICE(Item::_PROJECTILE_CHEESE_SLICE.getId(), 3.5f, sf::IntRect(0, 0, 16, 16), true);

const Item Item::ARROW(56, "Arrow", sf::IntRect(1312 >> SPRITE_SHEET_SHIFT, 48 >> SPRITE_SHEET_SHIFT, 1, 1), true, 9999, false,
    "The shopkeep made this himself!\nNo warranty",
    EQUIPMENT_TYPE::AMMO, 5, 0, 0, sf::Vector2f(), false, 8
);

const ProjectileData Item::DATA_PROJECTILE_ARROW(Item::ARROW.getId(), 3.75f, sf::IntRect(4, 4, 12, 12), true, true, 1000LL, false, 0, 0, true);

const Item Item::BOW(57, "Bow", sf::IntRect(1312 >> SPRITE_SHEET_SHIFT, 0, 1, 1), false, ARROW.getId(), false,
    "Helps you get nice forearms",
    EQUIPMENT_TYPE::TOOL, 0, 0, 0, sf::Vector2f(6, 0), true, 100, true,
    [](Entity* parent) {
        fireTargetedProjectile(parent, DATA_PROJECTILE_ARROW);
        return false;
    }, 1, false, 0, 500
);

const Item Item::CHEESE_SLICE(58, "Cheese", sf::IntRect(3, 11, 1, 1), true, 16, true,
    "Nummy\nHeals 20 HP",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 500, false,
    [](Entity* parent) {
        if (parent->getHitPoints() < parent->getMaxHitPoints()) {
            parent->heal(20);
            return true;
        }
        return false;
    }
);

const Item Item::_PROJECTILE_TEAR_DROP(59, "_TEAR_DROP_PROJECTILE", sf::IntRect(2, 12, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 4, 0, 0, sf::Vector2f(), false
);

const ProjectileData Item::DATA_PROJECTILE_TEAR_DROP(Item::_PROJECTILE_TEAR_DROP.getId(), 2.5f, sf::IntRect(4, 4, 12, 12), true, true);

const Item Item::CYCLOPS_EYE(60, "Cyclops Eye", sf::IntRect(1, 12, 1, 1), false, 0, true,
    "Equip it and it will cry for you",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 5, 0, 0, sf::Vector2f(), false, 25000, false,
    [](Entity* parent) {
        std::shared_ptr<Orbiter> eye = std::shared_ptr<Orbiter>(new Orbiter(90, OrbiterType::EYE_BALL.getId(), parent));
        eye->loadSprite(parent->getWorld()->getSpriteSheet());
        eye->setWorld(parent->getWorld());
        parent->getWorld()->addEntity(eye);

        return true;
    }
);

const Item Item::LIQUID_EXERCISE(61, "Liquid Exercise", sf::IntRect(1, 11, 1, 1), true, 10, true,
    "Increases max stamina by 50",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 15000, true,
    [](Entity* parent) {
        parent->setMaxStamina(parent->getMaxStamina() + 50);
        return true;
    }
);

const Item Item::CACTUS_FLESH(62, "Cactus Flesh", sf::IntRect(0, 11, 1, 1), true, 9999, true,
    "Prickly pieces\n\nHeals 6 HP",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 35, false,
    [](Entity* parent) {
        if (parent->getHitPoints() < parent->getMaxHitPoints()) {
            parent->heal(6);
            return true;
        }
        return false;
    }
);

const Item Item::_PROJECTILE_BLOOD_BALL(63, "_BLOOD_BALL_PROJECTILE", sf::IntRect(5, 4, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 12, 0, 0, sf::Vector2f(), false
);

const ProjectileData Item::DATA_PROJECTILE_BLOOD_BALL(Item::_PROJECTILE_BLOOD_BALL.getId(), 4.25f, sf::IntRect(5, 5, 6, 6), false);

const Item Item::_PROJECTILE_LARGE_BLOOD_BALL(64, "_LARGE_BLOOD_BALL_PROJECTILE", sf::IntRect(4, 4, 1, 1), false, 0, false,
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 8, 0, 0, sf::Vector2f(), false
);

const ProjectileData Item::DATA_PROJECTILE_LARGE_BLOOD_BALL(Item::_PROJECTILE_LARGE_BLOOD_BALL.getId(), 4.0f, sf::IntRect(0, 0, 16, 16), false);

const Item Item::FINGER_NAIL(65, "Finger Nail", sf::IntRect(0, 12, 1, 1), true, 9999, false,
    "Smells weird\n\nSell it to the shopkeep for a pretty penny",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 10000, false
);

const Item Item::BAD_VIBES_POTION(66, "Potion of Bad Vibes", sf::IntRect(4, 37, 1, 1), true, 64, true,
    "Makes you emit bad vibes that hurt enemies\n\nIf you already have bad vibes, this will\nupgrade it",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 5000, true,
    [](Entity* parent) {
        const unsigned int abilityId = Ability::DAMAGE_AURA.getId();
        if (!AbilityManager::givePlayerAbility(abilityId)) {
            int parameterChoice = randomInt(0, 2);
            bool maxedExpRate = AbilityManager::getParameter(abilityId, "expansion rate") >= 4.75;
            if (parameterChoice == 2 && maxedExpRate) {
                parameterChoice = randomInt(0, 1);
            }
            
            if (parameterChoice == 1 && AbilityManager::getParameter(abilityId, "radius") >= 175) {
                parameterChoice = randomInt(0, 1);
                if (parameterChoice == 1 && !maxedExpRate) parameterChoice = 2;
                else if (maxedExpRate) {
                    parameterChoice = 0;
                }
            }

            const std::string keys[3] = {"damage", "radius", "expansion rate"};
            float increment = 0.f;

            if (parameterChoice == 0) {
                increment = 1;
            } else if (parameterChoice == 1) {
                increment = randomInt(5, 10);
            } else if (parameterChoice == 2) {
                increment = (float)randomInt(10, 50) / 100;
            }

            AbilityManager::setParameter(abilityId, keys[parameterChoice], AbilityManager::getParameter(abilityId, keys[parameterChoice]) + increment);

            MessageManager::displayMessage("Increased bad vibes " + keys[parameterChoice] + " by " + trimString(std::to_string(increment)), 5);
        }
        return true;
    }
);

const Item Item::SPIKE_BALL(67, "Spike Ball", sf::IntRect(5, 37, 1, 1), false, 0, true,
    "Concentrated impaling", 
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 0, 0, 0, sf::Vector2f(), false, 10000, true,
    [](Entity* parent) {
        std::shared_ptr<Orbiter> spikeBall = std::shared_ptr<Orbiter>(new Orbiter(90, OrbiterType::SPIKE_BALL.getId(), parent));
        spikeBall->loadSprite(parent->getWorld()->getSpriteSheet());
        spikeBall->setWorld(parent->getWorld());
        parent->getWorld()->addEntity(spikeBall);
        return true;
    }
);

const Item Item::_PROJECTILE_THORN(68, "_THORN_PROJECTILE", sf::IntRect(1, 13, 1, 1), false, 0, false, 
    "This item should not be obtainable",
    EQUIPMENT_TYPE::NOT_EQUIPABLE, 15, 0, 0, sf::Vector2f(), false
);

const ProjectileData Item::DATA_PROJECTILE_THORN(Item::_PROJECTILE_THORN.getId(), 3.f, sf::IntRect(0, 0, 16, 16), true);

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

    for (auto& item : Item::ITEMS) {
        if (item->getId() == id) MessageManager::displayMessage("Duplicate item ID: " + std::to_string(id) + "\nCulprits:\n" + item->getName() + "\n" + name, 5, WARN);
    }
    ITEMS.push_back(this);
}

void Item::fireTargetedProjectile(Entity* parent, const ProjectileData projData, std::string soundName, int passThroughCount) {
    const unsigned int ammoId = ITEMS[parent->getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getAmmoId();
    if (parent->getHitPoints() > 0 && parent->getMagazineAmmoType() == ammoId && parent->getMagazineContents() > 0) {
        sf::Vector2f cBarrelPos = parent->getCalculatedBarrelPos();
        sf::Vector2f spawnPos(cBarrelPos.x, cBarrelPos.y);

        double x = (double)(parent->getTargetPos().x - cBarrelPos.x);
        double y = (double)(parent->getTargetPos().y - cBarrelPos.y);

        float angle = (float)((std::atan2(y, x)));

        std::shared_ptr<Projectile> proj = std::shared_ptr<Projectile>(new Projectile(
            spawnPos, parent, angle, projData.baseVelocity, projData, false, ITEMS[parent->getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getDamage()
        ));
        proj->passThroughCount = passThroughCount;
        proj->loadSprite(parent->getWorld()->getSpriteSheet());
        proj->setWorld(parent->getWorld());
        parent->getWorld()->addEntity(proj);

        parent->decrementMagazine();

        if (soundName != "NONE") SoundManager::playSound(soundName);

        const unsigned int itemId = parent->getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL);
        if (parent->getSaveId() == PLAYER && itemId != CHAINSAW.getId() && itemId != BLOW_TORCH.getId()) {
            StatManager::increaseStat(SHOTS_FIRED, 1.f);
        }
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

/**
* For armor, this returns protection value, which ideally should be < 25 for any singular piece of armor
*/
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

const std::map<unsigned int, unsigned int> Item::ITEM_UNLOCK_WAVE_NUMBERS = {
    {Item::TOP_HAT.getId(),                         8},
    {Item::TUX_VEST.getId(),                        8},
    {Item::TUX_PANTS.getId(),                       8},
    {Item::DRESS_SHOES.getId(),                     8},
    {Item::SOMBRERO.getId(),                        3},
    {Item::AXE.getId(),                             0},
    {Item::DAGGER.getId(),                          0},
    {Item::BULLET_455.getId(),                      3},
    {Item::HOWDAH.getId(),                          3},
    {Item::POD.getId(),                             100},
    {Item::POD_LAUNCHER.getId(),                    100},
    {Item::WIFE_BEATER.getId(),                     2},
    {Item::JORTS.getId(),                           2},
    {Item::WHITE_TENNIS_SHOES.getId(),              2},
    {Item::APPLE.getId(),                           0},
    {Item::RED_TEE_SHIRT.getId(),                   6},
    {Item::OVERALLS.getId(),                        6},
    {Item::BOOTS.getId(),                           6},
    {Item::RIFLE_ROUND.getId(),                     16},
    {Item::ASSAULT_RIFLE.getId(),                   16},
    {Item::PENNY.getId(),                           0},
    {Item::LIGHT_LASER_CHARGE.getId(),              18},
    {Item::_PROJECTILE_LIGHT_LASER_CHARGE.getId(),  0},
    {Item::LASER_PISTOL.getId(),                    18},
    {Item::PROPANE.getId(),                         20},
    {Item::_PROJECTILE_PROPANE.getId(),             0},
    {Item::BLOW_TORCH.getId(),                      20},
    {Item::_PROJECTILE_SLIME_BALL.getId(),          0},
    {Item::SLIME_BALL.getId(),                      12},
    {Item::BANANA.getId(),                          0},
    {Item::BOWLING_BALL.getId(),                    10},
    {Item::WOOD.getId(),                            0},
    {Item::_PROJECTILE_SNOW_BALL.getId(),           0},
    {Item::STEROIDS.getId(),                        18},
    {Item::PROTEIN_SHAKE.getId(),                   8},
    {Item::BOTTLE_OF_MILK.getId(),                  5},
    {Item::WOOD_BOAT.getId(),                       10},
    {Item::LIQUID_NAP.getId(),                      6},
    {Item::LOCUS_LIFT.getId(),                      4},
    {Item::BONE.getId(),                            0},
    {Item::COIN_MAGNET.getId(),                     6},
    {Item::SCYTHE.getId(),                          10},
    {Item::MATMURA_HELMET.getId(),                  0},
    {Item::MATMURA_CHESTPLATE.getId(),              0},
    {Item::MATMURA_LEGGINGS.getId(),                0},
    {Item::MATMURA_BOOTS.getId(),                   0},
    {Item::BROADSWORD.getId(),                      8},
    {Item::ENERGY_DRINK.getId(),                    4},
    {Item::AUTOLASER.getId(),                       24},
    {Item::RAILGUN_DART.getId(),                    30},
    {Item::_PROJECTILE_RAILGUN_DART.getId(),        0},
    {Item::RAILGUN.getId(),                         30},
    {Item::GASOLINE.getId(),                        34},
    {Item::_PROJECITLE_CHAINSAW.getId(),            0},
    {Item::CHAINSAW.getId(),                        34},
    {Item::_PROJECTILE_CHEESE_SLICE.getId(),        0},
    {Item::ARROW.getId(),                           2},
    {Item::BOW.getId(),                             2},
    {Item::CHEESE_SLICE.getId(),                    0},
    {Item::_PROJECTILE_TEAR_DROP.getId(),           0},
    {Item::CYCLOPS_EYE.getId(),                     0},
    {Item::LIQUID_EXERCISE.getId(),                 7},
    {Item::CACTUS_FLESH.getId(),                    0},
    {Item::_PROJECTILE_BLOOD_BALL.getId(),          0},
    {Item::_PROJECTILE_LARGE_BLOOD_BALL.getId(),    0},
    {Item::FINGER_NAIL.getId(),                     0},
    {Item::BAD_VIBES_POTION.getId(),                11},
    {Item::SPIKE_BALL.getId(),                      14},
    {Item::_PROJECTILE_THORN.getId(),               0}
};

bool Item::isUnlocked(unsigned int waveNumber) const {
    return waveNumber >= ITEM_UNLOCK_WAVE_NUMBERS.at(getId()) && isBuyable();
}

unsigned int Item::getRequiredWave() const {
    return ITEM_UNLOCK_WAVE_NUMBERS.at(getId());
}

const std::map<unsigned int, WeaponAnimationConfig> Item::ANIMATION_CONFIGS = {
    {Item::CHAINSAW.getId(), {Item::CHAINSAW.getId(), 4, 2}}
};

bool Item::isAnimated() const {
    return ANIMATION_CONFIGS.find(getId()) != ANIMATION_CONFIGS.end();
}

WeaponAnimationConfig Item::getAnimationConfig() const {
    return ANIMATION_CONFIGS.at(getId());
}

void Item::checkForIncompleteItemConfigs() {
    for (const auto& item : ITEMS) {
        if (ITEM_UNLOCK_WAVE_NUMBERS.count(item->getId()) == 0) {
            MessageManager::displayMessage("Missing unlock wave number for " + item->getName(), 60, WARN);
        }
    }
}