#include "Player.h"
#include <iostream>
#include "../World.h"
#include "../../core/InputBindings.h"
#include "../../statistics/StatManager.h"

Player::Player(sf::Vector2f pos, sf::RenderWindow* window, bool& gamePaused) : 
    HairyEntity(PLAYER, pos, BASE_PLAYER_SPEED, PLAYER_WIDTH / TILE_SIZE, PLAYER_HEIGHT / TILE_SIZE), _window(window), _gamePaused(gamePaused) {
    _canPickUpItems = true;

    setMaxHitPoints(100);
    heal(getMaxHitPoints());
    _stamina = _maxStamina;

    _hitBoxXOffset = 0;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE;
    _hitBox.height = TILE_SIZE * 2;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _entityType = "player";
}

void Player::update() {
    if (isInBoat() && !isSwimming()) getInventory().deEquip(EQUIPMENT_TYPE::BOAT);

    if (_isReloading) {
        std::shared_ptr<const Item> weapon = Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)];
        if (currentTimeMillis() - _reloadStartTimeMillis >= weapon->getReloadTimeMilliseconds()) {
            _isReloading = false;
            _magazineContents = _magContentsFilled;

            _magContentsPercentage = ((float)_magazineContents / weapon->getMagazineSize()) * 100;
        } else {
            float maxAmmo = ((float)weapon->getMagazineSize()) - ((float)weapon->getMagazineSize() - (float)_magContentsFilled);
            float reloadProgress = (float)(currentTimeMillis() - _reloadStartTimeMillis) / weapon->getReloadTimeMilliseconds();

            _magazineContents = maxAmmo * reloadProgress;

            _magContentsPercentage = ((float)(maxAmmo * reloadProgress) / (float)weapon->getMagazineSize()) * 100;
        }
    }

    fireAutomaticWeapon();

    float xAxis = 0;
    float yAxis = 0;
    if (GamePad::isConnected()) {
        xAxis = GamePad::getLeftStickXAxis();
        yAxis = GamePad::getLeftStickYAxis();
    }

    float xa = 0, ya = 0;

    if (DIAGONAL_MOVEMENT_ENABLED) {
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W)) || (yAxis < -20.f)) {
            ya = -getBaseSpeed();
            _movingDir = UP;
        } else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) || (yAxis > 20.f)) {
            ya = getBaseSpeed();
            _movingDir = DOWN;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || (xAxis < -20.f)) {
            xa = -getBaseSpeed();
            _movingDir = LEFT;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || (xAxis > 20.f)) {
            xa = getBaseSpeed();
            _movingDir = RIGHT;
        }
    } else {
        bool upOrDownIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        bool leftOrRightIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::D);
        bool verticalPressedLast = _lastUpOrDownPressTime > _lastLeftOrRightPressTime;

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) && (!leftOrRightIsPressed || verticalPressedLast)) || (std::abs(yAxis) > std::abs(xAxis) && yAxis < 0)) {
            ya = -getBaseSpeed();
            _movingDir = UP;
        } else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) && (!leftOrRightIsPressed || verticalPressedLast)) || (std::abs(yAxis) > std::abs(xAxis) && yAxis > 0)) {
            ya = getBaseSpeed();
            _movingDir = DOWN;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || (std::abs(yAxis) < std::abs(xAxis) && xAxis < 0)) {
            xa = -getBaseSpeed();
            _movingDir = LEFT;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || (std::abs(yAxis) < std::abs(xAxis) && xAxis > 0)) {
            xa = getBaseSpeed();
            _movingDir = RIGHT;
        }
    }

    if (DIAGONAL_MOVEMENT_ENABLED && xa && ya) {
        constexpr float diagonalMultiplier = 0.707107; // 0.785398
        xa *= diagonalMultiplier;
        ya *= diagonalMultiplier;
    }
    const float velX = xa, velY = ya;

    if ((sf::Keyboard::isKeyPressed(InputBindingManager::getKeyboardBinding(InputBindingManager::BINDABLE_ACTION::WALK))
            || GamePad::isButtonPressed(InputBindingManager::getGamepadBinding(InputBindingManager::BINDABLE_ACTION::WALK)))
        && !isDodging() && (!isSwimming() || NO_MOVEMENT_RESTRICIONS || freeMove) && !isInBoat()) {
        xa *= _slowMoveMultiplier;
        ya *= _slowMoveMultiplier;
        _animSpeed = 3;
    } else if (!isSwimming() && isMoving() || isInBoat()) {
        _animSpeed = 2;
    } else if (isSwimming() && !(NO_MOVEMENT_RESTRICIONS || freeMove)) {
        _animSpeed = 4;
        xa /= 4.f;
        ya /= 4.f;
    }

    if (hasSufficientStamina(DODGE_STAMINA_COST) && (!isSwimming() || NO_MOVEMENT_RESTRICIONS || freeMove) && !isDodging() && !isInBoat()
        && (sf::Keyboard::isKeyPressed(InputBindingManager::getKeyboardBinding(InputBindingManager::BINDABLE_ACTION::DODGE)) 
            || GamePad::isButtonPressed(InputBindingManager::getGamepadBinding(InputBindingManager::BINDABLE_ACTION::DODGE))) && _dodgeKeyReleased) {
        _isDodging = true;
        _dodgeTimer++;

        if (isMoving()) _stamina = std::max(_stamina - DODGE_STAMINA_COST, 0);
    } else if (isDodging() && _dodgeTimer < _maxDodgeTime) {
        _dodgeSpeedMultiplier = _dodgeMultiplier(_dodgeTimer);
        _dodgeTimer++;
    } else if (isDodging() && _dodgeTimer >= _maxDodgeTime) {
        _isDodging = false;
        _dodgeTimer = 0;
        _dodgeSpeedMultiplier = 1.f;

        StatManager::increaseStat(TIMES_ROLLED, 1.f);
    }

    _dodgeKeyReleased = 
        !sf::Keyboard::isKeyPressed(InputBindingManager::getKeyboardBinding(InputBindingManager::BINDABLE_ACTION::DODGE)) 
        && !GamePad::isButtonPressed(InputBindingManager::getGamepadBinding(InputBindingManager::BINDABLE_ACTION::DODGE));

    float oldX = getPosition().x, oldY = getPosition().y;
    move(xa * _dodgeSpeedMultiplier, ya * _dodgeSpeedMultiplier);
    if (DIAGONAL_MOVEMENT_ENABLED && xa && ya) {
        if (std::abs(oldX - getPosition().x) > std::abs(oldY - getPosition().y)) {
            float xRounded = std::round(_pos.x);
            float yRounded = std::round(_pos.y + (xRounded - _pos.x) * velY / velX);
            _pos.y = yRounded;
        } else if (std::abs(oldX - getPosition().x) <= std::abs(oldY - getPosition().y)) {
            float yRounded = std::round(_pos.y);
            float xRounded = std::round(_pos.x + (yRounded - _pos.y) * velX / velY);
            _pos.x = xRounded;
        }
    }

    _sprite.setPosition(getPosition()); 
    if (isInBoat()) _boatSprite.setPosition(sf::Vector2f(getPosition().x - TILE_SIZE, getPosition().y));

    if (isSwimming() && !isInBoat()) {
        _hitBoxYOffset = TILE_SIZE; 
        _hitBox.width = TILE_SIZE;
        _hitBox.height = TILE_SIZE;
    } else if (!isDodging() || !isMoving()) {
        _hitBoxYOffset = 0;
        _hitBox.width = TILE_SIZE;
        _hitBox.height = TILE_SIZE * 2;
    } else if (isDodging() && isMoving()) {
        _hitBox.width = 0;
        _hitBox.height = 0;
    }

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _isUsingStamina = isDodging();
    if (!isUsingStamina()) {
        _stamina = std::min(_stamina + getStaminaRefreshRate(), getMaxStamina());
    }

    blink();
}

void Player::draw(sf::RenderTexture& surface) {
    if (_isVisible) {
        TERRAIN_TYPE terrainType = getCurrentTerrain();
        _isSwimming = terrainType == TERRAIN_TYPE::WATER;

        if (getWorld()->playerIsInShop()) {
            _isSwimming = false;
            terrainType = TERRAIN_TYPE::EMPTY;
        }

        if (isSwimming() && !isInBoat()) {
            _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2));

            int xOffset = ((_numSteps >> _animSpeed) & 1) * 16;

            _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, 16, 16));
            _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2 + 8));
            surface.draw(_wavesSprite);
        } else if (isInBoat()) {
            _numSteps = 0;
            const long long boatAnimationTime = 500LL;
            if (currentTimeMillis() - _lastTimeBoatBobbedUp < boatAnimationTime && !_gamePaused) {
                _sprite.move(0, 1);
                _boatSprite.move(0, 1);
            } else if (!_gamePaused && currentTimeMillis() - _lastTimeBoatBobbedUp >= boatAnimationTime * 2) _lastTimeBoatBobbedUp = currentTimeMillis();
        }

        int xOffset = isDodging() ? ((_numSteps >> (_animSpeed / 2)) & 3) * 16 : 0;
        int yOffset = isMoving() || (isSwimming() && !isInBoat()) ? ((_numSteps >> _animSpeed) & 3) * 32 : 0;
        _sprite.setTextureRect(sf::IntRect(
            isDodging() && isMoving() ? xOffset : 16 * _facingDir, (_isBlinking ? 592 : 0) +
            (isDodging() && isMoving() ? 128 : 0 + yOffset),
            16,
            terrainType == TERRAIN_TYPE::WATER && !isInBoat() ? 16 : 32)
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

        if (isInBoat()) {
            _boatSprite.setTextureRect(sf::IntRect(
                (TILE_SIZE * 3) * _facingDir,
                544,
                TILE_SIZE * 3, TILE_SIZE * 3
            ));

            surface.draw(_boatSprite);
        }
    }
}

void Player::drawEquipables(sf::RenderTexture& surface) {
    drawHair(surface);

    drawApparel(_clothingHeadSprite, EQUIPMENT_TYPE::CLOTHING_HEAD, surface);
    drawApparel(_armorHeadSprite, EQUIPMENT_TYPE::ARMOR_HEAD, surface);

    if (!isSwimming() || isInBoat()) {
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
        if (equipType == EQUIPMENT_TYPE::CLOTHING_HEAD && getInventory().getEquippedItemId(EQUIPMENT_TYPE::ARMOR_HEAD) != NOTHING_EQUIPPED) return;

        constexpr int spriteHeight = 3;
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
    if (getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL) != NOTHING_EQUIPPED && !isSwimming() && !isInBoat() && (!isDodging() || !isMoving())) {
        sf::RectangleShape meleeHitBoxDisplay;
        sf::RectangleShape barrelDisplay;
        if (!_gamePaused) {
            std::shared_ptr<const Item> equippedTool = Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)];
            sf::IntRect itemTextureRect =
                equippedTool->getTextureRect();
            int spriteX = itemTextureRect.left + TILE_SIZE;
            int spriteY = itemTextureRect.top;
            if (equippedTool->isGun()) {
                int ammoId = equippedTool->getAmmoId();
                if (getMagazineAmmoType() != ammoId || getMagazineContents() == 0) {
                    spriteX += TILE_SIZE * 3;
                } else if (_isFiringAutomaticWeapon && equippedTool->isAnimated()) {
                    const WeaponAnimationConfig* animConfig = &equippedTool->getAnimationConfig();
                    spriteY += ((_automaticWeaponAnimationCounter / animConfig->ticksPerFrame) % animConfig->frameCount) * TILE_SIZE * 3;
                    _automaticWeaponAnimationCounter++;
                }
            }
            _toolSprite.setTextureRect(sf::IntRect(
                spriteX, spriteY, TILE_SIZE * 3, TILE_SIZE * 3
            ));

            _toolSprite.setOrigin(sf::Vector2f(_toolSprite.getLocalBounds().width / 2, _toolSprite.getLocalBounds().height));

            sf::Vector2f handPos = sf::Vector2f(getPosition().x, getPosition().y + 23);
            sf::Vector2i mPos = sf::Mouse::getPosition(*_window);

            sf::Vector2i center = _window->mapCoordsToPixel(handPos, surface.getView());

            double x = (double)(mPos.x - center.x);
            double y = (double)(mPos.y - center.y);

            float angle = (float)(std::atan2(y, x) * (180. / PI)) + 90.f;

            if (GamePad::isConnected()) { 
                angle = (float)(((std::atan2(GamePad::getRightStickYAxis(), GamePad::getRightStickXAxis()))) * (180. / PI)) + 90.f;
                if (GamePad::isRightStickDeadZoned()) {
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

            if (equippedTool->isMeleeWeapon()) {
                int meleeHitBoxSize = equippedTool->getHitBoxSize();
                float r = equippedTool->getHitBoxPos();
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

            if (equippedTool->isGun()) {
                sf::Vector2f barrelPos = equippedTool->getBarrelPos();
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

    if (GamePad::isConnected()) {
        delta = sf::Vector2f(GamePad::getRightStickXAxis(), GamePad::getRightStickYAxis()) - _lastMousePos;
        threshold = 50;
    }

    if ((std::abs(delta.x) > threshold || std::abs(delta.y) > threshold) && (_meleeAttackDelayCounter & 3) == 0) {
        for (auto& entity : getWorld()->getEntities()) {
            if (!entity->compare(this) && entity->isDamageable() && meleeHitBox.intersects(entity->getHitBox())) {
                int damage = Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getDamage() * getDamageMultiplier();
                entity->takeDamage(damage);
                StatManager::increaseStat(DAMAGE_DEALT, damage);
            }
        }
    }

    _lastMousePos = currentMousePos;
    if (GamePad::isConnected()) {
        _lastMousePos = sf::Vector2f(GamePad::getRightStickXAxis(), GamePad::getRightStickYAxis());
    }

    _meleeAttackDelayCounter++;
}

void Player::blink() {
    const long long blinkDuration = 100LL;
    const int blinkChance = 600;
    if (!_isBlinking) {
        _isBlinking = randomInt(0, blinkChance) == 0;
        if (_isBlinking) _blinkStartTime = currentTimeMillis();
    } else if (currentTimeMillis() - _blinkStartTime > blinkDuration) _isBlinking = false;

    if (isDodging() && isMoving()) _isBlinking = false;
}

void Player::move(float xa, float ya) {
    bool collidingX = false, collidingY = false;

    if (std::abs(xa) > 0 || std::abs(ya) > 0) {
        _numSteps++;

        for (auto& entity : getWorld()->getEntities()) {
            if (entity->isActive() && entity->hasColliders()) {
                for (auto& collider : entity->getColliders()) {
                    if (sf::FloatRect(_pos.x + xa, _pos.y, PLAYER_WIDTH, PLAYER_HEIGHT).intersects(collider)) collidingX = true;
                    if (sf::FloatRect(_pos.x, _pos.y + ya, PLAYER_WIDTH, PLAYER_HEIGHT).intersects(collider)) collidingY = true;
                    if (collidingX && collidingY) return;
                    else if (collidingX || collidingY) break;
                }
                if (collidingX || collidingY) break;
            }
        }

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

    if (!collidingX) _pos.x += xa;
    if (!collidingY) _pos.y += ya;
    // these are never used anywhere else 
    _velocity.x = xa;
    _velocity.y = ya;


    if ((!collidingX || !collidingY) && (xa != 0 || ya != 0)) {
        const int deltaPos = std::max(std::abs(xa), std::abs(ya));
        constexpr float metersPerPixel = 0.053;
        const float distMoved = (float)deltaPos * metersPerPixel;

        StatManager::increaseStat(DIST_TRAVELLED, distMoved);
        if (isSwimming() && !isInBoat()) StatManager::increaseStat(DIST_SWAM, distMoved);
        else if (isInBoat()) StatManager::increaseStat(DIST_SAILED, distMoved);
    }
}

sf::Vector2f Player::getPosition() const {
    return _pos;
}

bool Player::isDodging() const {
    return _isDodging;
}

bool Player::isInBoat() {
    return getInventory().getEquippedItemId(EQUIPMENT_TYPE::BOAT) != NOTHING_EQUIPPED;
}

int Player::getStamina() const {
    return _stamina;
}

int Player::getMaxStamina() const {
    return _maxStamina;
}

int Player::getStaminaRefreshRate() const {
    return _staminaRefreshRate;
}

int& Player::getStaminaRef() {
    return _stamina;
}

int& Player::getMaxStaminaRef() {
    return _maxStamina;
}

void Player::setMaxStamina(int amount) {
    _maxStamina = amount;
}

void Player::restoreStamina(int amount) {
    _stamina = std::min(_stamina + amount, getMaxStamina());
}

void Player::increaseStaminaRefreshRate(int amount) {
    _staminaRefreshRate += amount;
}

bool Player::isUsingStamina() {
    return _isUsingStamina;
}

bool Player::hasSufficientStamina(int cost) {
    return getStamina() - cost >= 0;
}

unsigned int Player::getCoinMagnetCount() const {
    return _coinMagnetCount;
}

void Player::addCoinMagnet() {
    _coinMagnetCount++;
}

int& Player::getMagazineContentsPercentage() {
    return _magContentsPercentage;
}

void Player::decrementMagazine() {
    if (_magazineContents != 0) {
        _magazineContents--;
        if (getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL) != NOTHING_EQUIPPED) {
            std::shared_ptr<const Item> weapon = Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)];
            _magContentsPercentage = ((float)_magazineContents / (float)weapon->getMagazineSize()) * 100.f;
        }
    }
}

void Player::emptyMagazine() {
    _magazineContents = 0;
    _magContentsPercentage = 0;
}

void Player::knockBack(float amt, MOVING_DIRECTION dir) {
    if (isDodging() && isMoving()) return;

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
    if (isDodging() && isMoving()) return;

    if (!isDodging() && !freeMove) {
        if (GamePad::isConnected()) {
            int vibrationAmount = ((float)MAX_CONTROLLER_VIBRATION * std::min(((float)damage / (float)getMaxHitPoints()), (float)100));
            GamePad::vibrate(vibrationAmount, 250);
        }

        _hitPoints -= (int)((float)damage * getTotalArmorCoefficient());
        if (_hitPoints <= 0) {
            _isActive = false;
            _hitPoints = 0;
        }

        StatManager::increaseStat(DAMAGE_TAKEN, damage);
    }
}

float getArmorCoefficient(unsigned int itemId) {
    constexpr float protectionDivisor = 100.f;

    if (itemId != NOTHING_EQUIPPED) {
        if (Item::ITEMS[itemId]->getEquipmentType() != EQUIPMENT_TYPE::ARMOR_HEAD
            && Item::ITEMS[itemId]->getEquipmentType() != EQUIPMENT_TYPE::ARMOR_BODY
            && Item::ITEMS[itemId]->getEquipmentType() != EQUIPMENT_TYPE::ARMOR_LEGS
            && Item::ITEMS[itemId]->getEquipmentType() != EQUIPMENT_TYPE::ARMOR_FEET) {
            MessageManager::displayMessage("Tried to get damage coefficient from non-armor item: " + std::to_string(itemId), 5, WARN);
            return 0.0f;
        }
        return (float)Item::ITEMS[itemId]->getDamage() / protectionDivisor;
    }

    return 0.0f;
}

float Player::getTotalArmorCoefficient() {
    float total = 0.f;
    unsigned int helmetId = getInventory().getEquippedItemId(EQUIPMENT_TYPE::ARMOR_HEAD);
    unsigned int chestplateId = getInventory().getEquippedItemId(EQUIPMENT_TYPE::ARMOR_BODY);
    unsigned int leggingsId = getInventory().getEquippedItemId(EQUIPMENT_TYPE::ARMOR_LEGS);
    unsigned int bootsId = getInventory().getEquippedItemId(EQUIPMENT_TYPE::ARMOR_FEET);

    total += getArmorCoefficient(helmetId) + getArmorCoefficient(chestplateId) + getArmorCoefficient(leggingsId) + getArmorCoefficient(bootsId);

    if (total >= 1.0f) {
        total = 1.0f;
        MessageManager::displayMessage("High total armor coefficient: \n"
            + std::to_string(helmetId) + ", " + std::to_string(chestplateId) + ", " + std::to_string(leggingsId) + ", " + std::to_string(bootsId), 5, WARN);
    }

    return 1.0f - total;
}


void Player::mouseButtonReleased(const int mx, const int my, const int button) {
    if (button == sf::Mouse::Button::Left && !_isReloading) {
        fireWeapon();
    }
}

void Player::keyPressed(sf::Keyboard::Key& key) {
    if ((key == sf::Keyboard::W || key == sf::Keyboard::S) && !_verticalMovementKeyIsPressed) {
        _verticalMovementKeyIsPressed = true;
        _lastUpOrDownPressTime = currentTimeMillis();
    }
    if ((key == sf::Keyboard::A || key == sf::Keyboard::D) && !_horizontalMovementKeyIsPressed) {
        _horizontalMovementKeyIsPressed = true;
        _lastLeftOrRightPressTime = currentTimeMillis();
    }
}

void Player::keyReleased(sf::Keyboard::Key& key) {
    if (key == InputBindingManager::getKeyboardBinding(InputBindingManager::BINDABLE_ACTION::RELOAD)) {
        startReloadingWeapon();
    }

    if ((key == sf::Keyboard::W || key == sf::Keyboard::S) && _verticalMovementKeyIsPressed) {
        _verticalMovementKeyIsPressed = false;
    }
    if ((key == sf::Keyboard::A || key == sf::Keyboard::D) && _horizontalMovementKeyIsPressed) {
        _horizontalMovementKeyIsPressed = false;
    }
}

void Player::controllerButtonReleased(GAMEPAD_BUTTON button) {
    if (button == InputBindingManager::getGamepadBinding(InputBindingManager::BINDABLE_ACTION::RELOAD)) startReloadingWeapon();
}

void Player::controllerButtonPressed(GAMEPAD_BUTTON button) {
    if (!_isReloading && button == InputBindingManager::getGamepadBinding(InputBindingManager::BINDABLE_ACTION::SHOOT)) fireWeapon();
}

void Player::gamepadDisconnected() {}

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
        (sf::Mouse::isButtonPressed(sf::Mouse::Left) || GamePad::isButtonPressed(InputBindingManager::getGamepadBinding(InputBindingManager::BINDABLE_ACTION::SHOOT)))
        && !_isReloading &&
        getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL) != NOTHING_EQUIPPED &&
        Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isGun() &&
        Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isAutomatic() &&
        currentTimeMillis() - _lastAutoFireTimeMillis >= Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getFireRateMilliseconds()
        && !isSwimming() && !isDodging()) {
        _lastAutoFireTimeMillis = currentTimeMillis();
        unsigned int id = getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL);
        Item::ITEMS[id]->use(this);

    } else if (!_gamePaused && !_inventoryMenuIsOpen &&
        (sf::Mouse::isButtonPressed(sf::Mouse::Left) || GamePad::isButtonPressed(InputBindingManager::getGamepadBinding(InputBindingManager::BINDABLE_ACTION::SHOOT)))
        && !_isReloading &&
        getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL) != NOTHING_EQUIPPED &&
        Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isGun() &&
        Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isAutomatic()
        && !isSwimming() && !isDodging()) {
        _isFiringAutomaticWeapon = true;
    } else _isFiringAutomaticWeapon = false;
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

            if (getMagazineContents() == 0 && !isReloading()) {
                std::shared_ptr<const Item> weapon = Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)];
                std::shared_ptr<const Item> ammo = Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::AMMO)];
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

    _boatSprite.setTexture(*spriteSheet);
    _boatSprite.setTextureRect(sf::IntRect(0, 544, 3 * TILE_SIZE, 3 * TILE_SIZE));
    _boatSprite.setPosition(sf::Vector2f(getPosition().x - TILE_SIZE, getPosition().y));

    _clothingHeadSprite.setTexture(*spriteSheet); 
    _clothingBodySprite.setTexture(*spriteSheet);
    _clothingLegsSprite.setTexture(*spriteSheet);
    _clothingFeetSprite.setTexture(*spriteSheet);

    _armorHeadSprite.setTexture(*spriteSheet);
    _armorBodySprite.setTexture(*spriteSheet);
    _armorLegsSprite.setTexture(*spriteSheet);
    _armorFeetSprite.setTexture(*spriteSheet);

    _toolSprite.setTexture(*spriteSheet);

    initHairSprites(spriteSheet);
}

void Player::toggleVisible() {
    _isVisible = !_isVisible;
}