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

    std::vector<int> availableItems;
    for (const auto& item : Item::ITEMS) {
        if (item->isUnlocked(getWorld()->getCurrentWaveNumber())) {
            availableItems.push_back(item->getId());
        }
    }

    int pennyCount = randomInt(5000, 10000);
    getInventory().addItem(Item::PENNY.getId(), pennyCount);

    int itemCount = randomInt(5, 50);
    for (int i = 0; i < itemCount; i++) {
        int itemAmount = randomInt(1, 100);
        //int itemId = randomInt(0, Item::ITEMS.size() - 1);
        int itemId = availableItems[randomInt(0, availableItems.size() - 1)];
        for (const auto& item : Item::ITEMS) {
            if (item->isGun() && item->getAmmoId() == itemId) {
                itemAmount += 100;
                itemAmount *= 5;
            }
        }

        if (stringStartsWith(Item::ITEMS[itemId]->getName(), "_") 
            || !Item::ITEMS[itemId]->isBuyable() 
            || !Item::ITEMS[itemId]->isUnlocked(getWorld()->getCurrentWaveNumber())) continue;

        if (!Item::ITEMS[itemId]->isStackable()) itemAmount = 1;
        else itemAmount = std::min((int)Item::ITEMS[itemId]->getStackLimit(), itemAmount);

        getInventory().addItem(itemId, itemAmount);
    }

    // Give ammo for each gun if we don't have any already
    std::vector<unsigned int> ammoNeeded;
    for (int i = 0; i < getInventory().getCurrentSize(); i++) {
        std::shared_ptr<const Item> item = Item::ITEMS[getInventory().getItemIdAt(i)];
        if (item->isGun()) {
            bool hasAmmo = false;
            for (int j = 0; j < getInventory().getCurrentSize(); j++) {
                if (item->getAmmoId() == getInventory().getItemIdAt(j)) {
                    hasAmmo = true;
                    break;
                }
            }

            if (!hasAmmo) {
                ammoNeeded.push_back(item->getAmmoId());
            }
        }
    }

    for (unsigned int ammoId : ammoNeeded) {
        int ammoCount = randomInt((5 + 100) * 5, (100 + 100) * 5);
        getInventory().addItem(ammoId, ammoCount);
    }
    //

    for (int i = 0; i < getInventory().getCurrentSize(); i++) {
        std::shared_ptr<const Item> item = Item::ITEMS[getInventory().getItemIdAt(i)];
        if (item->getEquipmentType() == EQUIPMENT_TYPE::CLOTHING_HEAD
            || item->getEquipmentType() == EQUIPMENT_TYPE::CLOTHING_BODY
            || item->getEquipmentType() == EQUIPMENT_TYPE::CLOTHING_LEGS
            || item->getEquipmentType() == EQUIPMENT_TYPE::CLOTHING_FEET) {
            getInventory().equip(i, item->getEquipmentType());
        }
    }

    for (unsigned int i = 0; i < _shopManager->getShopLedger()[seed].size(); i++) {
        auto& ledger = _shopManager->getShopLedger()[seed][i];
        unsigned int itemId = ledger.first;
        int amount = ledger.second;

        if (amount > 0) getInventory().addItem(itemId, amount);
        else getInventory().removeItem(itemId, -amount);
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
