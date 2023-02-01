#ifndef _PATH_FINDER_H
#define _PATH_FINDER_H

#include <array>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <unordered_set>
#include <unordered_map>
#include "World.h"
constexpr int GRID_UNIT_SIZE = 16;

namespace std {
    /* implement hash function so we can put GridLocation into an unordered_set */
    template <> struct hash<sf::Vector2i> {
        std::size_t operator()(const sf::Vector2i& id) const noexcept {
            // NOTE: better to use something like boost hash_combine
            return std::hash<int>()(id.x ^ (id.y << 16));
        }
    };
}

struct SearchArea {
    static std::array<sf::Vector2i, 4> DIRS;

    sf::Vector2i pos;
    int width, height;
    std::unordered_set<sf::Vector2i> walls;

    SearchArea(sf::Vector2i pos_, int width_, int height_)
        : pos(pos_), width(width_), height(height_) {
    }

    bool in_bounds(sf::Vector2i id) const {
        return pos.x <= id.x && id.x < pos.x + width
            && pos.y <= id.y && id.y < pos.y + height;
    }

    bool passable(sf::Vector2i id) const {
        return walls.find(id) == walls.end();
    }

    std::vector<sf::Vector2i> neighbors(sf::Vector2i id) const {
        std::vector<sf::Vector2i> results;

        for (sf::Vector2i dir : DIRS) {
            sf::Vector2i next( id.x + dir.x, id.y + dir.y );
            if (in_bounds(next) && passable(next)) {
                results.push_back(next);
            }
        }

        //if ((id.x + id.y) % 2 == 0) {
        //     //see "Ugly paths" section for an explanation:
        //    std::reverse(results.begin(), results.end());
        //}

        return results;
    }
};

class PathFinder {
public:
    static std::unordered_map<sf::Vector2i, sf::Vector2i> breadthFirstSearch(SearchArea searchArea, sf::Vector2i start, sf::Vector2i goal);

    static std::vector<sf::Vector2i> reconstruct_path(
        sf::Vector2i start, sf::Vector2i goal,
        std::unordered_map<sf::Vector2i, sf::Vector2i> came_from
    );

    static SearchArea createSearchArea(sf::Vector2i pos, int size, Entity* source, World& world);

private:

};

#endif