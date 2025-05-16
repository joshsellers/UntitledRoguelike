#include "UIShopInterface.h"
#include "../inventory/ShopManager.h"
#include "../core/gamepad/GamePad.h"
#include "../statistics/StatManager.h"
#include "../world/entities/Entity.h"

UIShopInterface::UIShopInterface(ShopManager& shopManager, bool buyMode, Inventory& source, sf::Font font, std::shared_ptr<sf::Texture> spriteSheet)
    : UIInventoryInterface(buyMode ? 2 : 76, 11, source, font, spriteSheet), _shopManager(shopManager) {
    _buyMode = buyMode;
    _text.setString(_buyMode ? "SHOP" : "INVENTORY"); 
    sf::Vector2f basePos(getRelativePos(sf::Vector2f(_x - 1.5f, _y)));
    _text.setPosition(basePos.x + getRelativeWidth(12.5f) - _text.getGlobalBounds().width / 2.f, getRelativeHeight(4.f));

    _displayControls = false;
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
    attemptTransaction(index, _source.getItemAmountAt(index) >= 25 ? 25 : 1);
}

void UIShopInterface::attemptTransaction(int index, int amount) {
    bool transactionWasSuccessful = false;

    if (_buyMode) transactionWasSuccessful = _shopManager.buy(_source.getItemIdAt(index), amount);
    else transactionWasSuccessful = _shopManager.sell(_source.getItemIdAt(index), amount);

    if (transactionWasSuccessful) {
        float discount = 0.f;
        const unsigned int itemId = _source.getItemIdAt(index);
        if (itemId == _shopManager.getDiscount().first) discount = _shopManager.getDiscount().second;
        int price = (Item::ITEMS[itemId]->getValue() - ((float)Item::ITEMS[itemId]->getValue() * discount));
        if (discount < 1.f && price == 0) price = 1;
        price *= amount;

        _source.addItem(Item::PENNY.getId(), price);
        if (!_buyMode) StatManager::increaseStat(PENNIES_COLLECTED, price);
        _source.removeItemAt(index, amount);
    }
}

void UIShopInterface::drawAdditionalTooltip(sf::RenderTexture& surface, int mousedOverItemIndex) {
    std::shared_ptr<const Item> item = Item::ITEMS[
        _source.getItemIdAt(
            (_gamepadShowTooltip && GamePad::isConnected() && _gamepadUnfilteredSelectedItemIndex >= 0 && !USING_MOUSE) ?
            _gamepadUnfilteredSelectedItemIndex :
            mousedOverItemIndex
        )
    ];

    float textXOffset = (float)WINDOW_WIDTH * (2.f / 100);

    int price = item->getValue();
    bool discount = false;
    if (item->getId() == _shopManager.getDiscount().first) {
        price = price - ((float)price * _shopManager.getDiscount().second);
        if (price == 0 && _shopManager.getDiscount().second < 1.f) price = 1;

        if (_buyMode && _shopManager.getDiscount().second != 0.f) discount = true;
    }

    std::string ttText = item->getId() == Item::PENNY.getId() ? 
        "The shopkeep can use these to buy\nstuff you want to sell him"
        : "PRICE: " + std::to_string(price) + "¢" + (discount ? " (" + std::to_string((int)(_shopManager.getDiscount().second * 100)) + "% discount!)" : "") + "\n\n" + item->getDescription();
    if (!_buyMode && item->getId() == Item::PENNY.getId()) ttText = "Use these to buy stuff";

    _tooltipText.setString(ttText);
    float textWidth = _tooltipText.getGlobalBounds().width;
    float textHeight = _tooltipText.getGlobalBounds().height;
    sf::Vector2f pos(_mousePos.x + textXOffset, _mousePos.y - textHeight / 2);

    if (_gamepadShowTooltip && GamePad::isConnected() && mousedOverItemIndex == -1) {
        sf::Vector2f itemPos(getRelativePos(sf::Vector2f(_x, _y + (ITEM_SPACING * _gamepadSelectedItemIndex) + 1.f)));
        pos.x = _background.getGlobalBounds().width;
        if (_x != 2) pos.x = getRelativeWidth(_x) - textWidth - getRelativeWidth(2.25f);
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

    if (_buyMode && _shopManager.getDiscount().second != 0.f) _discountedItemId = _shopManager.getDiscount().first;
    else _discountedItemId = -1;

    if (GamePad::isConnected()) {
        sf::Text controlLabel;
        controlLabel.setFont(_font);
        controlLabel.setCharacterSize(getRelativeWidth(1.25f));
        const float x = 43.f;
        const float xTextPadding = 3.5f;
        const float y = 70.f;
        const float ySpacing = 4.f;

        _buttonSprite.setTextureRect(sf::IntRect(0, 240, TILE_SIZE, TILE_SIZE));
        _buttonSprite.setPosition(getRelativePos(x, y + ySpacing * 0));
        surface.draw(_buttonSprite);

        controlLabel.setString("switch to buy menu");
        controlLabel.setPosition(getRelativePos(x + xTextPadding, y + ySpacing * 0 + 0.5f));
        surface.draw(controlLabel);

        _buttonSprite.setTextureRect(sf::IntRect(16, 240, TILE_SIZE, TILE_SIZE));
        _buttonSprite.setPosition(getRelativePos(x, y + ySpacing * 1));
        surface.draw(_buttonSprite);

        controlLabel.setString("switch to sell menu");
        controlLabel.setPosition(getRelativePos(x + xTextPadding, y + ySpacing * 1 + 0.5f));
        surface.draw(controlLabel);

        _buttonSprite.setTextureRect(sf::IntRect(0, 208, TILE_SIZE, TILE_SIZE));
        _buttonSprite.setPosition(getRelativePos(x, y + ySpacing * 2));
        surface.draw(_buttonSprite);

        controlLabel.setString("buy/sell");
        controlLabel.setPosition(getRelativePos(x + xTextPadding, y + ySpacing * 2 + 0.5f));
        surface.draw(controlLabel);

        _buttonSprite.setTextureRect(sf::IntRect(16, 208, TILE_SIZE, TILE_SIZE));
        _buttonSprite.setPosition(getRelativePos(x, y + ySpacing * 3));
        surface.draw(_buttonSprite);
        controlLabel.setString("buy/sell all");
        controlLabel.setPosition(getRelativePos(x + xTextPadding, y + ySpacing * 3 + 0.5f));
        surface.draw(controlLabel);

        _buttonSprite.setTextureRect(sf::IntRect(0, 224, TILE_SIZE, TILE_SIZE));
        _buttonSprite.setPosition(getRelativePos(x, y + ySpacing * 4));
        surface.draw(_buttonSprite);
        controlLabel.setString("buy/sell 25");
        controlLabel.setPosition(getRelativePos(x + xTextPadding, y + ySpacing * 4 + 1.f));
        surface.draw(controlLabel);
    }
}

void UIShopInterface::darkenUnselectedMenu(sf::RenderTexture& surface) {
    sf::RectangleShape cover;
    cover.setPosition(_background.getPosition().x, getRelativeHeight(0.f));
    cover.setSize(sf::Vector2f(_background.getSize().x, getRelativeHeight(100.f)));
    cover.setFillColor(sf::Color(0x000000AA));
    surface.draw(cover);
}
