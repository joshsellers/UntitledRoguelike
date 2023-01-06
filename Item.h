#ifndef _ITEM_H
#define _ITEM_H

#include <string>
#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include <functional>

class Entity;

class Item {
public:
    static const Item TEST_ITEM;
    static const Item TEST_ITEM_2;

    static std::vector<const Item*> ITEMS;

    Item(const unsigned int id, const std::string name, const sf::IntRect textureRect, 
        const bool isStackable, const unsigned int stackLimit, const bool isConsumable, 
        const std::function<void(Entity*)> use, const std::string description);

    unsigned int getId() const;
    std::string getName() const;
    std::string getDescription() const;

    sf::IntRect getTextureRect() const;

    bool isStackable() const;
    unsigned int getStackLimit() const;

    bool isConsumable() const;

    void use(Entity* parent) const;

private:
    const unsigned int _id;
    const std::string _name;
    const std::string _description;

    const sf::IntRect _textureRect;

    const bool _isStackable;
    const unsigned int _stackLimit;

    const bool _isConsumable;

    const std::function<void(Entity*)> _use;
};

#endif