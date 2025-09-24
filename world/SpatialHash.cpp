#include "SpatialHash.h"
#include "../core/MessageManager.h"

constexpr int CELL_SIZE = 64;

void SpatialHash::insert(std::shared_ptr<Entity> entity) {
    const sf::Vector2i cell = getCell(entity->getPosition());
    
    const int key = hash(cell.x, cell.y);
    _grid[key].push_back(entity);
}

std::vector<std::shared_ptr<Entity>> SpatialHash::queryNearby(sf::Vector2f pos) const {
    std::vector<std::shared_ptr<Entity>> result;

    const sf::Vector2i cell = getCell(pos);

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            const int key = hash(cell.x + dx, cell.y + dy);
            if (_grid.count(key)) {
                for (const auto& entity : _grid.at(key)) {
                    result.push_back(entity);
                }
            }
        }
    }

    return result;
}

void SpatialHash::entityMoved(std::shared_ptr<Entity> entity) {
    remove(entity);
    insert(entity);
}

void SpatialHash::remove(std::shared_ptr<Entity> entity) {
    const sf::Vector2i cell = getCell(entity->getPosition());
    const int key = hash(cell.x, cell.y);

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            const int key = hash(cell.x + dx, cell.y + dy);
            if (_grid.count(key)) {
                for (int i = 0; i < _grid[key].size(); i++) {
                    const auto& storedEntity = _grid[key].at(i);
                    if (entity->getUID() == storedEntity->getUID()) {
                        _grid[key].erase(_grid[key].begin() + i);
                        break;
                    }
                }
            }
        }
    }
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

