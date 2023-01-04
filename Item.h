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
    static std::vector<const Item*> ITEMS;

    Item(const unsigned int id, const std::string name, const sf::IntRect textureRect, const bool isStackable, const std::function<void(Entity*)> use);

    unsigned int getId() const;
    std::string getName() const;

    sf::IntRect getTextureRect() const;

    bool isStackable() const;

    void use(Entity* parent) const;

private:
    const unsigned int _id;
    const std::string _name;

    const sf::IntRect _textureRect;

    const bool _isStackable;

    const std::function<void(Entity*)> _use;
};

#endif