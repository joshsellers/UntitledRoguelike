#include "ShopManager.h"
#include "../world/entities/Entity.h"
#include "../core/MessageManager.h"
#include "../core/Tutorial.h"

void ShopManager::setBuyInterface(std::shared_ptr<UIShopInterface> buyInterface) {
    _buyInterface = buyInterface;
}

void ShopManager::setSellInterface(std::shared_ptr<UIShopInterface> sellInterface) {
    _sellInterface = sellInterface;
}

bool ShopManager::buy(int itemId, int amount) {
    int price = Item::ITEMS[itemId]->getValue() * amount;
    if (_sellInterface->getSource().hasItem(Item::PENNY.getId())
        && _sellInterface->getSource().getItemAmountAt(_sellInterface->getSource().findItem(Item::PENNY.getId())) >= price) {
        _sellInterface->addItem(itemId, amount);
        _sellInterface->getSource().removeItem(Item::PENNY.getId(), price);

        Entity* shopKeep = _buyInterface->getSource().getParent();
        unsigned int currentSeed = shopKeep->getPosition().x + shopKeep->getPosition().y * (shopKeep->getPosition().x - shopKeep->getPosition().y);

        unsigned int transactionNumber = 0;
        if (_shopLedger.size() != 0 && _shopLedger[currentSeed].size() != 0) transactionNumber = _shopLedger[currentSeed].size();

        _shopLedger[currentSeed][transactionNumber] = std::make_pair(itemId, -amount);
        _shopLedger[currentSeed][transactionNumber + 1u] = std::make_pair(Item::PENNY.getId(), price);

        if (!Tutorial::isCompleted() 
            && Item::ITEMS[itemId]->getEquipmentType() == EQUIPMENT_TYPE::TOOL
            && !Item::ITEMS[itemId]->isGun()) Tutorial::completeStep(TUTORIAL_STEP::BUY_AXE);

        return true;
    } else {
        MessageManager::displayMessage("You do not have enough pennies to buy this item!", 5);
        return false;
    }
}

bool ShopManager::sell(int itemId, int amount) {
    int price = Item::ITEMS[itemId]->getValue() * amount;
    if (_buyInterface->getSource().hasItem(Item::PENNY.getId())
        && _buyInterface->getSource().getItemAmountAt(_buyInterface->getSource().findItem(Item::PENNY.getId())) >= price) {
        _buyInterface->addItem(itemId, amount);
        _buyInterface->getSource().removeItem(Item::PENNY.getId(), price);

        Entity* shopKeep = _buyInterface->getSource().getParent();
        unsigned int currentSeed = shopKeep->getPosition().x + shopKeep->getPosition().y * (shopKeep->getPosition().x - shopKeep->getPosition().y);

        unsigned int transactionNumber = 0;
        if (_shopLedger.size() != 0 && _shopLedger[currentSeed].size() != 0) transactionNumber = _shopLedger[currentSeed].size();

        _shopLedger[currentSeed][transactionNumber] = std::make_pair(itemId, amount);
        _shopLedger[currentSeed][transactionNumber + 1u] = std::make_pair(Item::PENNY.getId(), -price);
        
        if (!Tutorial::isCompleted() && itemId == Item::WOOD.getId()) Tutorial::completeStep(TUTORIAL_STEP::SELL_WOOD);

        return true;
    } else {
        MessageManager::displayMessage("The Shopkeep does not have enough pennies to buy this item!", 5);
        return false;
    }
}

void ShopManager::clearLedger() {
    _shopLedger.clear();
}

std::map<unsigned int, std::map<unsigned int, std::pair<unsigned int, int>>> ShopManager::getShopLedger() const {
    return _shopLedger;
}

void ShopManager::controllerButtonReleased(GAMEPAD_BUTTON button) {
    switch (button) {
        case GAMEPAD_BUTTON::LEFT_BUMPER:
            _buyInterface->blockControllerInput = false;
            _sellInterface->blockControllerInput = true;
            break;
        case GAMEPAD_BUTTON::RIGHT_BUMPER:
            _buyInterface->blockControllerInput = true;
            _sellInterface->blockControllerInput = false;
            break;
    }
}
