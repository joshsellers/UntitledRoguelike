#include "ShopKeep.h"
#include "../World.h"
#include "../../core/Tutorial.h"

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

void ShopKeep::initInventory() {
    while (getInventory().getCurrentSize() != 0) {
        getInventory().removeItemAt(0, getInventory().getItemAmountAt(0));
    }

    unsigned int seed = _pos.x + _pos.y * (_pos.x - _pos.y);
    srand(seed);

    int pennyCount = randomInt(100, 500);
    getInventory().addItem(Item::PENNY.getId(), pennyCount);

   
    constexpr int itemCount = 10;
    constexpr int maxAttempts = 100;
    int attempts = 0;

    std::vector<unsigned int> availableItems;

    for (const auto& item : Item::ITEMS) {
        if (item->isBuyable()
            && item->getEquipmentType() != EQUIPMENT_TYPE::AMMO
            && !stringStartsWith(item->getName(), "_")
            && item->isUnlocked(getWorld()->getCurrentWaveNumber())
            && item->getId() != Item::PENNY.getId()) {

            if (item->isGun() && getWorld()->getPlayer()->getInventory().hasItem(item->getId())) continue;
            else if (item->getId() == Item::COIN_MAGNET.getId() && getWorld()->getPlayer()->getCoinMagnetCount() == 12) continue;
            availableItems.push_back(item->getId());
        }
    }

    while (getInventory().getCurrentSize() != itemCount + 1) {
        int currentInvSize = getInventory().getCurrentSize();

        for (int i = 0; i < (itemCount + 1) - currentInvSize; i++) {
            unsigned int itemPos = randomInt(0, availableItems.size() - 1);
            const auto& item = Item::ITEMS[availableItems.at(itemPos)];

            if (randomInt(0, item->getShopChance() - 1) == 0) {
                unsigned int itemAmount = 1;
                if (item->isStackable()) itemAmount = randomInt(1, item->getStackLimit());

                getInventory().addItem(item->getId(), itemAmount);
                availableItems.erase(availableItems.begin() + itemPos);
                if (availableItems.size() == 0) break;
            }
        }

        attempts++;
        if (attempts >= maxAttempts && getInventory().getCurrentSize() > 1) break;
        else if (availableItems.size() == 0) break;
    }

    /*for (int i = 0; i < getInventory().getCurrentSize(); i++) {
        std::shared_ptr<const Item> item = Item::ITEMS[getInventory().getItemIdAt(i)];
        if (item->getEquipmentType() == EQUIPMENT_TYPE::CLOTHING_HEAD
            || item->getEquipmentType() == EQUIPMENT_TYPE::CLOTHING_BODY
            || item->getEquipmentType() == EQUIPMENT_TYPE::CLOTHING_LEGS
            || item->getEquipmentType() == EQUIPMENT_TYPE::CLOTHING_FEET) {
            getInventory().equip(i, item->getEquipmentType());
        }
    }*/

    for (unsigned int i = 0; i < _shopManager->getShopLedger()[seed].size(); i++) {
        auto& ledger = _shopManager->getShopLedger()[seed][i];
        unsigned int itemId = ledger.first;
        int amount = ledger.second;

        if (amount > 0) getInventory().addItem(itemId, amount);
        else if (getInventory().hasItem(itemId) && getInventory().getItemAmountAt(getInventory().findItem(itemId)) >= -amount) {
            getInventory().removeItem(itemId, -amount);
        }
    }

    if (!Tutorial::isCompleted()) getInventory().addItem(Item::AXE.getId(), 1);
}

void ShopKeep::update() {
    if (!getWorld()->playerIsInShop()) deactivate();

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void ShopKeep::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);

    drawApparel(_clothingHeadSprite, EQUIPMENT_TYPE::CLOTHING_HEAD, surface);
    drawApparel(_clothingBodySprite, EQUIPMENT_TYPE::CLOTHING_BODY, surface);
    drawApparel(_clothingLegsSprite, EQUIPMENT_TYPE::CLOTHING_LEGS, surface);
    drawApparel(_clothingFeetSprite, EQUIPMENT_TYPE::CLOTHING_FEET, surface);
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

        if (getInventory().getEquippedItemId(equipType) != NOTHING_EQUIPPED) {
            sf::IntRect itemTextureRect = Item::ITEMS[getInventory().getEquippedItemId(equipType)]->getTextureRect();
            int spriteY = itemTextureRect.top + TILE_SIZE;

            sprite.setTextureRect(sf::IntRect(
                itemTextureRect.left + TILE_SIZE * 3 * DOWN, spriteY + yOffset, TILE_SIZE * 3, TILE_SIZE * spriteHeight)
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
                itemTextureRect.left + TILE_SIZE * DOWN, spriteY + yOffset, TILE_SIZE, TILE_SIZE)
            );

            sprite.setPosition(sf::Vector2f(_sprite.getPosition().x, _sprite.getPosition().y + TILE_SIZE));
            surface.draw(sprite);
        }
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
