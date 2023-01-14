#include "Item.h"
#include "World.h"
#include "Projectile.h"
#include <iostream>

const Item Item::TOP_HAT(0, "Top hat", sf::IntRect(0, 13, 1, 1), false, 0, false,
    "A fancy hat",
    EQUIPMENT_TYPE::CLOTHING_HEAD, 0, 0, 0, sf::Vector2f(),
    [](Entity* parent) {}
); 

const Item Item::TUX_VEST(1, "Tuxedo Vest", sf::IntRect(0, 26, 1, 1), false, 0, false,
    "A tuxedo vest\nFor festive occasions",
    EQUIPMENT_TYPE::CLOTHING_BODY, 0, 0, 0, sf::Vector2f(),
    [](Entity* parent) {}
);

const Item Item::TUX_PANTS(2, "Tuxedo Pants", sf::IntRect(4, 26, 1, 1), false, 0, false,
    "Tuxedo pants, for a tuxedo\nFor festive occasions",
    EQUIPMENT_TYPE::CLOTHING_LEGS, 0, 0, 0, sf::Vector2f(),
    [](Entity* parent) {}
);

const Item Item::DRESS_SHOES(3, "Dress Shoes", sf::IntRect(8, 26, 1, 1), false, 0, false,
    "Some nice shoes\nFor festive occasions",
    EQUIPMENT_TYPE::CLOTHING_FEET, 0, 0, 0, sf::Vector2f(),
    [](Entity* parent) {}
);

const Item Item::SOMBRERO(4, "Sombrero", sf::IntRect(12, 13, 1, 1), false, 0, false,
    "A nice hat to keep the\nsun out of your eyes",
    EQUIPMENT_TYPE::CLOTHING_HEAD, 0, 0, 0, sf::Vector2f(),
    [](Entity* parent) {}
);

const Item Item::AXE(5, "Axe", sf::IntRect(14, 0, 1, 1), false, 0, false,
    "A large, formidible axe",
    EQUIPMENT_TYPE::TOOL, 5, 18.f, 20, sf::Vector2f(),
    [](Entity* parent) {}
);

const Item Item::DAGGER(6, "Dagger", sf::IntRect(18, 0, 1, 1), false, 0, false,
    "Careful! It's sharp",
    EQUIPMENT_TYPE::TOOL, 3, 12.f, 5, sf::Vector2f(),
    [](Entity* parent) {}
);

const Item Item::TEST_AMMO(7, "test ammo", sf::IntRect(22, 3, 1, 1), true, 8, false,
    "bang boom",
    EQUIPMENT_TYPE::AMMO, 3, 0, 0, sf::Vector2f(),
    [](Entity* parent) {}
);

const Item Item::HOWDAH(8, "Howdah Pistol", sf::IntRect(22, 0, 1, 1), false, 0, false,
    "A large pistol",
    EQUIPMENT_TYPE::TOOL, 10, 0, 0, sf::Vector2f(20, 6),
    [](Entity* parent) {
        sf::Vector2f cBarrelPos = parent->getCalculatedBarrelPos();
        sf::Vector2f spawnPos(cBarrelPos.x, cBarrelPos.y);

        double x = (double)(parent->getTargetPos().x - cBarrelPos.x);
        double y = (double)(parent->getTargetPos().y - cBarrelPos.y);

        float angle = (float)((std::atan2(y, x)));

        std::shared_ptr<Projectile> proj = std::shared_ptr<Projectile>(new Projectile(spawnPos, angle, 2, TEST_AMMO.getId()));
        proj->loadSprite(parent->getWorld()->getSpriteSheet());
        parent->getWorld()->addEntity(proj);
    }
);

std::vector<const Item*> Item::ITEMS;

Item::Item(const unsigned int id, const std::string name, const sf::IntRect textureRect, const bool isStackable, 
    const unsigned int stackLimit, const bool isConsumable,
    std::string description, EQUIPMENT_TYPE equipType, const int damage, const float hitBoxPos,
    const int hitBoxSize, sf::Vector2f barrelPos, const std::function<void(Entity*)> use) :
    _id(id), _name(name), _textureRect(
        sf::IntRect(
            textureRect.left << SPRITE_SHEET_SHIFT, 
            textureRect.top << SPRITE_SHEET_SHIFT, 
            textureRect.width << SPRITE_SHEET_SHIFT,
            textureRect.height << SPRITE_SHEET_SHIFT
        )), 
    _isStackable(isStackable), _stackLimit(stackLimit), 
    _isConsumable(isConsumable), _use(use), _description(description), 
    _equipType(equipType), _damage(damage), _hitBoxSize(hitBoxSize), _hitBoxPos(hitBoxPos), _barrelPos(barrelPos) {

    ITEMS.push_back(this);
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

sf::IntRect Item::getTextureRect() const {
    return _textureRect;
}

bool Item::isStackable() const {
    return _isStackable;
}

unsigned int Item::getStackLimit() const {
    return _stackLimit;
}

bool Item::isConsumable() const {
    return _isConsumable;
}

void Item::use(Entity* parent) const {
    _use(parent);
}

EQUIPMENT_TYPE Item::getEquipmentType() const {
    return _equipType;
}