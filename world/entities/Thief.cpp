#include "Thief.h"
#include "../World.h"

Thief::Thief(sf::Vector2f pos, bool chasing) : Entity(THIEF, pos, 6.f, TILE_SIZE, TILE_SIZE * 2, false) {
    _chasing = chasing;
    setMaxHitPoints(20);
    heal(getMaxHitPoints());

    getInventory().addItem(Item::getIdFromName("Ski Mask"), 1);
    getInventory().addItem(Item::getIdFromName("Tank Top"), 1);
    getInventory().addItem(Item::getIdFromName("Jorts"), 1);
    getInventory().addItem(Item::getIdFromName("White Tennis Shoes"), 1);

    getInventory().equip(getInventory().findItem(Item::getIdFromName("Ski Mask")), EQUIPMENT_TYPE::CLOTHING_HEAD);
    getInventory().equip(getInventory().findItem(Item::getIdFromName("Tank Top")), EQUIPMENT_TYPE::CLOTHING_BODY);
    getInventory().equip(getInventory().findItem(Item::getIdFromName("Jorts")), EQUIPMENT_TYPE::CLOTHING_LEGS);
    getInventory().equip(getInventory().findItem(Item::getIdFromName("White Tennis Shoes")), EQUIPMENT_TYPE::CLOTHING_FEET);

    _hitBoxXOffset = static_cast<float>(-TILE_SIZE) / 2;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE;
    _hitBox.height = TILE_SIZE * 2;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _isMob = true;
    _isEnemy = true;
}

void Thief::update() {
    float xa = 0.f, ya = 0.f;

    const sf::Vector2f goalPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    const sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 2);

    if (_chasing) {
        if (getWorld()->getPlayer()->isActive() && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())) {
            _chasing = false;
            if (!getWorld()->getPlayer()->getInventory().hasItem(Item::PENNY.getId())) return;

            const unsigned int playerPennyAmt = getWorld()->getPlayer()->getInventory().getItemAmountAt(getWorld()->getPlayer()->getInventory().findItem(Item::PENNY.getId()));
            const unsigned int stealAmount = playerPennyAmt <= 100 ? playerPennyAmt : ((float)randomInt(25, 50) / 100.f) * (float)playerPennyAmt;
            getWorld()->getPlayer()->getInventory().removeItem(Item::PENNY.getId(), stealAmount);
            getInventory().addItem(Item::PENNY.getId(), stealAmount);
            _pennyAmt = stealAmount;
            MessageManager::displayMessage("You've been robbed!", 5);
            return;
        }

        if (goalPos.y < cLoc.y) {
            ya--;
        } else if (goalPos.y > cLoc.y) {
            ya++;
        }

        if (goalPos.x < cLoc.x) {
            xa--;
        } else if (goalPos.x > cLoc.x) {
            xa++;
        }
    } else {
        const float distSquared = std::pow(goalPos.x - cLoc.x, 2) + std::pow(goalPos.y - cLoc.y, 2);
        const float maxDist = 500 * 500;

        if (distSquared >= maxDist) deactivate();
        else {
            if (goalPos.y < cLoc.y) {
                ya++;
            } else if (goalPos.y > cLoc.y) {
                ya--;
            }

            if (goalPos.x < cLoc.x) {
                xa++;
            } else if (goalPos.x > cLoc.x) {
                xa--;
            }
        }
    }

    if (xa && ya) {
        xa *= 0.785398;
        ya *= 0.785398;
    }

    hoardMove(xa, ya, true);

    _sprite.setPosition(getPosition());

    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE;
        _hitBox.height = TILE_SIZE * 2 / 2;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 2;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void Thief::draw(sf::RenderTexture& surface) {
    const TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        sf::Vector2f(((int)_pos.x + PLAYER_WIDTH / 2), ((int)_pos.y + PLAYER_HEIGHT))
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2));

        int xOffset = ((_numSteps >> _animSpeed) & 1) * 16;

        _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, 16, 16));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2 + 8));
        surface.draw(_wavesSprite);
    }

    int xOffset = 0;
    int yOffset = isMoving() || (isSwimming()) ? ((_numSteps >> _animSpeed) & 3) * 32 : 0;
    _sprite.setTextureRect(sf::IntRect(
        16 * _movingDir, yOffset,
        16,
        terrainType == TERRAIN_TYPE::WATER ? 16 : 32)
    );

    surface.draw(_sprite);
    drawEquipables(surface);
}

void Thief::drawEquipables(sf::RenderTexture& surface) {
    drawApparel(_clothingHeadSprite, EQUIPMENT_TYPE::CLOTHING_HEAD, surface);

    if (!isSwimming()) {
        drawApparel(_clothingBodySprite, EQUIPMENT_TYPE::CLOTHING_BODY, surface);
        drawApparel(_clothingLegsSprite, EQUIPMENT_TYPE::CLOTHING_LEGS, surface);
        drawApparel(_clothingFeetSprite, EQUIPMENT_TYPE::CLOTHING_FEET, surface);
    }
}

void Thief::drawApparel(sf::Sprite& sprite, EQUIPMENT_TYPE equipType, sf::RenderTexture& surface) {
    if (equipType == EQUIPMENT_TYPE::CLOTHING_HEAD) {
        constexpr int spriteHeight = 3;
        int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * spriteHeight : 0;

        if (getInventory().getEquippedItemId(equipType) != NOTHING_EQUIPPED) {
            sf::IntRect itemTextureRect = Item::ITEMS[getInventory().getEquippedItemId(equipType)]->getTextureRect();
            int spriteY = itemTextureRect.top + TILE_SIZE;

            sprite.setTextureRect(sf::IntRect(
                itemTextureRect.left + TILE_SIZE * 3 * _movingDir, spriteY + yOffset, TILE_SIZE * 3, TILE_SIZE * spriteHeight)
            );

            sprite.setPosition(sf::Vector2f(_sprite.getPosition().x, _sprite.getPosition().y - TILE_SIZE));
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

void Thief::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        if (getInventory().hasItem(Item::PENNY.getId())) {
            getInventory().dropItem(Item::PENNY.getId(), getInventory().getItemAmountAt(getInventory().findItem(Item::PENNY.getId())));
        }
    }
}

std::string Thief::getSaveData() const {
    return std::to_string(_pennyAmt);
}

void Thief::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(0, 0, 16, 32));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE / 2.f, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2));
    _wavesSprite.setOrigin((float)TILE_SIZE / 2.f, 0);

    _clothingHeadSprite.setTexture(*spriteSheet);
    _clothingHeadSprite.setOrigin((float)TILE_SIZE * 3 / 2.f, 0);
    _clothingBodySprite.setTexture(*spriteSheet);
    _clothingBodySprite.setOrigin((float)TILE_SIZE / 2.f, 0);
    _clothingLegsSprite.setTexture(*spriteSheet);
    _clothingLegsSprite.setOrigin((float)TILE_SIZE / 2.f, 0);
    _clothingFeetSprite.setTexture(*spriteSheet);
    _clothingFeetSprite.setOrigin((float)TILE_SIZE / 2.f, 0);

    if (!_chasing && _pennyAmt == 0)
        _pennyAmt = getInventory().getItemAmountAt(getInventory().findItem(Item::PENNY.getId()));
}