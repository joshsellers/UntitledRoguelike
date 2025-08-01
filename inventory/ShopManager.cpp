#include "ShopManager.h"
#include "../world/entities/Entity.h"
#include "../core/MessageManager.h"
#include "../core/Tutorial.h"
#include "../statistics/StatManager.h"

void ShopManager::setBuyInterface(std::shared_ptr<UIShopInterface> buyInterface) {
    _buyInterface = buyInterface;
}

void ShopManager::setSellInterface(std::shared_ptr<UIShopInterface> sellInterface) {
    _sellInterface = sellInterface;
}

bool ShopManager::buy(int itemId, int amount) {
    float discount = 0.f;
    if (itemId == _discount.first) discount = _discount.second;
    int price = (Item::ITEMS[itemId]->getValue() - ((float)Item::ITEMS[itemId]->getValue() * discount));
    if (discount < 1.f && price == 0) price = 1;
    price *= amount;

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

        if (Item::ITEMS[itemId]->getName() == "Shopkeep's Heart") shopKeep->takeDamage(shopKeep->getMaxHitPoints());

        if (!Tutorial::isCompleted() 
            && Item::ITEMS[itemId]->getId() == Item::BOW.getId()) Tutorial::completeStep(TUTORIAL_STEP::BUY_BOW);

        StatManager::increaseStat(ITEMS_PURCHASED, amount);

        return true;
    } else {
        if (discount == 1.f && price == 0) return true;
        MessageManager::displayMessage("You do not have enough pennies to buy this item!", 5);
        return false;
    }
}

bool ShopManager::sell(int itemId, int amount) {
    float discount = 0.f;
    if (itemId == _discount.first) discount = _discount.second;
    int price = (Item::ITEMS[itemId]->getValue() - ((float)Item::ITEMS[itemId]->getValue() * discount));
    if (discount < 1.f && price == 0) price = 1;
    price *= amount;

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

        StatManager::increaseStat(ITEMS_SOLD, amount);

        return true;
    } else {
        MessageManager::displayMessage("The Shopkeep does not have enough pennies to buy this item!", 5);
        return false;
    }
}

void ShopManager::reset() {
    _shopLedger.clear();
    _discountHistory.clear();
    _discount = { 0, 0.f };
    _initialShopInventories.clear();
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

    if (!_buyInterface->blockControllerInput && !_sellInterface->blockControllerInput) {
        _buyInterface->blockControllerInput = false;
        _sellInterface->blockControllerInput = true;
    }
}

void ShopManager::setDiscount(unsigned int shopSeed, unsigned int itemId, float percentOff) {
    const bool useRecordedDiscount = _discountHistory.find(shopSeed) != _discountHistory.end();

    _discount = useRecordedDiscount ? 
        _discountHistory.at(shopSeed) :
        std::pair<unsigned int, float>(itemId, percentOff);

    if (!useRecordedDiscount && percentOff > 0.f) _discountHistory[shopSeed] = {itemId, percentOff};
}

std::pair<unsigned int, float> ShopManager::getDiscount() const {
    return _discount;
}

void ShopManager::addItemToInitialInventory(unsigned int shopSeed, unsigned int itemId, unsigned int amount) {
    if (_initialShopInventories.find(shopSeed) == _initialShopInventories.end()) {
        _initialShopInventories[shopSeed].push_back({ itemId, amount });
        return;
    }
    _initialShopInventories.at(shopSeed).push_back({ itemId, amount });
}

std::vector<std::pair<unsigned int, unsigned int>> ShopManager::getInitialInventory(unsigned int shopSeed) {
    if (_initialShopInventories.find(shopSeed) == _initialShopInventories.end()) {
        MessageManager::displayMessage("No initial inventory for shop " + std::to_string(shopSeed), 5, ERR);
        return {};
    }

    return _initialShopInventories.at(shopSeed);
}
