#include "Frog.h"
#include "../../core/Util.h"
#include "../World.h"

Frog::Frog(sf::Vector2f pos) : Entity(FROG, pos, 1, 16, 16, false) {
    _gen.seed(currentTimeNano());

    setMaxHitPoints(15);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -TILE_SIZE / 2 + 3;
    _hitBoxYOffset = 4;
    _hitBox.width = 11;
    _hitBox.height = 10;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _isMob = true;

    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE;
    _wanderTargetPos = feetPos;

    _animSpeed = 3;
}

void Frog::update() {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE;

    constexpr float maxPlayerDistSquared = 40 * 40;
    const sf::Vector2f playerPos = getWorld()->getPlayer()->getPosition();
    int movementChance = 500;
    float playerDistSquared = std::pow((playerPos.x + PLAYER_WIDTH / 2) - getPosition().x, 2) + std::pow((playerPos.y + PLAYER_HEIGHT / 2) - getPosition().y, 2);
    if (playerDistSquared < maxPlayerDistSquared && currentTimeMillis() - _lastTimePlayerCameTooClose > 1000LL) {
        _wanderTargetPos = feetPos;
        movementChance = 0;
        _lastTimePlayerCameTooClose = currentTimeMillis();
    }

    wander(feetPos, _gen, movementChance, 32);

    _sprite.setPosition(getPosition());
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void Frog::draw(sf::RenderTexture& surface) {
    int xOffset = getMovingDir() * TILE_SIZE;
    int yOffset = isMoving() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE : 0;

    _sprite.setTextureRect(sf::IntRect(
        192 + xOffset, 128 + yOffset, TILE_SIZE, TILE_SIZE
    ));

    surface.draw(_sprite);
}

void Frog::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}

void Frog::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(192, 128, TILE_SIZE, TILE_SIZE));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE / 2, 0);
}
