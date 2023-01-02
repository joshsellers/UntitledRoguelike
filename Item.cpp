#include "Item.h"

const Item Item::testItem(0, "testItem", sf::IntRect(0, 0, 16, 16));

std::vector<const Item*> Item::ITEMS;

Item::Item(const unsigned int id, const std::string name, const sf::IntRect textureRect) : 
    _id(id), _name(name), _textureRect(textureRect) {
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
