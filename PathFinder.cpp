#include "PathFinder.h"
#include <queue>
#include <iostream>

std::array<sf::Vector2i, 4> SearchArea::DIRS = {
    /* East, West, North, South */
    sf::Vector2i(GRID_UNIT_SIZE, 0), sf::Vector2i(-GRID_UNIT_SIZE, 0),
    sf::Vector2i(0, -GRID_UNIT_SIZE), sf::Vector2i(0, GRID_UNIT_SIZE)
};

bool operator == (sf::Vector2i a, sf::Vector2i b) {
    return a.x == b.x && a.y == b.y;
}

bool operator != (sf::Vector2i a, sf::Vector2i b) {
    return !(a == b);
}

bool operator < (sf::Vector2i a, sf::Vector2i b) {
    return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}

std::unordered_map<sf::Vector2i, sf::Vector2i> 
PathFinder::breadthFirstSearch(SearchArea searchArea, sf::Vector2i start, sf::Vector2i goal) {
    std::queue<sf::Vector2i> frontier;
    frontier.push(start);

    std::unordered_map<sf::Vector2i, sf::Vector2i> came_from;
    came_from[start] = start;

    while (!frontier.empty()) {
        sf::Vector2i current = frontier.front();
        frontier.pop();

        if (sf::IntRect(current.x, current.y, GRID_UNIT_SIZE, GRID_UNIT_SIZE).contains(goal)) {
            break;
        }

        for (sf::Vector2i next : searchArea.neighbors(current)) {
            if (came_from.find(next) == came_from.end()) {
                frontier.push(next);
                came_from[next] = current;
            }
        }
    }
    return came_from;
}

std::vector<sf::Vector2i> PathFinder::reconstruct_path(sf::Vector2i start, sf::Vector2i goal, std::unordered_map<sf::Vector2i, sf::Vector2i> came_from) {
    std::vector<sf::Vector2i> path;
    sf::Vector2i current = goal;
    if (came_from.find(goal) == came_from.end()) {
        return path; // no path can be found
    }
    while (current != start) {
        path.push_back(current);
        current = came_from[current];
    }
    //path.push_back(start); // optional
    std::reverse(path.begin(), path.end());
    return path;
}

SearchArea PathFinder::createSearchArea(sf::Vector2i pos, int size, Entity* source, World& world) {
    SearchArea searchArea(pos, size, size);

    for (int y = pos.y - (GRID_UNIT_SIZE / 2); y < pos.y - (GRID_UNIT_SIZE / 2) + size; y += GRID_UNIT_SIZE) {
        for (int x = pos.x - (GRID_UNIT_SIZE / 2); x < pos.x - (GRID_UNIT_SIZE / 2) + size; x += GRID_UNIT_SIZE) {
            if (world.getTerrainDataAt(x, y) == TERRAIN_TYPE::WATER) {
                searchArea.walls.insert(sf::Vector2i(x, y));
            }
            /*for (auto& entity : world.getEntities()) {
                sf::FloatRect entityBounds = entity->getHitBox();
                sf::FloatRect tileBounds(x - GRID_UNIT_SIZE / 4, y - GRID_UNIT_SIZE / 4, GRID_UNIT_SIZE, GRID_UNIT_SIZE);
                if (entity->isMob() && entity->isActive() && !entity->compare(source) && entityBounds.intersects(tileBounds)) {
                    searchArea.walls.insert(sf::Vector2i(x, y));
                }
            }*/
        }
    } 

    return searchArea;
}
