#include "Inventory.h"
#include "World.h"
#include <iostream>
#include "DroppedItem.h"
#include "MessageManager.h"
#include "MultiplayerManager.h"
#include "Globals.h"

Inventory::Inventory(Entity* parent) : 
    _parent(parent) {
}

void Inventory::addItem(unsigned int itemId, unsigned int amount) {
    if (amount > 0) {
        const Item* itemData = Item::ITEMS[itemId];;

        if (_inventory.size() + amount <= getMaxSize() || itemData->isStackable()) {
            for (auto& item : _inventory) {
                if (item.x == itemId && itemData->isStackable() && item.y + amount <= itemData->getStackLimit()) {
                    item.y += amount;
                    return;
                }
            }

            if (itemData->isStackable() && amount <= itemData->getStackLimit() && _inventory.size() < getMaxSize())
                _inventory.push_back(sf::Vector2u(itemId, amount));
            else if (itemData->isStackable() && amount > itemData->getStackLimit() && _inventory.size() < getMaxSize()) {
                _inventory.push_back(sf::Vector2u(itemId, itemData->getStackLimit()));
                if (_inventory.size() + 1 <= getMaxSize()) addItem(itemId, amount - itemData->getStackLimit());
                else {
                    int excess = amount - itemData->getStackLimit();
                    dropItem(itemId, excess);
                }
            } else if (itemData->isStackable() && _inventory.size() == getMaxSize()) {
                for (auto& item : _inventory) {
                    if (item.x == itemId && itemData->isStackable() && item.y + amount > itemData->getStackLimit()) {
                        if (item.y == itemData->getStackLimit()) continue;
                        else {
                            int excess = item.y + amount - itemData->getStackLimit();
                            dropItem(itemId, excess);
                            item.y += amount - excess;
                            return;
                        }
                    }
                }
                dropItem(itemId, amount);
            } else if (!itemData->isStackable() && _inventory.size() < getMaxSize())
                for (int i = 0; i < amount; i++) _inventory.push_back(sf::Vector2u(itemId, 1));
        } else {
            int excess = _inventory.size() + amount - getMaxSize();
            dropItem(itemId, excess);
            addItem(itemId, amount - excess);
        }

        updateRemoteInventory((std::string)"addItem." + std::to_string(itemId) + (std::string)"," + std::to_string(amount));
    }
}

void Inventory::removeItem(unsigned int itemId, unsigned int amount) {
    if (amount > 0) {
        for (int i = 0; i < _inventory.size(); i++) {
            auto& item = _inventory.at(i);
            if (item.x == itemId) {
                removeItemAt(i, amount);
                return;
            }
        }
    }
}

void Inventory::removeItemAt(unsigned int index, unsigned int amount) {
    if (amount > 0) {
        auto& item = _inventory.at(index);
        item.y -= amount;
        if (item.y <= 0) {
            for (int j = 0; j < EQUIPMENT_SLOT_COUNT; j++) {
                if (_equippedItems[j] == (int)index) { 
                    deEquip((EQUIPMENT_TYPE)j);
                }

                if ((int)index < _equippedItems[j]) _equippedItems[j]--;
            }
            _inventory.erase(_inventory.begin() + index);
        }

        updateRemoteInventory((std::string)"removeItemAt." + std::to_string(index) + (std::string)"," + std::to_string(amount));
    }
}

void Inventory::dropItem(unsigned int itemId, unsigned int amount) {
    float px = _parent->getPosition().x;
    float py = _parent->getPosition().y;

    switch (_parent->getMovingDir()) {
    case UP:
        py -= 20;
        break;
    case DOWN:
        py += 37;
        break;
    case LEFT:
        px -= 20;
        py += 16;
        break;
    case RIGHT:
        px += 20;
        py += 16;
        break;
    }

    sf::Vector2f pos(px, py);
    std::shared_ptr<DroppedItem> droppedItem = std::shared_ptr<DroppedItem>(
        new DroppedItem(pos, 1.f, itemId, amount, Item::ITEMS[itemId]->getTextureRect())
    );
    droppedItem->setWorld(_parent->getWorld());
    droppedItem->loadSprite(_parent->getWorld()->getSpriteSheet());

    _parent->getWorld()->addEntity(droppedItem);

    updateRemoteInventory((std::string)"dropItem." + std::to_string(itemId) + (std::string)"," + std::to_string(amount));
}

bool Inventory::hasItem(unsigned int itemId) const {
    for (auto& item : _inventory) 
        if (item.x == itemId) return true;

    return false;
}

int Inventory::findItem(unsigned int itemId) const {
    for (int i = 0; i < _inventory.size(); i++) {
        if (getItemIdAt(i) == itemId) return i;
    }
    return NO_ITEM;
}

void Inventory::useItem(size_t inventoryIndex) const {
    if (inventoryIndex < _inventory.size()) Item::ITEMS.at(_inventory.at(inventoryIndex).x)->use(_parent);
    else MessageManager::displayMessage("Invalid inventory index: " + std::to_string(inventoryIndex) + ". Inventory size is " + std::to_string(_inventory.size()), 10, WARN);
}

unsigned int Inventory::getItemIdAt(unsigned int index) const {
    return _inventory.at(index).x;
}

unsigned int Inventory::getItemAmountAt(unsigned int index) const {
    return _inventory.at(index).y;
}

void Inventory::equip(int index, EQUIPMENT_TYPE equipType) {
    if (equipType != EQUIPMENT_TYPE::NOT_EQUIPABLE) {
        if (index != NOTHING_EQUIPPED 
            && Item::ITEMS[getItemIdAt(index)]->getEquipmentType() == EQUIPMENT_TYPE::TOOL
            && getEquippedItemId(EQUIPMENT_TYPE::TOOL) != NOTHING_EQUIPPED
            && Item::ITEMS[getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->isGun()) emptyAmmoMagazine(equipType);
        _equippedItems[(int)equipType] = index;

        updateRemoteInventory((std::string)"equip." + std::to_string(index) + (std::string)"," + std::to_string((int)equipType));
    }
}

void Inventory::deEquip(EQUIPMENT_TYPE equipType) {
    emptyAmmoMagazine(equipType);
    equip(NOTHING_EQUIPPED, equipType);
}

bool Inventory::isEquipped(int index) const {
    for (int i : _equippedItems) 
        if (i == index) return true;
    return false;
}

int Inventory::getEquippedItemId(EQUIPMENT_TYPE equipType) const {
    int equippedIndex = _equippedItems[(int)equipType];
    if (equippedIndex != NOTHING_EQUIPPED)
        return getItemIdAt(equippedIndex);
    return NOTHING_EQUIPPED;
}

int Inventory::getEquippedIndex(EQUIPMENT_TYPE equipType) const {
    return _equippedItems[(int)equipType];
}

void Inventory::setMaxSize(unsigned int maxSize) {
    _maxSize = maxSize;
}

unsigned int Inventory::getMaxSize() const {
    return _maxSize;
}

unsigned int Inventory::getCurrentSize() const {
    return _inventory.size();
}

Entity* Inventory::getParent() const {
    return _parent;
}

void Inventory::emptyAmmoMagazine(EQUIPMENT_TYPE equipType) {
    if (getEquippedItemId(equipType) != NOTHING_EQUIPPED && Item::ITEMS[getEquippedItemId(equipType)]->isGun()) {
        const Item* weapon = Item::ITEMS[getEquippedItemId(equipType)];
        addItem(weapon->getAmmoId(), _parent->getMagazineContents());
        _parent->emptyMagazine();

        //don't think we need to do this bug if theres bugs with ammo stuff it probably has to do with this
        //updateRemoteInventory("emptyAmmoMagazine." + std::to_string((int)equipType));
    }
}

void Inventory::updateRemoteInventory(std::string data) {
    if (_parent->shouldSendMultiplayerInventoryUpdates() && IS_MULTIPLAYER_CONNECTED) {
        int times = 0;
        for (auto& peer : Multiplayer::manager.getConnectedPeers()) {
            Multiplayer::manager.sendMessage(MultiplayerMessage(PayloadType::INVENTORY_DATA, data), peer);
            times++;
        }
    }
}
