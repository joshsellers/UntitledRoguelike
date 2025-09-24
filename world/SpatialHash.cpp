#include "SpatialHash.h"
#include "../core/MessageManager.h"

constexpr int CELL_SIZE = 64;

void SpatialHash::insert(std::shared_ptr<Entity> entity) {
    const sf::Vector2i cell = getCell(entity->getPosition());
    
    const int key = hash(cell.x, cell.y);
    _grid[key].push_back(entity);
}

std::vector<std::shared_ptr<Entity>> SpatialHash::queryNearby(sf::Vector2f pos, bool onlyEnemies) const {
    std::vector<std::shared_ptr<Entity>> result;

    const sf::Vector2i cell = getCell(pos);

    constexpr int neighborRange = 1;
    for (int dx = -neighborRange; dx <= neighborRange; dx++) {
        for (int dy = -neighborRange; dy <= neighborRange; dy++) {
            const int key = hash(cell.x + dx, cell.y + dy);
            if (_grid.count(key)) {
                for (const auto& entity : _grid.at(key)) {
                    if (entity->isEnemy() || !onlyEnemies) result.push_back(entity);
                }
            }
        }
    }

    return result;
}

void SpatialHash::clear() {
    _grid.clear();
}

sf::Vector2i SpatialHash::getCell(sf::Vector2f pos) const {
    const int cellX = std::floor(pos.x / CELL_SIZE);
    const int cellY = std::floor(pos.y / CELL_SIZE);
    return { cellX, cellY };
}

int SpatialHash::hash(int cellX, int cellY) const {
    const int p1 = 73856093;
    const int p2 = 19349663;
    return (cellX * p1) ^ (cellY * p2);
}

