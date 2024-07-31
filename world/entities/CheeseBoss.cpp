#include "CheeseBoss.h"
#include "../World.h"

CheeseBoss::CheeseBoss(sf::Vector2f pos) : Entity(CHEESE_BOSS, pos, 1, TILE_SIZE * 5, TILE_SIZE * 6, false) {
    setMaxHitPoints(500);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 5) / 2;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 5;
    _hitBox.height = TILE_SIZE * 6;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;
    _isMob = true;
    _isEnemy = true;

    _entityType = "cheeseboss";

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(10000, 12000);
    getInventory().addItem(Item::PENNY.getId(), pennyAmount);
}

void CheeseBoss::update() {
    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 5);

    float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
    float desiredDist = 100.f;
    float distanceRatio = desiredDist / dist;

    float xa = 0.f, ya = 0.f;

    sf::Vector2f goalPos((1.f - distanceRatio) * playerPos.x + distanceRatio * cLoc.x, (1.f - distanceRatio) * playerPos.y + distanceRatio * cLoc.y);
    if (dist > desiredDist) {
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

            if (xa > 0) _movingDir = RIGHT;
            else _movingDir = LEFT;
        }
    } else {
        // fix 
        if (std::abs(goalPos.y - cLoc.y) > std::abs(goalPos.x - cLoc.x)) {
            if (goalPos.y < cLoc.y) {
                _movingDir = DOWN;
            } else if (goalPos.y > cLoc.y) {
                _movingDir = UP;
            }
        } else {
            if (goalPos.x < cLoc.x) {
                _movingDir = RIGHT;
            } else if (goalPos.x > cLoc.x) {
                _movingDir = LEFT;
            }
        }
    }

    move(xa, ya); 
    
    _sprite.setPosition(getPosition());

    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE * 3;
        _hitBox.height = TILE_SIZE * 3;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 6;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    blink();
}

void CheeseBoss::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 6;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 3));

        int xOffset = ((_numSteps >> _animSpeed) & 1) * 16;

        _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, 16, 16));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - TILE_SIZE, getPosition().y + (TILE_SIZE * 2) / 2 + 9));
        surface.draw(_wavesSprite);
    }

    int xOffset = getMovingDir() * TILE_SIZE * 5;
    int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 7) * TILE_SIZE * 6 : 0;

    _sprite.setTextureRect(sf::IntRect(
        (_isBlinking ? 720 : 400) + xOffset, 736 + yOffset, TILE_SIZE * 5, isSwimming() ? TILE_SIZE * 3 : TILE_SIZE * 6
    ));

    surface.draw(_sprite);
}

void CheeseBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(400, 736, TILE_SIZE * 5, TILE_SIZE * 6));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE * 5 / 2, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}

void CheeseBoss::damage(int damage) {
}

void CheeseBoss::blink() {
    const long long blinkDuration = 150LL;
    const int blinkChance = 600;
    if (!_isBlinking) {
        _isBlinking = randomInt(0, blinkChance) == 0;
        if (_isBlinking) _blinkStartTime = currentTimeMillis();
    } else if (currentTimeMillis() - _blinkStartTime > blinkDuration) _isBlinking = false;
}
