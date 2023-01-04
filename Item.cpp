#include "Item.h"
#include "Entity.h"

const Item Item::TEST_ITEM(0, "testItem", sf::IntRect(0, 0, 16, 16), true,
    [](Entity* parent) {
        parent->move(0, -10);
    }
);

std::vector<const Item*> Item::ITEMS;

Item::Item(const unsigned int id, const std::string name, const sf::IntRect textureRect, const bool isStackable, 
    const std::function<void(Entity*)> use) :
    _id(id), _name(name), _textureRect(textureRect), _isStackable(isStackable), _use(use) {
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

void Item::use(Entity* parent) const {
    _use(parent);
}