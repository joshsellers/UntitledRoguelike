#ifndef _INVENTORY_H
#define _INVENTORY_H

#include <vector>
#include <SFML/System/Vector2.hpp>

constexpr unsigned int DEFAULT_MAX_SIZE = 20;

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

    void setMaxSize(unsigned int maxSize);
    unsigned int getMaxSize() const;

    unsigned int getCurrentSize() const;

    Entity* getParent() const;

private:
    unsigned int _maxSize = DEFAULT_MAX_SIZE;
    // list of sf::Vector2u(itemId, amount)
    std::vector<sf::Vector2u> _inventory;

    Entity* _parent;
    // Inventory will probably have to take world as a parameter
    // Maybe to work around how complicated thats gonna be 
    // instead make inventory take a reference to the world's 
    // entity buffer which is slightly less problematic
    // might cause multithreading issues though
    World* _world;
};

#endif