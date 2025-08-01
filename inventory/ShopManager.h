#ifndef _SHOP_MANAGER_H
#define _SHOP_MANAGER_H

#include <memory>
#include "../ui/UIShopInterface.h"

class ShopManager {
public:
    void setBuyInterface(std::shared_ptr<UIShopInterface> buyInterface);
    void setSellInterface(std::shared_ptr<UIShopInterface> sellInterface);

    // These both return whether or not the buyer/seller has enough pennies for the transaction
    bool buy(int itemId, int amount);
    bool sell(int itemId, int amount);

    void reset();
    std::map<unsigned int, std::map<unsigned int, std::pair<unsigned int, int>>> getShopLedger() const;

    void controllerButtonReleased(GAMEPAD_BUTTON button);

    void setDiscount(unsigned int shopSeed, unsigned int itemId, float percentOff);
    std::pair<unsigned int, float> getDiscount() const;

    void addItemToInitialInventory(unsigned int shopSeed, unsigned int itemId, unsigned int amount);
    std::vector<std::pair<unsigned int, unsigned int>> getInitialInventory(unsigned int shopSeed);

    friend class SaveManager;
private:
    std::shared_ptr<UIShopInterface> _buyInterface;
    std::shared_ptr<UIShopInterface> _sellInterface;

    // seed: {transactionNumber: itemId: amount}
    std::map<unsigned int, std::map<unsigned int, std::pair<unsigned int, int>>> _shopLedger;

    // seed: itemId, discount
    std::map<unsigned int, std::pair<unsigned int, float>> _discountHistory;

    std::pair<unsigned int, float> _discount;

    // seed: itemId, amount
    std::map<unsigned int, std::vector<std::pair<unsigned int, unsigned int>>> _initialShopInventories;
};

#endif