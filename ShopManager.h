#ifndef _SHOP_MANAGER_H
#define _SHOP_MANAGER_H

#include "UIShopInterface.h"
#include <memory>

class ShopManager {
public:
    void setBuyInterface(std::shared_ptr<UIShopInterface> buyInterface);
    void setSellInterface(std::shared_ptr<UIShopInterface> sellInterface);

    // These both return whether or not the buyer/seller has enough pennies for the transaction
    bool buy(int itemId, int amount);
    bool sell(int itemId, int amount);

private:
    std::shared_ptr<UIShopInterface> _buyInterface;
    std::shared_ptr<UIShopInterface> _sellInterface;
};

#endif