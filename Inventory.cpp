#include "Inventory.h"
#include "Item.h"
#include "Entity.h"
#include <iostream>

Inventory::Inventory(Entity* parent) : 
    _parent(parent) {
}

void Inventory::addItem(unsigned int itemId, unsigned int amount) {
    if (amount > 0) {
        for (auto& item : _inventory) {
            if (item.x == itemId && Item::ITEMS[itemId]->isStackable()) {
                item.y += amount;
                return;
            }
        }

        _inventory.push_back(sf::Vector2u(itemId, amount));
    }
}

void Inventory::removeItem(unsigned int itemId, unsigned int amount) {
    if (amount > 0) {
        for (int i = 0; i < _inventory.size(); i++) {
            auto& item = _inventory.at(i);
            if (item.x == itemId) {
                item.y -= amount;
                if (item.y <= 0) {
                    _inventory.erase(_inventory.begin() + i);
                }
                return;
            }
        }
    }
}

bool Inventory::hasItem(unsigned int itemId) const {
    for (auto& item : _inventory) 
        if (item.x == itemId) return true;

    return false;
}

void Inventory::useItem(size_t inventoryIndex) const {
    if (inventoryIndex < _inventory.size()) Item::ITEMS.at(inventoryIndex)->use(_parent);
    else std::cout << "Invalid inventory index: " << inventoryIndex << ". Inventory size is " << _inventory.size() << std::endl;
}

void Inventory::setMaxSize(unsigned int maxSize) {
    _maxSize = maxSize;
}

unsigned int Inventory::getMaxSize() const {
    return _maxSize;
}

Entity* Inventory::getParent() const {
    return _parent;
}
