#include "PlantMan.h"
#include <boost/random/uniform_int_distribution.hpp>
#include "World.h"
#include "Util.h"
#include "PathFinder.h"

PlantMan::PlantMan(sf::Vector2f pos) :
    Entity(pos, 1, 1, 1, false) {
    _gen.seed(currentTimeNano());

    setMaxHitPoints(75);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE / 2;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE;
    _hitBox.height = TILE_SIZE * 3;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = true;
    _isMob = true;
}

void PlantMan::update() {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 3;

    

    for (auto& entity : getWorld()->getEntities()) {
        if (entity->isActive() && !entity->isMob() && !entity->compare(this) && entity->getHitBox().intersects(getHitBox())) {
            entity->damage(5);
            entity->knockBack(20.f, getMovingDir());
            break;
        }
    }


    _sprite.setPosition(getPosition());
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void PlantMan::draw(sf::RenderTexture& surface) {
    int xOffset = getMovingDir() * TILE_SIZE;
    int yOffset = isMoving() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * 3: 0;

    _sprite.setTextureRect(sf::IntRect(
        24 * TILE_SIZE + xOffset, 13 * TILE_SIZE + yOffset, TILE_SIZE, TILE_SIZE * 3
    ));

    surface.draw(_sprite);

    int dist = GRID_UNIT_SIZE * 79;
    sf::Vector2i goal((int)_world->getPlayer()->getPosition().x, (int)_world->getPlayer()->getPosition().y);
    sf::Vector2i cLoc(((int)getPosition().x) + PLAYER_WIDTH / 2, ((int)getPosition().y) + 48 / 2);
    SearchArea searchArea = PathFinder::createSearchArea(sf::Vector2i(cLoc.x - dist / 2, cLoc.y - dist / 2), dist, this, *getWorld());
    std::unordered_map<sf::Vector2i, sf::Vector2i> map = PathFinder::breadthFirstSearch(searchArea, cLoc, goal);

    for (auto& point : map) {
        if (sf::IntRect(point.first.x, point.first.y, GRID_UNIT_SIZE, GRID_UNIT_SIZE).contains(goal)) {
            goal = point.first;
            break;
        }
    }

    std::vector<sf::Vector2i> path = PathFinder::reconstruct_path(cLoc, goal, map);
    if (path.size()) {
        sf::Vector2i goalPos = path.at(0);
        int xa = 0, ya = 0;
        if (goalPos.y < cLoc.y) {
            ya--;
            _movingDir = UP;
        } else if (goalPos.y > cLoc.y) {
            ya++;
            _movingDir = DOWN;
        } else if (goalPos.x < cLoc.x) {
            xa--;
            _movingDir = LEFT;
        } else if (goalPos.x > cLoc.x) {
            xa++;
            _movingDir = RIGHT;
        }

        move(xa, ya);
    }

    for (auto& point : path) {
        sf::RectangleShape a;
        a.setPosition(point.x, point.y);
        a.setSize(sf::Vector2f(GRID_UNIT_SIZE, GRID_UNIT_SIZE));
        a.setFillColor(sf::Color(0xFF00FF99));
        surface.draw(a);
    }
}

void PlantMan::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}

void PlantMan::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(24 * TILE_SIZE, 13 * TILE_SIZE, TILE_SIZE, TILE_SIZE * 3));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE / 2, 0);
}
