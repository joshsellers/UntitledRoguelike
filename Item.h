#ifndef _ITEM_H
#define _ITEM_H

#include <SFML/Graphics.hpp>
#include <string>

class Item {
public:
    static const Item testItem;
    static std::vector<const Item*> ITEMS;

    Item(const unsigned int id, const std::string name, const sf::IntRect textureRect);

    unsigned int getId() const;
    std::string getName() const;

    sf::IntRect getTextureRect() const;

private:
    const unsigned int _id;
    const std::string _name;

    const sf::IntRect _textureRect;
};

#endif