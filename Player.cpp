#include "Player.h"
#include <iostream>
#include "Item.h"
#include "Globals.h"
#include "World.h"

Player::Player(sf::Vector2f pos, sf::RenderWindow* window, bool& gamePaused) : 
    Entity(pos, BASE_PLAYER_SPEED, PLAYER_WIDTH / TILE_SIZE, PLAYER_HEIGHT / TILE_SIZE, false), _window(window), _gamePaused(gamePaused) {
    _canPickUpItems = true;

    setMaxHitPoints(100);
    heal(getMaxHitPoints());

    _hitBoxXOffset = 0;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE;
    _hitBox.height = TILE_SIZE * 2;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
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

    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE;
        _hitBox.height = TILE_SIZE;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 2;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
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
        isDodging() && isMoving() ? xOffset : 16 * _facingDir, 
        isDodging() && isMoving() ? 128 : 0 + yOffset, 
        16, 
        terrainType == TERRAIN_TYPE::WATER ? 16 : 32)
    );

    if (_facingDir == UP || _facingDir == LEFT) {
        drawTool(surface);

        surface.draw(_sprite);

        if (!isDodging() || !isMoving()) drawEquipables(surface);
    } else if (_facingDir == DOWN || _facingDir == RIGHT) {
        surface.draw(_sprite);

        if (!isDodging() || !isMoving()) drawEquipables(surface);
        drawTool(surface);
    }

    //// i guess whatever 16 is multiplied by has to be odd
    //int dist = 16 * 79;
    //sf::Vector2i mPos = sf::Mouse::getPosition(*_window);
    //sf::Vector2i goal(_window->mapPixelToCoords(mPos, surface.getView()).x, _window->mapPixelToCoords(mPos, surface.getView()).y);
    //sf::Vector2i cLoc(((int)getPosition().x) + PLAYER_WIDTH / 2, ((int)getPosition().y) + PLAYER_HEIGHT / 2);
    //SearchArea searchArea = PathFinder::createSearchArea(sf::Vector2i(cLoc.x - dist / 2, cLoc.y - dist / 2), dist, this, *getWorld());
    //std::unordered_map<sf::Vector2i, sf::Vector2i> map = PathFinder::breadthFirstSearch(searchArea, cLoc, goal);

    //std::cout << searchArea.walls.size() << std::endl;
    //for (auto& point : map) {
    //    if (sf::IntRect(point.first.x, point.first.y, 16, 16).contains(goal)) {
    //        goal = point.first;
    //        break;
    //    }
    //    /*sf::RectangleShape a;
    //    a.setPosition(point.first.x, point.first.y);
    //    a.setSize(sf::Vector2f(10, 10));
    //    a.setFillColor(sf::Color(0xFF0000FF));
    //    surface.draw(a);

    //    sf::RectangleShape b;
    //    b.setPosition(point.second.x, point.second.y);
    //    b.setSize(sf::Vector2f(5, 5));
    //    b.setFillColor(sf::Color(0x00FF00FF));
    //    surface.draw(b);

    //    sf::RectangleShape c;
    //    c.setPosition(goal.x, goal.y);
    //    c.setSize(sf::Vector2f(5, 5));
    //    c.setFillColor(sf::Color(0x0000FFFF));
    //    surface.draw(c);*/
    //}

    //for (auto& wall : searchArea.walls) {
    //    sf::RectangleShape a;
    //    a.setPosition(wall.x, wall.y);
    //    a.setSize(sf::Vector2f(16, 16));
    //    a.setFillColor(sf::Color(0xFF000099));
    //    surface.draw(a);
    //}

    //std::vector<sf::Vector2i> path = PathFinder::reconstruct_path(cLoc, goal, map);

    //for (auto& point : path) {
    //    sf::RectangleShape a;
    //    a.setPosition(point.x, point.y);
    //    a.setSize(sf::Vector2f(16, 16));
    //    a.setFillColor(sf::Color(0xFF00FF99));
    //    surface.draw(a);
    //}
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
                itemTextureRect.left + TILE_SIZE * 3 * _facingDir, spriteY + yOffset, TILE_SIZE * 3, TILE_SIZE * spriteHeight)
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
                itemTextureRect.left + TILE_SIZE * _facingDir, spriteY + yOffset, TILE_SIZE, TILE_SIZE)
            );

            sprite.setPosition(sf::Vector2f(_sprite.getPosition().x, _sprite.getPosition().y + TILE_SIZE));
            surface.draw(sprite);
        }
    }
}

void Player::drawTool(sf::RenderTexture& surface) {
    if (getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL) != NOTHING_EQUIPPED && !isSwimming() && (!isDodging() || !isMoving())) {
        sf::RectangleShape meleeHitBoxDisplay;
        sf::RectangleShape barrelDisplay;
        if (!_gamePaused) {
            sf::IntRect itemTextureRect =
                Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getTextureRect();
            int spriteX = itemTextureRect.left + TILE_SIZE;
            if (Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isGun()) {
                int ammoId = Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getAmmoId();
                if (!getInventory().hasItem(ammoId) || getInventory().getEquippedItemId(EQUIPMENT_TYPE::AMMO) != ammoId) {
                    spriteX += TILE_SIZE * 3;
                }
            }
            _toolSprite.setTextureRect(sf::IntRect(
                spriteX, itemTextureRect.top, TILE_SIZE * 3, TILE_SIZE * 3
            ));

            _toolSprite.setOrigin(sf::Vector2f(_toolSprite.getLocalBounds().width / 2, _toolSprite.getLocalBounds().height));

            sf::Vector2f handPos = sf::Vector2f(getPosition().x, getPosition().y + 23);
            sf::Vector2i mPos = sf::Mouse::getPosition(*_window);

            sf::Vector2i center = _window->mapCoordsToPixel(handPos, surface.getView());

            double x = (double)(mPos.x - center.x);
            double y = (double)(mPos.y - center.y);

            float angle = (float)(std::atan2(y, x) * (180. / PI)) + 90.f;

            if (angle >= -45.f && angle < 45.f) 
                _facingDir = UP;
            else if (angle >= 45.f && angle < 135.f)
                _facingDir = RIGHT;
            else if (angle >= 135.f && angle < 225.f) 
                _facingDir = DOWN;
            else if (angle >= 225.f || angle < -45.f)
                _facingDir = LEFT;

            if (angle >= 0.f && angle < 180.f) _toolSprite.setScale(-1, 1);
            else if (angle >= 180 || angle < 0) _toolSprite.setScale(1, 1);

            _toolSprite.setRotation(angle);

            switch (_facingDir) {
            case UP:
                handPos.x += 13;
                break;
            case DOWN:
                handPos.x += 2;
                break;
            case LEFT:
                handPos.x += 7;
                break;
            case RIGHT:
                handPos.x += 8;
                break;
            }

            _toolSprite.setPosition(sf::Vector2f(
                handPos.x, handPos.y
            ));

            if (Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isMeleeWeapon()) {
                int meleeHitBoxSize = Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getHitBoxSize();
                float r = Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getHitBoxPos();
                sf::Vector2f meleeHitBoxPos(handPos.x + r * std::cos((angle - 90.f) * (PI / 180.f)), handPos.y + r * std::sin((angle - 90.f) * (PI / 180.f)));
                meleeHitBoxPos.x -= (float)meleeHitBoxSize / 2;
                meleeHitBoxPos.y -= (float)meleeHitBoxSize / 2;

                if (getWorld()->showDebug()) {
                    meleeHitBoxDisplay.setPosition(meleeHitBoxPos);
                    meleeHitBoxDisplay.setSize(sf::Vector2f(meleeHitBoxSize, meleeHitBoxSize));
                    meleeHitBoxDisplay.setFillColor(sf::Color::Transparent);
                    meleeHitBoxDisplay.setOutlineColor(sf::Color(0xFF0000FF));
                    meleeHitBoxDisplay.setOutlineThickness(1.f);
                }
                
                sf::FloatRect meleeHitBox;
                meleeHitBox.left = meleeHitBoxPos.x;
                meleeHitBox.top = meleeHitBoxPos.y;
                meleeHitBox.width = meleeHitBoxSize;
                meleeHitBox.height = meleeHitBoxSize;

                meleeAttack(meleeHitBox, _window->mapPixelToCoords(mPos, surface.getView()));
            }

            if (Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isGun()) {
                sf::Vector2f barrelPos = Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getBarrelPos();
                float r = barrelPos.x;
                sf::Vector2f xAxisPos(handPos.x + r * std::cos((angle - 90.f) * (PI / 180.f)), handPos.y + r * std::sin((angle - 90.f) * (PI / 180.f)));
                float q = barrelPos.y * _toolSprite.getScale().x;
                _calculatedBarrelPos = sf::Vector2f(xAxisPos.x + q * std::cos((angle) * (PI / 180.f)), xAxisPos.y + q * std::sin((angle) * (PI / 180.f)));
                float targetPosDist = 10;
                _targetPos = sf::Vector2f(_calculatedBarrelPos.x + targetPosDist * std::cos((angle - 90.f) * (PI / 180.f)),
                    _calculatedBarrelPos.y + targetPosDist * std::sin((angle - 90.f) * (PI / 180.f)));

                if (getWorld()->showDebug()) {
                    barrelDisplay.setPosition(_calculatedBarrelPos);
                    barrelDisplay.setSize(sf::Vector2f(1, 1));
                    barrelDisplay.setFillColor(sf::Color::Transparent);
                    barrelDisplay.setOutlineColor(sf::Color(0xFF0000FF));
                    barrelDisplay.setOutlineThickness(1.f);
                }
            }
        }
        
        surface.draw(_toolSprite);

        if (getWorld()->showDebug()) {
            surface.draw(meleeHitBoxDisplay);
            surface.draw(barrelDisplay);
        }
    } else _facingDir = (MOVING_DIRECTION)_movingDir;
}

void Player::meleeAttack(sf::FloatRect meleeHitBox, sf::Vector2f currentMousePos) {
    sf::Vector2f delta = currentMousePos - _lastMousePos;
    const int threshold = 25;
    if ((std::abs(delta.x) > threshold || std::abs(delta.y) > threshold) && (_meleeAttackDelayCounter & 3) == 0) {
        for (auto& entity : getWorld()->getEntities()) {
            if (!entity->compare(this) && entity->isDamageable() && meleeHitBox.intersects(entity->getHitBox())) {
                entity->damage(Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getDamage());
            }
        }
    }
    _lastMousePos = currentMousePos;

    _meleeAttackDelayCounter++;
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
    _velocity.x = xa;
    _velocity.y = ya;
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

void Player::damage(int damage) {
    if (!isDodging()) {
        _hitPoints -= damage;
        if (_hitPoints <= 0) {
            _isActive = false;
        }
    }
}

void Player::mouseButtonReleased(const int mx, const int my, const int button) {
    if (!_gamePaused && button == sf::Mouse::Button::Left && 
        getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL) != NOTHING_EQUIPPED && 
        Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isGun()
        && !isSwimming() && !isDodging()) {
        unsigned int id = getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL);
        Item::ITEMS[id]->use(this);
    }
}

void Player::keyReleased(sf::Keyboard::Key& key) {
    if (!_gamePaused && key == sf::Keyboard::R) {
        if (getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL) != NOTHING_EQUIPPED &&
            Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isGun()) {
            if (getInventory().hasItem(Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getAmmoId())) {
                getInventory().equip(
                    getInventory().findItem(Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getAmmoId()),
                    EQUIPMENT_TYPE::AMMO
                );
            }
        }
    }
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
