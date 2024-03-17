#ifndef _UI_SHOP_INTERFACE_H
#define _UI_SHOP_INTERFACE_H

#include "UIInventoryInterface.h"

class ShopManager;

class UIShopInterface : public UIInventoryInterface {
public:
    UIShopInterface(ShopManager& shopManager, bool buyMode, Inventory& source, sf::Font, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void buttonPressed(std::string buttonCode);

    void addItem(int itemId, int amount);

protected:
    virtual void useItem(int index);
    virtual void dropItem(int index);
    virtual void dropStack(int index);

    virtual void drawAdditionalTooltip(sf::RenderTexture& surface, int mousedOverItemIndex);

private:
    bool _buyMode;

    ShopManager& _shopManager;

    void attemptTransaction(int index, int amount);
};

#endif