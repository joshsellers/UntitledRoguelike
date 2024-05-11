#include "Player.h"
#include <iostream>
#include "Item.h"
#include "Globals.h"
#include "World.h"
#include "MultiplayerManager.h"

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

    _multiplayerSendUpdates = true;

    _entityType = "player";
}

void Player::update() {
    if (_isReloading) {
        const Item* weapon = Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)];
        if (currentTimeMillis() - _reloadStartTimeMillis >= weapon->getReloadTimeMilliseconds()) {
            _isReloading = false;
            _magazineContents = _magContentsFilled;
        } else {
            float maxAmmo = ((float)weapon->getMagazineSize()) - ((float)weapon->getMagazineSize() - (float)_magContentsFilled);
            float reloadProgress = (float)(currentTimeMillis() - _reloadStartTimeMillis) / weapon->getReloadTimeMilliseconds();

            _magazineContents = maxAmmo * reloadProgress;
        }
    }
    fireAutomaticWeapon();

    float xAxis = 0;
    float yAxis = 0;
    if (GameController::isConnected()) {
        xAxis = GameController::getLeftStickXAxis();
        yAxis = GameController::getLeftStickYAxis();
    }

    float xa = 0, ya = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || (std::abs(yAxis) > std::abs(xAxis) && yAxis < 0)) {
        ya = -getBaseSpeed();
        _movingDir = UP;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || (std::abs(yAxis) > std::abs(xAxis) && yAxis > 0)) {
        ya = getBaseSpeed();
        _movingDir = DOWN;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || (std::abs(yAxis) < std::abs(xAxis) && xAxis < 0)) {
        xa = -getBaseSpeed();
        _movingDir = LEFT;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || (std::abs(yAxis) < std::abs(xAxis) && xAxis > 0)) {
        xa = getBaseSpeed();
        _movingDir = RIGHT;
    }

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || GameController::isButtonPressed(CONTROLLER_BUTTON::LEFT_BUMPER)) 
        && !isDodging() && (!isSwimming() || NO_MOVEMENT_RESTRICIONS || freeMove)) {
        xa *= _sprintMultiplier;
        ya *= _sprintMultiplier;
        _animSpeed = 2;
    } else if (!isSwimming() && isMoving()) {
        _animSpeed = 3;
    } else if (isSwimming() && !(NO_MOVEMENT_RESTRICIONS || freeMove)) {
        _animSpeed = 4;
        xa /= 2.f;
        ya /= 2.f;
    }

    if ((!isSwimming() || NO_MOVEMENT_RESTRICIONS || freeMove) && !isDodging() 
        && (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || GameController::isButtonPressed(CONTROLLER_BUTTON::A)) && _dodgeKeyReleased) {
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

    _dodgeKeyReleased = !sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !GameController::isButtonPressed(CONTROLLER_BUTTON::A);

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
    TERRAIN_TYPE terrainType = getCurrentTerrain();
    _isSwimming =  terrainType == TERRAIN_TYPE::WATER;

    if (getWorld()->playerIsInShop()) {
        _isSwimming = false;
        terrainType = TERRAIN_TYPE::EMPTY;
    }

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
                if (getMagazineAmmoType() != ammoId || getMagazineContents() == 0) {
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
            if (IS_MULTIPLAYER_CONNECTED && _multiplayerSendUpdates) {
                _multiplayerAimAngle = angle;
            } else if (IS_MULTIPLAYER_CONNECTED) {
                angle = _multiplayerAimAngle;
            }

            if (GameController::isConnected()) { 
                angle = (float)(((std::atan2(GameController::getRightStickYAxis(), GameController::getRightStickXAxis()))) * (180. / PI)) + 90.f;
                if (GameController::isRightStickDeadZoned()) {
                    switch (_movingDir) {
                        case UP:
                            angle = 0.f;
                            break;
                        case DOWN:
                            angle = 180.f;
                            break;
                        case LEFT:
                            angle = 270.f;
                            break;
                        case RIGHT:
                            angle = 90.f;
                            break;
                    }
                }
            }

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

            _isHoldingWeapon = true;
        }
        
        surface.draw(_toolSprite);

        if (getWorld()->showDebug()) {
            surface.draw(meleeHitBoxDisplay);
            surface.draw(barrelDisplay);
        }
    } else {
        _facingDir = (MOVING_DIRECTION)_movingDir;

        _isHoldingWeapon = false;
    }
}

TERRAIN_TYPE Player::getCurrentTerrain() {
    return _world->getTerrainDataAt(
        _world->getCurrentChunk(), sf::Vector2f(((int)_pos.x + PLAYER_WIDTH / 2), ((int)_pos.y + PLAYER_HEIGHT))
    );
}

void Player::meleeAttack(sf::FloatRect meleeHitBox, sf::Vector2f currentMousePos) {
    int threshold = 25;
    sf::Vector2f delta = currentMousePos - _lastMousePos;

    if (GameController::isConnected()) {
        delta = sf::Vector2f(GameController::getRightStickXAxis(), GameController::getRightStickYAxis()) - _lastMousePos;
        threshold = 50;
    }

    if ((std::abs(delta.x) > threshold || std::abs(delta.y) > threshold) && (_meleeAttackDelayCounter & 3) == 0) {
        for (auto& entity : getWorld()->getEntities()) {
            if (!entity->compare(this) && entity->isDamageable() && meleeHitBox.intersects(entity->getHitBox())) {
                entity->takeDamage(Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getDamage());
            }
        }
    }

    _lastMousePos = currentMousePos;
    if (GameController::isConnected()) {
        _lastMousePos = sf::Vector2f(GameController::getRightStickXAxis(), GameController::getRightStickYAxis());
    }

    _meleeAttackDelayCounter++;
}

void Player::move(float xa, float ya) {
    if (std::abs(xa) > 0 || std::abs(ya) > 0) {
        _numSteps++;

        //if (getWorld()->playerIsInShop()) {
            for (auto& entity : getWorld()->getEntities()) {
                if (entity->isActive() && entity->hasColliders()) {
                    for (auto& collider : entity->getColliders()) {
                        if (sf::FloatRect(_pos.x + xa, _pos.y + ya, PLAYER_WIDTH, PLAYER_HEIGHT).intersects(collider)) return;
                    }
                }
            }
        //}

        _isMoving = true;
        _isActuallyMoving = true;
    } else if (isSwimming()) {
        _numSteps++;
        // I don't know why _isMoving is set to false here, but if I put it here it's gotta be important right?
        // The above is the reason for _isActuallyMoving
        _isMoving = false;
        _isActuallyMoving = true;
    } else {
        _isMoving = false;
        _isActuallyMoving = false;
    }

    _pos.x += xa;
    _pos.y += ya;
    _velocity.x = xa;
    _velocity.y = ya;
}

sf::Vector2f Player::getPosition() const {
    return _pos;
}

bool Player::isDodging() const {
    return _isDodging;
}

void Player::knockBack(float amt, MOVING_DIRECTION dir) {
    if (!freeMove) {
        switch (dir) {
            case UP:
                move(0, -amt);
                break;
            case DOWN:
                move(0, amt);
                break;
            case LEFT:
                move(-amt, 0);
                break;
            case RIGHT:
                move(amt, 0);
                break;
        }
    }
}

void Player::damage(int damage) {
    if (!isDodging() && !freeMove) {
        if (GameController::isConnected()) {
            int vibrationAmount = ((float)MAX_CONTROLLER_VIBRATION * std::min(((float)damage / (float)getMaxHitPoints()), (float)100));
            GameController::vibrate(vibrationAmount, 250);
        }
        _hitPoints -= damage;
        if (_hitPoints <= 0) {
            _isActive = false;
        }
    }
}

void Player::mouseButtonReleased(const int mx, const int my, const int button) {
    if (button == sf::Mouse::Button::Left && !_isReloading) {
        fireWeapon();
    }
}

void Player::keyReleased(sf::Keyboard::Key& key) {
    if (key == sf::Keyboard::R) {
        startReloadingWeapon();
    }
}

void Player::controllerButtonReleased(CONTROLLER_BUTTON button) {
    switch (button) {
        case CONTROLLER_BUTTON::X:
            startReloadingWeapon();
            break;
        default:
            break;
    }
}

void Player::controllerButtonPressed(CONTROLLER_BUTTON button) {
    switch (button) {
        case CONTROLLER_BUTTON::RIGHT_TRIGGER:
            if (!_isReloading) fireWeapon();
            break;
        default:
            break;
    }
}

void Player::fireWeapon() {
    if (!_gamePaused &&
        getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL) != NOTHING_EQUIPPED &&
        Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isGun() &&
        !Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isAutomatic()
        && !isSwimming() && !isDodging()) {
        unsigned int id = getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL);
        Item::ITEMS[id]->use(this);
    }
}

void Player::fireAutomaticWeapon() {
    if (!_gamePaused && !_inventoryMenuIsOpen &&
        (sf::Mouse::isButtonPressed(sf::Mouse::Left) || GameController::isButtonPressed(CONTROLLER_BUTTON::RIGHT_TRIGGER)) && !_isReloading &&
        getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL) != NOTHING_EQUIPPED &&
        Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isGun() &&
        Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isAutomatic() &&
        currentTimeMillis() - _lastAutoFireTimeMillis >= Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getFireRateMilliseconds()
        && !isSwimming() && !isDodging()) {
        _lastAutoFireTimeMillis = currentTimeMillis();
        unsigned int id = getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL);
        Item::ITEMS[id]->use(this);
    }
}

void Player::startReloadingWeapon() {
    if (!_gamePaused && getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL) != NOTHING_EQUIPPED &&
        Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isGun() &&
        Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getReloadTimeMilliseconds() == 0) {
        reloadWeapon();
        return;
    } else if (reloadWeapon()) {
        _isReloading = true;
        _reloadStartTimeMillis = currentTimeMillis();
        _magContentsFilled = _magazineContents;
        _magazineContents = 0;
    }
}

bool Player::reloadWeapon() {
    if (!_gamePaused && getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL) != NOTHING_EQUIPPED &&
        Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isGun()) {
        if (getInventory().hasItem(Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getAmmoId())) {
            getInventory().equip(
                getInventory().findItem(Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getAmmoId()),
                EQUIPMENT_TYPE::AMMO
            );

            if (getMagazineContents() == 0) {
                const Item* weapon = Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)];
                const Item* ammo = Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::AMMO)];
                const unsigned int removeAmount = std::min(
                    (unsigned int)weapon->getMagazineSize(), getInventory().getItemAmountAt(getInventory().getEquippedIndex(EQUIPMENT_TYPE::AMMO))
                );
                getInventory().removeItemAt(getInventory().getEquippedIndex(EQUIPMENT_TYPE::AMMO), removeAmount);
                _magazineAmmoType = ammo->getId();
                _magazineSize = weapon->getMagazineSize();
                _magazineContents = (int)removeAmount;

                return true;
            }
        }
    }
    return false;
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
