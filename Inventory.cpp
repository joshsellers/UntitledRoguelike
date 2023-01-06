#include "Inventory.h"
#include "Item.h"
#include "World.h"
#include <iostream>
#include "DroppedItem.h"

Inventory::Inventory(Entity* parent) : 
    _parent(parent) {
}

void Inventory::addItem(unsigned int itemId, unsigned int amount) {
    if (amount > 0) {
        for (auto& item : _inventory) {
            if (item.x == itemId && Item::ITEMS[itemId]->isStackable() && item.y + amount <= Item::ITEMS[itemId]->getStackLimit()) {
                item.y += amount;
                return;
            }
        }

        if (Item::ITEMS[itemId]->isStackable() && amount <= Item::ITEMS[itemId]->getStackLimit() && _inventory.size() < getMaxSize()) 
            _inventory.push_back(sf::Vector2u(itemId, amount));
        else if (Item::ITEMS[itemId]->isStackable() && amount > Item::ITEMS[itemId]->getStackLimit() && _inventory.size() < getMaxSize()) {
            _inventory.push_back(sf::Vector2u(itemId, Item::ITEMS[itemId]->getStackLimit()));
            addItem(itemId, amount - Item::ITEMS[itemId]->getStackLimit());
        } else if (!Item::ITEMS[itemId]->isStackable() && _inventory.size() < getMaxSize())
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

void Inventory::removeItemAt(unsigned int index, unsigned int amount) {
    if (amount > 0) {
        auto& item = _inventory.at(index);
        item.y -= amount;
        if (item.y <= 0) {
            _inventory.erase(_inventory.begin() + index);
        }
    }
}

void Inventory::dropItem(unsigned int itemId, unsigned int amount) {
    float px = _parent->getPosition().x;
    float py = _parent->getPosition().y;

    switch (_parent->getMovingDir()) {
    case UP:
        py -= 20;
        break;
    case DOWN:
        py += 37;
        break;
    case LEFT:
        px -= 20;
        py += 16;
        break;
    case RIGHT:
        px += 20;
        py += 16;
        break;
    }

    sf::Vector2f pos(px, py);
    std::shared_ptr<DroppedItem> droppedItem = std::shared_ptr<DroppedItem>(
        new DroppedItem(pos, 1.f, itemId, amount, Item::ITEMS[itemId]->getTextureRect())
    );
    droppedItem->setWorld(_parent->getWorld());
    droppedItem->loadSprite(_parent->getWorld()->getSpriteSheet());

    _parent->getWorld()->addEntity(droppedItem);
}

bool Inventory::hasItem(unsigned int itemId) const {
    for (auto& item : _inventory) 
        if (item.x == itemId) return true;

    return false;
}

void Inventory::useItem(size_t inventoryIndex) const {
    if (inventoryIndex < _inventory.size()) Item::ITEMS.at(_inventory.at(inventoryIndex).x)->use(_parent);
    else std::cout << "Invalid inventory index: " << inventoryIndex << ". Inventory size is " << _inventory.size() << std::endl;
}

unsigned int Inventory::getItemIdAt(unsigned int index) const {
    return _inventory.at(index).x;
}

unsigned int Inventory::getItemAmountAt(unsigned int index) const {
    return _inventory.at(index).y;
}

void Inventory::setMaxSize(unsigned int maxSize) {
    _maxSize = maxSize;
}

unsigned int Inventory::getMaxSize() const {
    return _maxSize;
}

unsigned int Inventory::getCurrentSize() const {
    return _inventory.size();
}

Entity* Inventory::getParent() const {
    return _parent;
}
