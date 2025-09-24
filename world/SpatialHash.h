#ifndef _QUAD_H
#define _QUAD_H

#include <unordered_map>
#include <memory>
#include "entities/Entity.h"

class SpatialHash {
public:
    void insert(std::shared_ptr<Entity> entity);
    std::vector<std::shared_ptr<Entity>> queryNearby(sf::Vector2f pos) const;

    void entityMoved(std::shared_ptr<Entity> entity);
    void remove(std::shared_ptr<Entity> entity);

    void clear();
private:
    std::unordered_map<int, std::vector<std::shared_ptr<Entity>>> _grid;

    sf::Vector2i getCell(sf::Vector2f pos) const;
    int hash(int cellX, int cellY) const;
};

#endif