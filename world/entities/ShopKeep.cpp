#include "ShopKeep.h"
#include "../World.h"
#include "../../core/Tutorial.h"
#include "ShopKeepCorpse.h"
#include "../../statistics/AchievementManager.h"
#include "../../inventory/abilities/AbilityManager.h"
#include "../../inventory/abilities/Ability.h"
#include "../../statistics/StatManager.h"

ShopKeep::ShopKeep(sf::Vector2f pos, ShopManager* shopManager, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 96, 48, false) {
    loadSprite(spriteSheet);
    _shopManager = shopManager;

    _entityType = "shopkeep";

    _hitBoxXOffset = -TILE_SIZE;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 3;
    _hitBox.height = TILE_SIZE * 4;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    setMaxHitPoints(10000000);
    heal(getMaxHitPoints());

    deactivate();
}

void ShopKeep::initInventory(bool visited) {
    while (getInventory().getCurrentSize() != 0) {
        getInventory().removeItemAt(0, getInventory().getItemAmountAt(0));
    }

    for (int i = 0; i < 4; i++) {
        _equippedApparel[i] = NOTHING_EQUIPPED;
    }

    unsigned int seed = _pos.x + _pos.y * (_pos.x - _pos.y);
    srand(seed);

    int pennyCount = randomInt(100, 500);
    getInventory().addItem(Item::PENNY.getId(), pennyCount);

    // apparel
    std::vector<std::vector<int>> clothingOptions = {
        {NOTHING_EQUIPPED}, {NOTHING_EQUIPPED},
        {NOTHING_EQUIPPED}, {NOTHING_EQUIPPED}
    };

    for (const auto& item : Item::ITEMS) {
        if (item->getEquipmentType() != EQUIPMENT_TYPE::NOT_EQUIPABLE && item->getEquipmentType() < EQUIPMENT_TYPE::ARMOR_HEAD && item->isBuyable()
            && item->getId() != Item::getIdFromName("Leaf Hat") && (item->getId() != Item::getIdFromName("Ski Mask"))) {
            clothingOptions.at((int)item->getEquipmentType()).push_back(item->getId());
        }
    }

    for (int i = 0; i < clothingOptions.size(); i++) {
        constexpr float skipChance = 0.25f;
        if (randomChance(skipChance)) continue;
        _equippedApparel[i] = clothingOptions.at(i).at((size_t)randomInt(0, clothingOptions.at(i).size() - 1));
    }

   
    if (!visited) {
        int additionalCapacity = 0;
        if (AbilityManager::playerHasAbility(Ability::ORDER_FORM.getId())) {
            additionalCapacity = (int)AbilityManager::getParameter(Ability::ORDER_FORM.getId(), "capacity");
        }

        const int itemCount = 10 + additionalCapacity;
        constexpr int maxAttempts = 100;
        int attempts = 0;

        std::vector<unsigned int> availableItems;

        for (const auto& item : Item::ITEMS) {
            if (item->isBuyable()
                && item->getEquipmentType() != EQUIPMENT_TYPE::AMMO
                && !stringStartsWith(item->getName(), "_")
                && item->isUnlocked(StatManager::getOverallStat(HIGHEST_WAVE_REACHED))
                && item->getId() != Item::PENNY.getId()) {

                const EQUIPMENT_TYPE equipType = item->getEquipmentType();
                bool isClothing = equipType == EQUIPMENT_TYPE::CLOTHING_HEAD
                    || equipType == EQUIPMENT_TYPE::CLOTHING_BODY
                    || equipType == EQUIPMENT_TYPE::CLOTHING_LEGS
                    || equipType == EQUIPMENT_TYPE::CLOTHING_FEET
                    || equipType == EQUIPMENT_TYPE::ARMOR_HEAD
                    || equipType == EQUIPMENT_TYPE::ARMOR_BODY
                    || equipType == EQUIPMENT_TYPE::ARMOR_LEGS
                    || equipType == EQUIPMENT_TYPE::ARMOR_FEET;
                bool isBoat = equipType == EQUIPMENT_TYPE::BOAT;

                if ((item->isGun() || isClothing || isBoat) && getWorld()->getPlayer()->getInventory().hasItem(item->getId())) continue;
                else if (item->getId() == Item::COIN_MAGNET.getId() && getWorld()->getPlayer()->getCoinMagnetCount() == 12) continue;
                if (item->getId() == Item::getIdFromName("Rebound Jewel") && AbilityManager::playerHasAbility(Ability::BOUNCING_PROJECTILES.getId())) continue;
                else if (item->getId() == Item::getIdFromName("Burst Jewel") && AbilityManager::playerHasAbility(Ability::EXPLOSIVE_ROUNDS.getId())) continue;
                else if (item->getId() == Item::getIdFromName("Bloat Jewel") && AbilityManager::playerHasAbility(Ability::BIG_BULLETS.getId())) continue;
                else if (item->getId() == Item::getIdFromName("Seek Jewel") && AbilityManager::playerHasAbility(Ability::TARGET_SEEKING_BULLETS.getId())) continue;
                else if (item->getId() == Item::getIdFromName("Map") && getWorld()->getPlayer()->getInventory().hasItem(Item::getIdFromName("Map"))) continue;

                availableItems.push_back(item->getId());
            }
        }

        while (getInventory().getCurrentSize() != itemCount + 1) {
            int currentInvSize = getInventory().getCurrentSize();

            for (int i = 0; i < (itemCount + 1) - currentInvSize; i++) {
                unsigned int itemPos = randomInt(0, availableItems.size() - 1);
                const auto& item = Item::ITEMS[availableItems.at(itemPos)];

                float spawnChance = item->getShopChance() / 100.f;
                const EQUIPMENT_TYPE equipType = item->getEquipmentType();
                if ((equipType == EQUIPMENT_TYPE::CLOTHING_HEAD
                    || equipType == EQUIPMENT_TYPE::CLOTHING_BODY
                    || equipType == EQUIPMENT_TYPE::CLOTHING_LEGS
                    || equipType == EQUIPMENT_TYPE::CLOTHING_FEET) && spawnChance == 1.f) spawnChance -= 0.67f;

                if (randomChance(spawnChance)) {
                    unsigned int itemAmount = 1;
                    if (item->isStackable()) itemAmount = randomInt(1, item->getStackLimit());

                    getInventory().addItem(item->getId(), itemAmount);
                    _shopManager->addItemToInitialInventory(seed, item->getId(), itemAmount);
                    availableItems.erase(availableItems.begin() + itemPos);
                    if (availableItems.size() == 0) break;
                }
            }

            attempts++;
            if (attempts >= maxAttempts && getInventory().getCurrentSize() > 1) break;
            else if (availableItems.size() == 0) break;
        }
    } else {
        const std::vector<std::pair<unsigned int, unsigned int>>& initialInventory = _shopManager->getInitialInventory(seed);

        for (const auto& item : initialInventory) {
            getInventory().addItem(item.first, item.second);
        }

        unsigned int currentInvSize = getInventory().getCurrentSize();
        for (int i = 0; i < currentInvSize; i++) {
            const auto& item = Item::ITEMS[getInventory().getItemIdAt(i)];

            const EQUIPMENT_TYPE equipType = item->getEquipmentType();
            bool isClothing = equipType == EQUIPMENT_TYPE::CLOTHING_HEAD
                || equipType == EQUIPMENT_TYPE::CLOTHING_BODY
                || equipType == EQUIPMENT_TYPE::CLOTHING_LEGS
                || equipType == EQUIPMENT_TYPE::CLOTHING_FEET;
            bool isBoat = equipType == EQUIPMENT_TYPE::BOAT;

            bool remove = false;
            if ((item->isGun() || isClothing || isBoat) && getWorld()->getPlayer()->getInventory().hasItem(item->getId())) remove = true;
            else if (item->getId() == Item::COIN_MAGNET.getId() && getWorld()->getPlayer()->getCoinMagnetCount() == 12) remove = true;
            if (item->getId() == Item::getIdFromName("Rebound Jewel") && AbilityManager::playerHasAbility(Ability::BOUNCING_PROJECTILES.getId())) remove = true;
            else if (item->getId() == Item::getIdFromName("Map") && getWorld()->getPlayer()->getInventory().hasItem(Item::getIdFromName("Map"))) remove = true;

            if (remove) {
                const unsigned int itemIndex = getInventory().findItem(item->getId());
                if (itemIndex == NO_ITEM) {
                    MessageManager::displayMessage("findItem returned NO_ITEM for item ID " + std::to_string(item->getId()), 5, WARN);
                    continue;
                }
                getInventory().removeItemAt(itemIndex, getInventory().getItemAmountAt(itemIndex));
                currentInvSize = getInventory().getCurrentSize();
                i--;
            }
        }
    }

    // ledger
    for (unsigned int i = 0; i < _shopManager->getShopLedger()[seed].size(); i++) {
        auto& ledger = _shopManager->getShopLedger()[seed][i];
        unsigned int itemId = ledger.first;
        int amount = ledger.second;

        if (amount > 0) getInventory().addItem(itemId, amount);
        else if (getInventory().hasItem(itemId) && getInventory().getItemAmountAt(getInventory().findItem(itemId)) >= -amount) {
            getInventory().removeItem(itemId, -amount);
        }
    }
    //

    // discount
    float discountChance = 0.25f;
    if (AbilityManager::playerHasAbility(Ability::COUPON.getId())) {
        discountChance += AbilityManager::getParameter(Ability::COUPON.getId(), "chance");
    }
    if (randomChance(discountChance)) {
        const unsigned int itemId = getInventory().getItemIdAt(randomInt(0, getInventory().getCurrentSize() - 1));
        constexpr float freeChance = 0.02f;
        const float discountAmount = randomChance(freeChance) ? 1.f : ((float)randomInt(25, 90) / 100.f);

        if (itemId != Item::PENNY.getId()) {
            _shopManager->setDiscount(seed, itemId, discountAmount);
        } else {
            _shopManager->setDiscount(seed, 0, 0.f);
        }
    } else {
        _shopManager->setDiscount(seed, 0, 0.f);
    }
    //

    if (!Tutorial::isCompleted() && !getInventory().hasItem(Item::BOW.getId())) getInventory().addItem(Item::BOW.getId(), 1);
    if (!Tutorial::isCompleted() && getInventory().hasItem(Item::AXE.getId())) getInventory().removeItem(Item::AXE.getId(), 1);
}

void ShopKeep::update() {
    if (!getWorld()->playerIsInShop()) deactivate();

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void ShopKeep::draw(sf::RenderTexture& surface) {
    if (_isActive) {
        surface.draw(_sprite);

        drawApparel(_clothingHeadSprite, EQUIPMENT_TYPE::CLOTHING_HEAD, surface);
        drawApparel(_clothingBodySprite, EQUIPMENT_TYPE::CLOTHING_BODY, surface);
        drawApparel(_clothingLegsSprite, EQUIPMENT_TYPE::CLOTHING_LEGS, surface);
        drawApparel(_clothingFeetSprite, EQUIPMENT_TYPE::CLOTHING_FEET, surface);
    }
}

void ShopKeep::setPosition(sf::Vector2f pos) {
    _pos = pos;
    _sprite.setPosition(getPosition()); 
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void ShopKeep::drawApparel(sf::Sprite& sprite, EQUIPMENT_TYPE equipType, sf::RenderTexture& surface) {
    if (equipType == EQUIPMENT_TYPE::CLOTHING_HEAD || equipType == EQUIPMENT_TYPE::ARMOR_HEAD) {
        int spriteHeight = 3;
        int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * spriteHeight : 0;

        if ((int)equipType < 4 && (int)equipType > -1 && _equippedApparel[(int)equipType] != NOTHING_EQUIPPED) {
            sf::IntRect itemTextureRect = Item::ITEMS[_equippedApparel[(int)equipType]]->getTextureRect();
            int spriteY = itemTextureRect.top + TILE_SIZE;

            sprite.setTextureRect(sf::IntRect(
                itemTextureRect.left + TILE_SIZE * 3 * DOWN, spriteY + yOffset, TILE_SIZE * 3, TILE_SIZE * spriteHeight)
            );

            sprite.setPosition(sf::Vector2f(_sprite.getPosition().x - TILE_SIZE, _sprite.getPosition().y - TILE_SIZE));
            surface.draw(sprite);
        }
    } else {
        int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE : 0;

        if ((int)equipType < 4 && (int)equipType > -1 && _equippedApparel[(int)equipType] != NOTHING_EQUIPPED) {
            sf::IntRect itemTextureRect = Item::ITEMS[_equippedApparel[(int)equipType]]->getTextureRect();
            int spriteY = itemTextureRect.top;

            sprite.setTextureRect(sf::IntRect(
                itemTextureRect.left + TILE_SIZE * DOWN, spriteY + yOffset, TILE_SIZE, TILE_SIZE)
            );

            sprite.setPosition(sf::Vector2f(_sprite.getPosition().x, _sprite.getPosition().y + TILE_SIZE));
            surface.draw(sprite);
        }
    }
}

void ShopKeep::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;

        getWorld()->shopKeepKilled(_pos.x + _pos.y * (_pos.x - _pos.y));

        sf::Vector2f corpsePos(getPosition().x - 8, getPosition().y + 10);
        std::shared_ptr<ShopKeepCorpse> corpse = std::shared_ptr<ShopKeepCorpse>(new ShopKeepCorpse(corpsePos, getWorld()->getSpriteSheet()));
        corpse->setWorld(getWorld());
        getWorld()->addEntity(corpse);

        AchievementManager::unlock(HEARTBREAKER);
    }
}

void ShopKeep::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(0, 0, 16, 32)
    );
    _sprite.setPosition(getPosition());

    _clothingHeadSprite.setTexture(*spriteSheet);
    _clothingBodySprite.setTexture(*spriteSheet);
    _clothingLegsSprite.setTexture(*spriteSheet);
    _clothingFeetSprite.setTexture(*spriteSheet);
}
