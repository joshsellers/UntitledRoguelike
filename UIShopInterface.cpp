#include "UIShopInterface.h"
#include "ShopManager.h"

UIShopInterface::UIShopInterface(ShopManager& shopManager, bool buyMode, Inventory& source, sf::Font font, std::shared_ptr<sf::Texture> spriteSheet)
    : UIInventoryInterface(buyMode ? 2 : 76, 11, source, font, spriteSheet), _shopManager(shopManager) {
    _buyMode = buyMode;
    _text.setString(_buyMode ? "SHOP" : "INVENTORY");
}

void UIShopInterface::buttonPressed(std::string buttonCode) {
    _y = _originalY;
    if (buttonCode == "filter_none") setFilter(FILTER_TYPE::NONE);
    else if (buttonCode == "filter_apparel") setFilter(FILTER_TYPE::APPAREL);
    else if (buttonCode == "filter_weapons") setFilter(FILTER_TYPE::WEAPONS);
    else if (buttonCode == "filter_ammo") setFilter(FILTER_TYPE::AMMO);
    else if (buttonCode == "filter_misc") setFilter(FILTER_TYPE::MISC);
}

void UIShopInterface::addItem(int itemId, int amount) {
    if (amount == 0) return;
    _source.addItem(itemId, amount);
}

void UIShopInterface::useItem(int index) {
    attemptTransaction(index, 1);
}

void UIShopInterface::dropItem(int index) {
    attemptTransaction(index, _source.getItemAmountAt(index));
}

void UIShopInterface::dropStack(int index) {
    attemptTransaction(index, _source.getItemAmountAt(index) > 1 ? _source.getItemAmountAt(index) / 4 : 1);
}

void UIShopInterface::attemptTransaction(int index, int amount) {
    bool transactionWasSuccessful = false;

    if (_buyMode) transactionWasSuccessful = _shopManager.buy(_source.getItemIdAt(index), amount);
    else transactionWasSuccessful = _shopManager.sell(_source.getItemIdAt(index), amount);

    if (transactionWasSuccessful) {
        _source.addItem(Item::PENNY.getId(), Item::ITEMS[_source.getItemIdAt(index)]->getValue() * amount);
        _source.removeItemAt(index, amount);
    }
}

void UIShopInterface::drawAdditionalTooltip(sf::RenderTexture& surface, int mousedOverItemIndex) {
    const Item* item = Item::ITEMS[
        _source.getItemIdAt(_gamepadShowTooltip ? _gamepadUnfilteredSelectedItemIndex : mousedOverItemIndex)
    ];

    float textXOffset = (float)WINDOW_WIDTH * (2.f / 100);

    std::string ttText = item->getId() == Item::PENNY.getId() ? 
        item->getDescription() 
        : "PRICE: " + std::to_string(item->getValue()) + "¢\n\n" + item->getDescription();

    _tooltipText.setString(ttText);
    float textWidth = _tooltipText.getGlobalBounds().width;
    float textHeight = _tooltipText.getGlobalBounds().height;
    sf::Vector2f pos(_mousePos.x + textXOffset, _mousePos.y - textHeight / 2);

    if (_gamepadShowTooltip) {
        sf::Vector2f itemPos(getRelativePos(sf::Vector2f(_x, _y + (ITEM_SPACING * _gamepadSelectedItemIndex))));
        pos.x = _background.getGlobalBounds().width;
        pos.y = itemPos.y;
    }

    _tooltipText.setPosition(pos);

    float padding = (float)WINDOW_HEIGHT * (1.f / 100);

    float bgWidth = textWidth + padding * 2;
    float bgHeight = textHeight + padding * 2;
    _tooltipBg.setPosition(pos.x - padding, pos.y - padding);
    _tooltipBg.setSize(sf::Vector2f(bgWidth, bgHeight));

    surface.draw(_tooltipBg);
    surface.draw(_tooltipText);
}

void UIShopInterface::subDraw(sf::RenderTexture& surface) {
    if (blockControllerInput) {
        darkenUnselectedMenu(surface);
    }
}

void UIShopInterface::darkenUnselectedMenu(sf::RenderTexture& surface) {
    sf::RectangleShape cover;
    cover.setPosition(_background.getPosition().x, _background.getPosition().y);
    cover.setSize(_background.getSize());
    cover.setFillColor(sf::Color(0x000000AA));
    surface.draw(cover);
}
