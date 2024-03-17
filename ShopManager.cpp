#include "ShopManager.h"
#include "MessageManager.h"

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
        return true;
    } else {
        MessageManager::displayMessage("The Shopkeep does not have enough pennies to buy this item!", 5);
        return false;
    }
}
