#ifndef _INVENTORY_H
#define _INVENTORY_H

#include <SFML/System/Vector2.hpp>
#include "Item.h"

constexpr unsigned int DEFAULT_MAX_SIZE = 100;

constexpr int NOTHING_EQUIPPED = -1;
constexpr int EQUIPMENT_SLOT_COUNT = 10;

class World;
class Entity;

class Inventory {
public:
    Inventory(Entity* parent);

    void addItem(unsigned int itemId, unsigned int amount);
    void removeItem(unsigned int itemId, unsigned int amount);
    void removeItemAt(unsigned int index, unsigned int amount);

    void dropItem(unsigned int itemId, unsigned int amount);

    bool hasItem(unsigned int itemId) const;

    void useItem(size_t inventoryIndex) const;

    unsigned int getItemIdAt(unsigned int index) const;
    unsigned int getItemAmountAt(unsigned int index) const;

    void equip(int index, EQUIPMENT_TYPE equipType);
    void deEquip(EQUIPMENT_TYPE equipType);
    bool isEquipped(int index) const;
    int getEquippedItemId(EQUIPMENT_TYPE equipType) const;

    void setMaxSize(unsigned int maxSize);
    unsigned int getMaxSize() const;

    unsigned int getCurrentSize() const;

    Entity* getParent() const;

private:
    unsigned int _maxSize = DEFAULT_MAX_SIZE;
    // list of sf::Vector2u(itemId, amount)
    std::vector<sf::Vector2u> _inventory;
    
    int _equippedItems[EQUIPMENT_SLOT_COUNT] = {
        NOTHING_EQUIPPED, NOTHING_EQUIPPED, NOTHING_EQUIPPED, 
        NOTHING_EQUIPPED, NOTHING_EQUIPPED, NOTHING_EQUIPPED,
        NOTHING_EQUIPPED, NOTHING_EQUIPPED, NOTHING_EQUIPPED,
        NOTHING_EQUIPPED
    };

    Entity* _parent;
};

#endif