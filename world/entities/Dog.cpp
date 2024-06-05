#include "Dog.h"
#include "../World.h"

Dog::Dog(sf::Vector2f pos) : Entity(DOG, pos, 1, TILE_SIZE * 2, TILE_SIZE * 2, false) {
    _gen.seed(currentTimeNano());

    setMaxHitPoints(25);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE + 6;
    _hitBoxYOffset = 8;
    _hitBox.width = 22;
    _hitBox.height = 27;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;
    _isMob = true;

    _entityType = "dog";
}

void Dog::update() {
    if (!_hasOwner) {
        sf::Vector2f feetPos = getPosition();
        feetPos.y += TILE_SIZE * 2;

        wander(feetPos, _gen, 100);
    } else {
        sf::Vector2f playerPos((int)_parent->getPosition().x + PLAYER_WIDTH / 2, (int)_parent->getPosition().y + PLAYER_WIDTH);
        sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE);

        float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
        float desiredDist = 100.f;
        float distanceRatio = desiredDist / dist;

        float xa = 0.f, ya = 0.f;

        if (dist > desiredDist) {
            sf::Vector2f goalPos((1.f - distanceRatio) * playerPos.x + distanceRatio * cLoc.x, (1.f - distanceRatio) * playerPos.y + distanceRatio * cLoc.y);

            if (goalPos.y < cLoc.y) {
                ya--;
                _movingDir = UP;
            } else if (goalPos.y > cLoc.y) {
                ya++;
                _movingDir = DOWN;
            }

            if (goalPos.x < cLoc.x) {
                xa--;
                _movingDir = LEFT;
            } else if (goalPos.x > cLoc.x) {
                xa++;
                _movingDir = RIGHT;
            }

            if (xa && ya) {
                xa *= 0.785398;
                ya *= 0.785398;
            }
        }

        move(xa, ya);
    }

    _sprite.setPosition(getPosition());
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void Dog::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 2;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2));

        int xOffset = ((_numSteps >> _animSpeed) & 1) * 16;

        _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, 16, 16));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - TILE_SIZE, getPosition().y + (TILE_SIZE * 2) / 2 + 9));
        surface.draw(_wavesSprite);
    }

    int xOffset = getMovingDir() * TILE_SIZE * 2;
    int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * 2 : 0;

    _sprite.setTextureRect(sf::IntRect(
        32 * TILE_SIZE + xOffset, 5 * TILE_SIZE + yOffset, TILE_SIZE * 2, isSwimming() ? TILE_SIZE * 2 / 2 : TILE_SIZE * 2
    ));

    surface.draw(_sprite);
}

void Dog::setParent(Entity* parent) {
    _parent = parent;
    _hasOwner = true;
}

Entity* Dog::getParent() const {
    return _parent;
}

void Dog::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(32 * TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE * 2, TILE_SIZE * 2));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}

std::string Dog::getSaveData() const {
    return (_hasOwner ? "1" : "0") + 
        (std::string)":" + (_hasOwner ? _parent->getUID() : "NONE");
}

void Dog::invokeFunction(std::string functionName, std::string args) {
    if (functionName == "addOwner") {
        for (auto& entity : getWorld()->getEntities()) {
            if (entity->isActive() && entity->getUID() == args) {
                setParent(entity.get());
                return;
            }
        }
    }
}

void Dog::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}
