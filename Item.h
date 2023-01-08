#ifndef _ITEM_H
#define _ITEM_H

#include <string>
#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include <functional>
#include "EquipmentType.h"

class Entity;

class Item {
public:
    static const Item TEST_ITEM;
    static const Item TEST_ITEM_2;

    static const Item TEST_HAT;
    static const Item TEST_SHIRT;
    static const Item TEST_PANTS;
    static const Item TEST_SHOES;
    
    static const Item TEST_HELMET;
    static const Item TEST_BODYARMOR;
    static const Item TEST_LEGARMOR;
    static const Item TEST_FEETARMOR;

    static const Item TEST_AMMO;
    static const Item TEST_TOOL;

    static std::vector<const Item*> ITEMS;

    Item(const unsigned int id, const std::string name, const sf::IntRect textureRect, 
        const bool isStackable, const unsigned int stackLimit, const bool isConsumable, 
        const std::function<void(Entity*)> use, const std::string description, EQUIPMENT_TYPE equipType);

    unsigned int getId() const;
    std::string getName() const;
    std::string getDescription() const;

    sf::IntRect getTextureRect() const;

    bool isStackable() const;
    unsigned int getStackLimit() const;

    bool isConsumable() const;

    void use(Entity* parent) const;

    EQUIPMENT_TYPE getEquipmentType() const;

private:
    const unsigned int _id;
    const std::string _name;
    const std::string _description;

    const sf::IntRect _textureRect;

    const bool _isStackable;
    const unsigned int _stackLimit;

    const bool _isConsumable;

    const std::function<void(Entity*)> _use;

    const EQUIPMENT_TYPE _equipType;
};

#endif