#include "Soldier.h"
#include "../World.h"
#include "../../core/ShaderManager.h"

Soldier::Soldier(sf::Vector2f pos) : Entity(SOLDIER, pos, 1.f, TILE_SIZE, TILE_SIZE * 2, false), 
    _weaponId(Item::getIdFromName("_SNIPER_SOLDIER"))
{
    setMaxHitPoints(100);
    heal(getMaxHitPoints()); 
    
    _hitBoxXOffset = 0;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE;
    _hitBox.height = TILE_SIZE * 2;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    getInventory().addItem(_weaponId, 1);
    getInventory().equip(getInventory().findItem(_weaponId), EQUIPMENT_TYPE::TOOL);
    _magazineContents = 1;
    _magazineAmmoType = Item::ITEMS[_weaponId]->getAmmoId();

    _useDefaultDamageIndicator = false;
    _canPickUpItems = false;
    _isMob = true;
    _isEnemy = true;

    _entityType = "soldier";

    _equippedApparel[0] = Item::getIdFromName("Military Helmet");
    _equippedApparel[1] = Item::getIdFromName("Military Vest");
    _equippedApparel[2] = Item::getIdFromName("Military Briefs");
    _equippedApparel[3] = Item::getIdFromName("Military Boots");
}

void Soldier::update() {
    if (!_isActive) return;

    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x + (float)PLAYER_WIDTH / 2.f), ((int)getPosition().y) + 32);

    float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
    float desiredDist = 128.f;
    float distanceRatio = desiredDist / dist;

    sf::Vector2f goalPos((1.f - distanceRatio) * playerPos.x + distanceRatio * cLoc.x, (1.f - distanceRatio) * playerPos.y + distanceRatio * cLoc.y);

    float xa = 0.f, ya = 0.f;
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

    if (xa && ya) {
        xa *= 0.785398;
        ya *= 0.785398;
    }

    hoardMove(xa, ya, false);

    _sprite.setPosition(getPosition()); 
    
    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE;
        _hitBox.width = TILE_SIZE;
        _hitBox.height = TILE_SIZE;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.width = TILE_SIZE;
        _hitBox.height = TILE_SIZE * 2;
    }

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    const long long fireRate = Item::ITEMS.at(getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL))->getFireRateMilliseconds();
    if (currentTimeMillis() - _lastFireTime >= (Item::ITEMS.at(_weaponId)->isAutomatic() ? fireRate : Item::ITEMS.at(_weaponId)->getReloadTimeMilliseconds())) {
        getInventory().useItem(getInventory().getEquippedIndex(EQUIPMENT_TYPE::TOOL));
        _lastFireTime = currentTimeMillis();
        _shotsFired++;
    }

    if (Item::ITEMS.at(_weaponId)->isAutomatic() && _shotsFired == Item::ITEMS.at(_weaponId)->getMagazineSize()) {
        _shotsFired = 0;
        _lastFireTime += Item::ITEMS.at(_weaponId)->getReloadTimeMilliseconds();
    }
}

void Soldier::draw(sf::RenderTexture& surface) {
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
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2 + 8));
        surface.draw(_wavesSprite);
    }

    int xOffset = 0;
    int yOffset = isMoving() || (isSwimming()) ? ((_numSteps >> _animSpeed) & 3) * 32 : 0;
    _sprite.setTextureRect(sf::IntRect(
        16 * _facingDir, (0) +
        (0 + yOffset),
        16,
        terrainType == TERRAIN_TYPE::WATER ? 16 : 32)
    );

    if (_facingDir == UP || _facingDir == LEFT) {
        drawWeapon(surface);

        surface.draw(_sprite, isTakingDamage() ? ShaderManager::getShader("damage_frag") : sf::RenderStates::Default);
        drawApparel(_clothingHeadSprite, EQUIPMENT_TYPE::CLOTHING_HEAD, surface);
        drawApparel(_clothingBodySprite, EQUIPMENT_TYPE::CLOTHING_BODY, surface);
        drawApparel(_clothingLegsSprite, EQUIPMENT_TYPE::CLOTHING_LEGS, surface);
        drawApparel(_clothingFeetSprite, EQUIPMENT_TYPE::CLOTHING_FEET, surface);
    } else if (_facingDir == DOWN || _facingDir == RIGHT) {
        surface.draw(_sprite, isTakingDamage() ? ShaderManager::getShader("damage_frag") : sf::RenderStates::Default);
        drawApparel(_clothingHeadSprite, EQUIPMENT_TYPE::CLOTHING_HEAD, surface);
        drawApparel(_clothingBodySprite, EQUIPMENT_TYPE::CLOTHING_BODY, surface);
        drawApparel(_clothingLegsSprite, EQUIPMENT_TYPE::CLOTHING_LEGS, surface);
        drawApparel(_clothingFeetSprite, EQUIPMENT_TYPE::CLOTHING_FEET, surface);

        drawWeapon(surface);
    }
}

void Soldier::decrementMagazine() {}

void Soldier::drawWeapon(sf::RenderTexture& surface) {
    if (getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL) != NOTHING_EQUIPPED) {
        sf::RectangleShape meleeHitBoxDisplay;
        sf::RectangleShape barrelDisplay;
        std::shared_ptr<const Item> equippedTool = Item::ITEMS[getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)];
        sf::IntRect itemTextureRect =
            equippedTool->getTextureRect();
        int spriteX = itemTextureRect.left + TILE_SIZE;
        int spriteY = itemTextureRect.top;

        _toolSprite.setTextureRect(sf::IntRect(
            spriteX, spriteY, TILE_SIZE * 3, TILE_SIZE * 3
        ));

        _toolSprite.setOrigin(sf::Vector2f(_toolSprite.getLocalBounds().width / 2, _toolSprite.getLocalBounds().height));

        sf::Vector2f handPos = sf::Vector2f(getPosition().x, getPosition().y + 23);
        sf::Vector2f mPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);

        sf::Vector2f center = handPos;

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

        surface.draw(_toolSprite);

        if (getWorld()->showDebug()) {
            surface.draw(meleeHitBoxDisplay);
            surface.draw(barrelDisplay);
        }
    }
}

void Soldier::drawApparel(sf::Sprite& sprite, EQUIPMENT_TYPE equipType, sf::RenderTexture& surface) {
    if (equipType == EQUIPMENT_TYPE::CLOTHING_HEAD || equipType == EQUIPMENT_TYPE::ARMOR_HEAD) {
        int spriteHeight = 3;
        int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * spriteHeight : 0;

        if ((int)equipType < 4 && (int)equipType > -1 && _equippedApparel[(int)equipType] != NOTHING_EQUIPPED) {
            sf::IntRect itemTextureRect = Item::ITEMS[_equippedApparel[(int)equipType]]->getTextureRect();
            int spriteY = itemTextureRect.top + TILE_SIZE;

            sprite.setTextureRect(sf::IntRect(
                itemTextureRect.left + TILE_SIZE * 3 * _facingDir, spriteY + yOffset, TILE_SIZE * 3, TILE_SIZE * spriteHeight)
            );

            sprite.setPosition(sf::Vector2f(_sprite.getPosition().x - TILE_SIZE, _sprite.getPosition().y - TILE_SIZE));
            surface.draw(sprite, isTakingDamage() ? ShaderManager::getShader("damage_frag") : sf::RenderStates::Default);
        }
    } else {
        int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE : 0;

        if ((int)equipType < 4 && (int)equipType > -1 && _equippedApparel[(int)equipType] != NOTHING_EQUIPPED) {
            sf::IntRect itemTextureRect = Item::ITEMS[_equippedApparel[(int)equipType]]->getTextureRect();
            int spriteY = itemTextureRect.top;

            sprite.setTextureRect(sf::IntRect(
                itemTextureRect.left + TILE_SIZE * _facingDir, spriteY + yOffset, TILE_SIZE, TILE_SIZE)
            );

            sprite.setPosition(sf::Vector2f(_sprite.getPosition().x, _sprite.getPosition().y + TILE_SIZE));
            surface.draw(sprite, isTakingDamage() ? ShaderManager::getShader("damage_frag") : sf::RenderStates::Default);
        }
    }
}

void Soldier::damage(int damage) {
    _hitPoints -= damage;

    if (_hitPoints <= 0) {
        _isActive = false; 
        
        getInventory().removeItem(getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL), 1);

        srand(currentTimeMillis());
        constexpr float DROP_ARMOR_CHANCE = 0.02f;
        const bool dropHelmet = randomChance(DROP_ARMOR_CHANCE);
        const bool dropVest = randomChance(DROP_ARMOR_CHANCE);
        const bool dropPants = randomChance(DROP_ARMOR_CHANCE);
        const bool dropBoots = randomChance(DROP_ARMOR_CHANCE);

        const bool dropWeapon = randomChance(0.01f);
        if (dropWeapon) getInventory().addItem(Item::getIdFromName("Sniper Rifle"), 1);

        if (dropHelmet) getInventory().addItem(Item::getIdFromName("Military Helmet"), 1);
        if (dropVest) getInventory().addItem(Item::getIdFromName("Military Vest"), 1);
        if (dropPants) getInventory().addItem(Item::getIdFromName("Military Briefs"), 1);
        if (dropBoots) getInventory().addItem(Item::getIdFromName("Military Boots"), 1);

        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}

void Soldier::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(0, 0, 16, 32));
    _sprite.setPosition(getPosition()); 
    //_sprite.setOrigin((float)TILE_SIZE / 2.f, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2));

    _toolSprite.setTexture(*spriteSheet);

    _clothingHeadSprite.setTexture(*spriteSheet);
    _clothingBodySprite.setTexture(*spriteSheet);
    _clothingLegsSprite.setTexture(*spriteSheet);
    _clothingFeetSprite.setTexture(*spriteSheet);
}
