#ifndef _INVENTORY_H
#define _INVENTORY_H

#include <vector>
#include <SFML/System/Vector2.hpp>

constexpr unsigned int DEFAULT_MAX_SIZE = 20;

class Entity;

class Inventory {
public:
    Inventory(Entity* parent);

    void addItem(unsigned int itemId, unsigned int amount);
    void removeItem(unsigned int itemId, unsigned int amount);
    void removeItemAt(unsigned int index, unsigned int amount);

    bool hasItem(unsigned int itemId) const;

    void useItem(size_t inventoryIndex) const;

    unsigned int getItemIdAt(unsigned int index) const;
    unsigned int getItemAmountAt(unsigned int index) const;

    void setMaxSize(unsigned int maxSize);
    unsigned int getMaxSize() const;

    unsigned int getCurrentSize() const;

    Entity* getParent() const;

private:
    unsigned int _maxSize = DEFAULT_MAX_SIZE;
    // list of sf::Vector2u(itemId, amount)
    std::vector<sf::Vector2u> _inventory;

    Entity* _parent;
};

#endif