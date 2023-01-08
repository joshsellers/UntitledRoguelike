#include "Item.h"
#include "Entity.h"

const Item Item::TEST_ITEM(0, "testItem", sf::IntRect(0, 0, 1, 1), false, 0, false,
    [](Entity* parent) {
        
    },
    "An amazing test item.\nIt doesn't do anything.",
    EQUIPMENT_TYPE::NOT_EQUIPABLE
);
const Item Item::TEST_ITEM_2(1, "testItem2", sf::IntRect(2, 9, 1, 1), true, 5, true,
    [](Entity* parent) {
        parent->setBaseSpeed(parent->getBaseSpeed() + 0.5);
    },
    "Another amazing test item.\nIncreases base speed by 0.5 when consumed.\ntest\nest\nest",
    EQUIPMENT_TYPE::NOT_EQUIPABLE
);

const Item Item::TEST_HAT(2, "testHat", sf::IntRect(1, 1, 1, 1), false, 0, false, [](Entity* parent) {},
    "A test hat",
    EQUIPMENT_TYPE::CLOTHING_HEAD
);
const Item Item::TEST_SHIRT(3, "testShirt", sf::IntRect(2, 1, 1, 1), false, 0, false, [](Entity* parent) {},
    "A test shirt",
    EQUIPMENT_TYPE::CLOTHING_BODY
);
const Item Item::TEST_PANTS(4, "testPants", sf::IntRect(3, 1, 1, 1), false, 0, false, [](Entity* parent) {},
    "A test pant",
    EQUIPMENT_TYPE::CLOTHING_LEGS
);
const Item Item::TEST_SHOES(5, "testShoes", sf::IntRect(4, 1, 1, 1), false, 0, false, [](Entity* parent) {},
    "A test shoes",
    EQUIPMENT_TYPE::CLOTHING_FEET
);

const Item Item::TEST_HELMET(6, "testHelmet", sf::IntRect(5, 1, 1, 1), false, 0, false, [](Entity* parent) {},
    "A test helmet",
    EQUIPMENT_TYPE::ARMOR_HEAD
);
const Item Item::TEST_BODYARMOR(7, "testBodyarmor", sf::IntRect(6, 1, 1, 1), false, 0, false, [](Entity* parent) {},
    "A test bodyarmor",
    EQUIPMENT_TYPE::ARMOR_BODY
);
const Item Item::TEST_LEGARMOR(8, "testLegarmor", sf::IntRect(7, 1, 1, 1), false, 0, false, [](Entity* parent) {},
    "A test legarmor",
    EQUIPMENT_TYPE::ARMOR_LEGS
);
const Item Item::TEST_FEETARMOR(9, "testFeetarmor", sf::IntRect(8, 1, 1, 1), false, 0, false, [](Entity* parent) {},
    "A test feetarmor",
    EQUIPMENT_TYPE::ARMOR_FEET
);

const Item Item::TEST_AMMO(10, "testAmmo", sf::IntRect(9, 1, 1, 1), true, 16, false, [](Entity* parent) {},
    "A test ammo",
    EQUIPMENT_TYPE::AMMO
);
const Item Item::TEST_TOOL(11, "testTool", sf::IntRect(10, 1, 1, 1), false, 0, false, [](Entity* parent) {},
    "A test tool",
    EQUIPMENT_TYPE::TOOL
);

std::vector<const Item*> Item::ITEMS;

Item::Item(const unsigned int id, const std::string name, const sf::IntRect textureRect, const bool isStackable, 
    const unsigned int stackLimit, const bool isConsumable,
    const std::function<void(Entity*)> use, std::string description, EQUIPMENT_TYPE equipType) :
    _id(id), _name(name), _textureRect(
        sf::IntRect(
            textureRect.left << SPRITE_SHEET_SHIFT, 
            textureRect.top << SPRITE_SHEET_SHIFT, 
            textureRect.width << SPRITE_SHEET_SHIFT,
            textureRect.height << SPRITE_SHEET_SHIFT
        )), 
    _isStackable(isStackable), _stackLimit(stackLimit), 
    _isConsumable(isConsumable), _use(use), _description(description), _equipType(equipType) {

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