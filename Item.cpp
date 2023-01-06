#include "Item.h"
#include "Entity.h"

const Item Item::TEST_ITEM(0, "testItem", sf::IntRect(0, 0, 1, 1), false, 0, false,
    [](Entity* parent) {
        
    }
);
const Item Item::TEST_ITEM_2(1, "testItem2", sf::IntRect(2, 9, 1, 1), true, 5, true,
    [](Entity* parent) {
        parent->setBaseSpeed(parent->getBaseSpeed() + 0.5);
    }
);

std::vector<const Item*> Item::ITEMS;

Item::Item(const unsigned int id, const std::string name, const sf::IntRect textureRect, const bool isStackable, 
    const unsigned int stackLimit, const bool isConsumable,
    const std::function<void(Entity*)> use) :
    _id(id), _name(name), _textureRect(
        sf::IntRect(
            textureRect.left << SPRITE_SHEET_SHIFT, 
            textureRect.top << SPRITE_SHEET_SHIFT, 
            textureRect.width << SPRITE_SHEET_SHIFT,
            textureRect.height << SPRITE_SHEET_SHIFT
        )), 
    _isStackable(isStackable), _stackLimit(stackLimit), _isConsumable(isConsumable), _use(use) {

    ITEMS.push_back(this);
}

unsigned int Item::getId() const {
    return _id;
}

std::string Item::getName() const {
    return _name;
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