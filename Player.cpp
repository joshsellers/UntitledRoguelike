#include "Player.h"
#include "World.h"
#include <iostream>
#include "Item.h"

Player::Player(sf::Vector2f pos) : 
    Entity(pos, BASE_PLAYER_SPEED, PLAYER_WIDTH / TILE_SIZE, PLAYER_HEIGHT / TILE_SIZE, false) {
    _canPickUpItems = true;
}

void Player::update() {
    float xa = 0, ya = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        ya = -getBaseSpeed();
        _movingDir = UP;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        ya = getBaseSpeed();
        _movingDir = DOWN;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        xa = -getBaseSpeed();
        _movingDir = LEFT;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        xa = getBaseSpeed();
        _movingDir = RIGHT;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && !isDodging() && (!isSwimming() || NO_MOVEMENT_RESTRICIONS || freeMove)) {
        xa *= _sprintMultiplier;
        ya *= _sprintMultiplier;
        _animSpeed = 2;
    } else if (!isSwimming() && isMoving()) {
        _animSpeed = 3;
    } else if (isSwimming() && !(NO_MOVEMENT_RESTRICIONS || freeMove)) {
        _animSpeed = 4;
        xa /= 2;
        ya /= 2;
    }

    if ((!isSwimming() || NO_MOVEMENT_RESTRICIONS || freeMove) && !isDodging() && sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && _dodgeKeyReleased) {
        _isDodging = true;
        _dodgeTimer++;
    } else if (isDodging() && _dodgeTimer < _maxDodgeTime) {
        _dodgeSpeedMultiplier = _dodgeMultiplier(_dodgeTimer) * _sprintMultiplier;
        _dodgeTimer++;
    } else if (isDodging() && _dodgeTimer >= _maxDodgeTime) {
        _isDodging = false;
        _dodgeTimer = 0;
        _dodgeSpeedMultiplier = 1.f;
    }

    _dodgeKeyReleased = !sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

    move(xa * _dodgeSpeedMultiplier, ya * _dodgeSpeedMultiplier);

    _sprite.setPosition(getPosition());
}

void Player::draw(sf::RenderTexture& surface) {
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        _world->getCurrentChunk(), sf::Vector2f(((int)_pos.x + PLAYER_WIDTH / 2), ((int)_pos.y + PLAYER_HEIGHT))
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2));

        int xOffset = ((_numSteps >> _animSpeed) & 1) * 16;

        _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, 16, 16));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2 + 8));
        surface.draw(_wavesSprite);
    }

    int xOffset = isDodging() ? ((_numSteps >> (_animSpeed / 2)) & 3) * 16 : 0;
    int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * 32 : 0;
    _sprite.setTextureRect(sf::IntRect(
        isDodging() && isMoving() ? xOffset : 16 * _movingDir, 
        isDodging() && isMoving() ? 128 : 0 + yOffset, 
        16, 
        terrainType == TERRAIN_TYPE::WATER ? 16 : 32)
    );
    surface.draw(_sprite);

    if (!isDodging() || !isMoving()) drawEquipables(surface);
}

void Player::drawEquipables(sf::RenderTexture& surface) {
    drawApparel(_clothingHeadSprite, EQUIPMENT_TYPE::CLOTHING_HEAD, surface);
    drawApparel(_armorBodySprite, EQUIPMENT_TYPE::ARMOR_HEAD, surface);

    if (!isSwimming()) {
        drawApparel(_clothingBodySprite, EQUIPMENT_TYPE::CLOTHING_BODY, surface);
        drawApparel(_clothingLegsSprite, EQUIPMENT_TYPE::CLOTHING_LEGS, surface);
        drawApparel(_clothingFeetSprite, EQUIPMENT_TYPE::CLOTHING_FEET, surface);

        drawApparel(_armorBodySprite, EQUIPMENT_TYPE::ARMOR_BODY, surface);
        drawApparel(_armorLegsSprite, EQUIPMENT_TYPE::ARMOR_LEGS, surface);
        drawApparel(_armorFeetSprite, EQUIPMENT_TYPE::ARMOR_FEET, surface);
    }
}

void Player::drawApparel(sf::Sprite& sprite, EQUIPMENT_TYPE equipType, sf::RenderTexture& surface) {
    if (equipType == EQUIPMENT_TYPE::CLOTHING_HEAD || equipType == EQUIPMENT_TYPE::ARMOR_HEAD) {
        int spriteHeight = 3;
        int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * spriteHeight : 0;

        if (getInventory().getEquippedItemId(equipType) != NOTHING_EQUIPPED) {
            sf::IntRect itemTextureRect = Item::ITEMS[getInventory().getEquippedItemId(equipType)]->getTextureRect();
            int spriteY = itemTextureRect.top + TILE_SIZE;

            sprite.setTextureRect(sf::IntRect(
                itemTextureRect.left + TILE_SIZE * 3 * _movingDir, spriteY + yOffset, TILE_SIZE * 3, TILE_SIZE * spriteHeight)
            );

            sprite.setPosition(sf::Vector2f(_sprite.getPosition().x - TILE_SIZE, _sprite.getPosition().y - TILE_SIZE));
            surface.draw(sprite);
        }
    } else {
        int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE : 0;

        if (getInventory().getEquippedItemId(equipType) != NOTHING_EQUIPPED) {
            sf::IntRect itemTextureRect = Item::ITEMS[getInventory().getEquippedItemId(equipType)]->getTextureRect();
            int spriteY = itemTextureRect.top;

            sprite.setTextureRect(sf::IntRect(
                itemTextureRect.left + TILE_SIZE * _movingDir, spriteY + yOffset, TILE_SIZE, TILE_SIZE)
            );

            sprite.setPosition(sf::Vector2f(_sprite.getPosition().x, _sprite.getPosition().y + TILE_SIZE));
            surface.draw(sprite);
        }
    }
}

void Player::move(float xa, float ya) {
    if (std::abs(xa) > 0 || std::abs(ya) > 0) {
        _numSteps++;
        _isMoving = true;
    } else if (isSwimming()) {
        _numSteps++;
        _isMoving = false;
    } else _isMoving = false;

    _pos.x += xa;
    _pos.y += ya;
}

sf::Vector2f Player::getPosition() const {
    return _pos;
}

bool Player::isSwimming() const {
    return _isSwimming;
}

bool Player::isDodging() const {
    return _isDodging;
}

void Player::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(0, 0, 16, 32));
    _sprite.setPosition(getPosition());

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2));

    _clothingHeadSprite.setTexture(*spriteSheet); 
    _clothingBodySprite.setTexture(*spriteSheet);
    _clothingLegsSprite.setTexture(*spriteSheet);
    _clothingFeetSprite.setTexture(*spriteSheet);

    _armorHeadSprite.setTexture(*spriteSheet);
    _armorBodySprite.setTexture(*spriteSheet);
    _armorLegsSprite.setTexture(*spriteSheet);
    _armorFeetSprite.setTexture(*spriteSheet);

    _toolSprite.setTexture(*spriteSheet);
}
