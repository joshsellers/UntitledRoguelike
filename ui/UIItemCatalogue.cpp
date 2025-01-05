#include "UIItemCatalogue.h"
#include <iostream>
#include "../core/gamepad/GamePad.h"
#include "UIHandler.h"

constexpr int ITEM_SPACING = 7;
constexpr float SCROLL_RATE = 2.f;

constexpr long long DPAD_HOLD_TIME = 250LL;

UIItemCatalogue::UIItemCatalogue(float x, float y, int unlockedCount, int unlockableCount, Inventory source, sf::Font font, std::shared_ptr<sf::Texture> spriteSheet) :
    _source(source), _spriteSheet(spriteSheet), UIElement(x, y, 3, 3, false, false, font), _originalY(_y) {

    _disableAutomaticTextAlignment = true;

    float fontSize = 2.f;
    int relativeFontSize = (float)WINDOW_WIDTH * (fontSize / 100);
    _text.setFont(_font);
    _text.setCharacterSize(relativeFontSize);
    _text.setFillColor(sf::Color(0x81613DFF));
    _text.setString(std::to_string(unlockedCount) + "/" + std::to_string(unlockableCount) + " items unlocked");
    sf::Vector2f basePos(getRelativePos(sf::Vector2f(_x - 1.5f, _y)));
    _text.setPosition(basePos.x + getRelativeWidth(12.5f) - _text.getGlobalBounds().width / 2.f, getRelativeHeight(5.f));

    fontSize = 1;
    relativeFontSize = (float)WINDOW_WIDTH * (fontSize / 100);
    _tooltipText.setFont(_font);
    _tooltipText.setCharacterSize(relativeFontSize);
    _tooltipText.setFillColor(sf::Color(0x000023FF));

    _tooltipBg.setFillColor(sf::Color(0xFFFFCAFF));
    _tooltipBg.setOutlineColor(sf::Color(0xEEEEB9FF));
    _tooltipBg.setOutlineThickness(getRelativeWidth(0.25f));

    //_background.setFillColor(sf::Color(0x000044EE));
    //_background.setOutlineColor(sf::Color(0x000066EE));
    //_background.setOutlineThickness(getRelativeWidth(0.75f));
    _background.setPosition(getRelativePos(_x - 1.5f, 0.9f));
    _background.setSize(sf::Vector2f(getRelativeWidth(25.f), getRelativeHeight(98.3f)));
    _background.setTexture(UIHandler::getUISpriteSheet().get());
    _background.setTextureRect(sf::IntRect(0, 32, 80, 160));

    //_headerBg.setFillColor(sf::Color(0x000044FF));
    _headerBg.setPosition(
        _background.getPosition().x,
        getRelativeHeight(1.75f)
    );
    _headerBg.setSize(sf::Vector2f(
        _background.getSize().x,
        getRelativeHeight(8.f)
    ));
    _headerBg.setTexture(UIHandler::getUISpriteSheet().get());
    _headerBg.setTextureRect(sf::IntRect(0, 16, 80, 16));
}

void UIItemCatalogue::update() {
    if (GamePad::isButtonPressed(GAMEPAD_BUTTON::DPAD_UP)
        && currentTimeMillis() - _lastDPadPressTime > DPAD_HOLD_TIME
        && !blockControllerInput) {
        gamepadScrollUp();
    } else if (GamePad::isButtonPressed(GAMEPAD_BUTTON::DPAD_DOWN)
        && currentTimeMillis() - _lastDPadPressTime > DPAD_HOLD_TIME
        && !blockControllerInput) {
        gamepadScrollDown();
    }
}

void UIItemCatalogue::draw(sf::RenderTexture& surface) {
    surface.draw(_background);
    if (_itemsOutsideWindow) {
        sf::RectangleShape edgeCover;
        edgeCover.setPosition(getRelativePos(_x - 1.5f, 95.f));
        edgeCover.setSize(sf::Vector2f(_background.getSize().x, getRelativeHeight(5.f)));
        edgeCover.setTexture(UIHandler::getUISpriteSheet().get());
        edgeCover.setTextureRect(sf::IntRect(0, 187, 80, 1));
        surface.draw(edgeCover);

        edgeCover.setPosition(getRelativePos(_x - 1.5f, 0.f));
        edgeCover.setTextureRect(sf::IntRect(0, 111, 80, 1));
        surface.draw(edgeCover);

        _headerBg.setTextureRect(sf::IntRect(80, 16, 80, 16));
    } else _headerBg.setTextureRect(sf::IntRect(0, 16, 80, 16));

    int mousedOverItemIndex = -1;
    int filteredIndex = 0;
    for (int i = 0; i < _source.getCurrentSize(); i++) {
        std::shared_ptr<const Item> item = Item::ITEMS[_source.getItemIdAt(i)];

        const EQUIPMENT_TYPE itemType = item->getEquipmentType();
        if (!isItemCorrectType(itemType)) continue;

        sf::Vector2f itemPos(getRelativePos(sf::Vector2f(_x, _y + (ITEM_SPACING * filteredIndex) + 1.f)));

        sf::Text label;
        float fontSize = 1.5;
        int relativeFontSize = (float)WINDOW_WIDTH * (fontSize / 100);
        label.setFont(_font);
        label.setCharacterSize(relativeFontSize);
        label.setFillColor(sf::Color::White);
        label.setPosition(itemPos.x - (_width / 8) + _width + (_width / 8) * 3, itemPos.y + _height / 6);
        std::string itemName = item->getName();

        label.setString(itemName);

        sf::RectangleShape labelBg(sf::Vector2f(_width + (_width / 8) * 3 + label.getGlobalBounds().width + (_width / 8), _height + (_height / 8) * 2));
        labelBg.setPosition(sf::Vector2f(itemPos.x - (_width / 8), itemPos.y - (_width / 8)));
        sf::Uint32 labelBgColor = 0x000066FF;
        labelBg.setFillColor(sf::Color(labelBgColor));

        sf::RectangleShape bg(sf::Vector2f(_width + (_width / 8) * 2, _height + (_height / 8) * 2));
        bg.setPosition(sf::Vector2f(itemPos.x - (_width / 8), itemPos.y - (_width / 8)));
        bg.setTexture(UIHandler::getUISpriteSheet().get());
        bg.setTextureRect(sf::IntRect(96, 64, 24, 24));

        sf::Sprite itemSprite;
        itemSprite.setTexture(*_spriteSheet);
        itemSprite.setTextureRect(item->getTextureRect());
        itemSprite.setScale(sf::Vector2f((_width) / itemSprite.getGlobalBounds().width, (_height) / itemSprite.getGlobalBounds().height));
        itemSprite.setPosition(itemPos);

        const sf::FloatRect interactionBox(itemPos.x - (_width / 8), itemPos.y - (_width / 8), (_width + (_width / 8) * 2) * 5.5f, _height + (_height / 8) * 2);
        const bool touchingItem = interactionBox.contains(_mousePos) || filteredIndex == _gamepadSelectedItemIndex;

        if (touchingItem) {
            //bg.setFillColor(sf::Color(0x0000FFFF));
            bg.setTextureRect(sf::IntRect(128, 64, 24, 24));
        } else {
            //bg.setFillColor(sf::Color(0x000099FF));
        }

        //if (_source.isEquipped(i) && touchingItem) bg.setTextureRect(sf::IntRect(192, 64, 24, 24));
        //else if (_source.isEquipped(i)) bg.setTextureRect(sf::IntRect(160, 64, 24, 24));

        //surface.draw(labelBg);
        surface.draw(bg);
        surface.draw(itemSprite);
        surface.draw(label);

        filteredIndex++;

        if (labelBg.getGlobalBounds().contains(_mousePos) || interactionBox.contains(_mousePos))
            mousedOverItemIndex = i;
    }

    if (_itemsOutsideWindow = filteredIndex + 1 > 13) {
        constexpr float headerPadding = 12.f;

        sf::RectangleShape scrollBar;
        float scrollBarMinHeight = 8.f;

        sf::Vector2f scrollBarPos = getRelativePos(_x + 21.5f, _originalY);
        float entryHeight = getRelativeHeight((float)ITEM_SPACING);
        float itemAmount = (float)filteredIndex + 1;
        float allItemsHeight = itemAmount * entryHeight;
        float scrollDelta = (_originalY - _y);

        scrollBar.setSize(sf::Vector2f(
            getRelativeWidth(1.f), getRelativeHeight(std::max(scrollBarMinHeight, (91.f * ((getRelativeHeight(100) - getRelativeHeight(headerPadding)) / allItemsHeight)) / 1.f)))
        );

        float relY = getRelativeHeight(scrollDelta);
        float yToDeltaHeightRatio = relY / (allItemsHeight - (getRelativeHeight(100) - getRelativeHeight(headerPadding)));
        float scrollBarHeight = getRelativeHeight(100) - getRelativeHeight(headerPadding);
        float handleHeight = scrollBar.getSize().y;
        float handleY = yToDeltaHeightRatio * (scrollBarHeight - handleHeight);

        if (_userIsDraggingScrollbar) {
            scrollBarPos.y = (_mousePos.y) - (_mouseYWhenClickedInScrollbar - (_scrollBarPosY));
            float newY = (allItemsHeight - (getRelativeHeight(100) - getRelativeHeight(headerPadding))) * scrollBarPos.y / (scrollBarHeight - scrollBar.getSize().y);
            scrollDelta = newY / (float)WINDOW_HEIGHT * 100.f;
            _y = (_originalY - scrollDelta);
        } else {
            _scrollBarPosY = handleY;
            scrollBarPos.y = _scrollBarPosY;
        }

        // scroll bar gfx
        _scrollBarRTexture.create(6, handleHeight);
        sf::RectangleShape handleCenter;
        handleCenter.setPosition(0, 0);
        handleCenter.setSize(sf::Vector2f(6, handleHeight));
        handleCenter.setTexture(UIHandler::getUISpriteSheet().get());
        handleCenter.setTextureRect(sf::IntRect(113, 32, 6, 2));
        _scrollBarRTexture.draw(handleCenter);

        //sf::RectangleShape handleTop;
        //handleTop.setPosition(0, 0);
        //handleTop.setSize(sf::Vector2f(6, 16));
        //handleTop.setTexture(UIHandler::getUISpriteSheet().get());
        //handleTop.setTextureRect(sf::IntRect(101, 32, 6, 3));
        //handleTop.setScale(0, -1);
        ////_scrollBarRTexture.draw(handleTop);

        //sf::RectangleShape handleBottom;
        //handleBottom.setPosition(0, 0);
        //handleBottom.setSize(sf::Vector2f(6, 16));
        //handleBottom.setTexture(UIHandler::getUISpriteSheet().get());
        //handleBottom.setTextureRect(sf::IntRect(107, 32, 6, 3));
        //_scrollBarRTexture.draw(handleBottom);
        //


        scrollBar.setPosition(scrollBarPos.x, scrollBarPos.y + getRelativeHeight(headerPadding));
        //scrollBar.setFillColor(sf::Color(_userIsDraggingScrollbar || _mousedOverScrollbar ? 0x0000FFEE : 0x0000FFAA));
        scrollBar.setTexture(&_scrollBarRTexture.getTexture());
        scrollBar.setTextureRect(sf::IntRect(0, 0, 6, handleHeight));

        sf::Sprite handleTop;
        handleTop.setTexture(*UIHandler::getUISpriteSheet());
        handleTop.setTextureRect(sf::IntRect(101, 32, 6, 3));
        handleTop.setScale(scrollBar.getSize().x / 6.f, getRelativeHeight(1.f) / 3.f);
        handleTop.setPosition(scrollBarPos.x, scrollBarPos.y + getRelativeHeight(headerPadding) - handleTop.getGlobalBounds().height);

        sf::Sprite handleBottom;
        handleBottom.setTexture(*UIHandler::getUISpriteSheet());
        handleBottom.setTextureRect(sf::IntRect(107, 32, 6, 3));
        handleBottom.setScale(scrollBar.getSize().x / 6.f, getRelativeHeight(1.f) / 3.f);
        handleBottom.setPosition(scrollBarPos.x, scrollBarPos.y + getRelativeHeight(headerPadding) + scrollBar.getGlobalBounds().height);

        sf::RectangleShape scrollBg;
        scrollBg.setSize(getRelativePos(1.f, 83.f));
        scrollBg.setPosition(getRelativePos(_x + 21.5f, 12.f));
        scrollBg.setFillColor(sf::Color(0x000000FF));

        //surface.draw(scrollBg);
        surface.draw(scrollBar);
        surface.draw(handleTop);
        surface.draw(handleBottom);
    }

    surface.draw(_headerBg);
    surface.draw(_text);

    try {
        if (mousedOverItemIndex >= 0 || (_gamepadShowTooltip && _gamepadSelectedItemIndex < (int)_source.getCurrentSize() && _gamepadSelectedItemIndex >= 0
            && _gamepadUnfilteredSelectedItemIndex < (int)_source.getCurrentSize())) {
            std::shared_ptr<const Item> item = Item::ITEMS[
                _source.getItemIdAt(
                    (_gamepadShowTooltip && GamePad::isConnected() && _gamepadUnfilteredSelectedItemIndex >= 0 && !USING_MOUSE) ?
                    _gamepadUnfilteredSelectedItemIndex :
                    mousedOverItemIndex
                )
            ];

            float textXOffset = (float)WINDOW_WIDTH * (2.f / 100);

            _tooltipText.setString(item->getDescription());
            float textWidth = _tooltipText.getGlobalBounds().width;
            float textHeight = _tooltipText.getGlobalBounds().height;
            sf::Vector2f pos(_mousePos.x + textXOffset, _mousePos.y - textHeight / 2);

            if (_gamepadShowTooltip && GamePad::isConnected() && mousedOverItemIndex == -1) {
                sf::Vector2f itemPos(getRelativePos(sf::Vector2f(_x, _y + (ITEM_SPACING * _gamepadSelectedItemIndex) + 1.f)));
                pos.x = _background.getGlobalBounds().width;
                if (_x != 2) pos.x = getRelativeWidth(_x) - textWidth - getRelativeWidth(2.25f);
                pos.y = itemPos.y + ;
            }

            _tooltipText.setPosition(pos);

            float padding = (float)WINDOW_HEIGHT * (1.f / 100);

            float bgWidth = textWidth + padding * 2;
            float bgHeight = textHeight + padding * 2;
            _tooltipBg.setPosition(pos.x - padding, pos.y - padding);
            _tooltipBg.setSize(sf::Vector2f(bgWidth, bgHeight));

            surface.draw(_tooltipBg);
            surface.draw(_tooltipText);

            drawAdditionalTooltip(surface, mousedOverItemIndex);
        }
    } catch (std::exception ex) {
        MessageManager::displayMessage("Error in UIItemCatalogue::draw: " + (std::string)ex.what(), 5, ERR);
        return;
    }

    subDraw(surface);
}

void UIItemCatalogue::subDraw(sf::RenderTexture& surface) {}

void UIItemCatalogue::drawAdditionalTooltip(sf::RenderTexture& surface, int mousedOverItemIndex) {}

bool UIItemCatalogue::isItemCorrectType(EQUIPMENT_TYPE type) {
    return true;
}

void UIItemCatalogue::useItem(int index) {

}

void UIItemCatalogue::dropItem(int index) {

}

void UIItemCatalogue::dropStack(int index) {

}

void UIItemCatalogue::controllerButtonReleased(GAMEPAD_BUTTON button) {

}

void UIItemCatalogue::controllerButtonPressed(GAMEPAD_BUTTON button) {
    switch (button) {
        case GAMEPAD_BUTTON::DPAD_DOWN:
            _lastDPadPressTime = currentTimeMillis();
            gamepadScrollDown();
            break;
        case GAMEPAD_BUTTON::DPAD_UP:
            _lastDPadPressTime = currentTimeMillis();
            gamepadScrollUp();
            break;
    }
}

void UIItemCatalogue::gamepadScrollDown() {
    if (_source.getCurrentSize() > 0 && _gamepadSelectedItemIndex < (int)_source.getCurrentSize() - 1) {
        _gamepadSelectedItemIndex++;

        if (_gamepadSelectedItemIndex >= 12 && (int)_source.getCurrentSize() > 13) {
            _y -= ITEM_SPACING;
        }

        unfilterGamepadIndex();
    }
}

void UIItemCatalogue::gamepadScrollUp() {
    if (_source.getCurrentSize() > 0 && _gamepadSelectedItemIndex > 0) {
        _gamepadSelectedItemIndex--;

        if (_gamepadSelectedItemIndex >= 11 && (int)_source.getCurrentSize() > 13) {
            _y += ITEM_SPACING;
        }

        unfilterGamepadIndex();
    }
}

void UIItemCatalogue::unfilterGamepadIndex() {
    _gamepadUnfilteredSelectedItemIndex = _gamepadSelectedItemIndex;
}

void UIItemCatalogue::mouseButtonPressed(const int mx, const int my, const int button) {
    if (button == sf::Mouse::Left) {
        int totalDisplayedItems = 0;
        for (int i = 0; i < _source.getCurrentSize(); i++) {
            std::shared_ptr<const Item> item = Item::ITEMS[_source.getItemIdAt(i)];
            if (isItemCorrectType(item->getEquipmentType())) totalDisplayedItems++;
        }

        if (totalDisplayedItems + 1 > 13) {
            constexpr float headerPadding = 12.f;

            sf::FloatRect scrollBar;
            float scrollBarMinHeight = 8.f;

            sf::Vector2f scrollBarPos = getRelativePos(_x + 21.5f, _originalY);
            float entryHeight = getRelativeHeight((float)ITEM_SPACING);
            float itemAmount = (float)totalDisplayedItems + 1;
            float allItemsHeight = itemAmount * entryHeight;
            float scrollDelta = (_originalY - _y);

            scrollBar.width = getRelativeWidth(1.f);
            scrollBar.height = getRelativeHeight(std::max(scrollBarMinHeight, (91.f * ((getRelativeHeight(100) - getRelativeHeight(headerPadding)) / allItemsHeight)) / 1.f));

            float relY = getRelativeHeight(scrollDelta);
            float yToDeltaHeightRatio = relY / (allItemsHeight - (getRelativeHeight(100) - getRelativeHeight(headerPadding)));
            float scrollBarHeight = getRelativeHeight(100) - getRelativeHeight(headerPadding);
            float handleHeight = scrollBar.height;
            float handleY = yToDeltaHeightRatio * (scrollBarHeight - handleHeight);

            scrollBarPos.y = handleY + getRelativeHeight(headerPadding);

            scrollBar.left = scrollBarPos.x;
            scrollBar.top = scrollBarPos.y;

            if (sf::FloatRect(mx, my, 5, 5).intersects(scrollBar)) {
                _userIsDraggingScrollbar = true;
                _mouseYWhenClickedInScrollbar = my;
            }
        }
    }
}

void UIItemCatalogue::mouseButtonReleased(const int mx, const int my, const int button) {
    _userIsDraggingScrollbar = false;
}

void UIItemCatalogue::mouseMoved(const int mx, const int my) {
    _gamepadSelectedItemIndex = -1;
    //_gamepadShowTooltip = false;
    _mousePos = sf::Vector2f(mx, my);

    blockControllerInput = false;

    int totalDisplayedItems = 0;
    for (int i = 0; i < _source.getCurrentSize(); i++) {
        std::shared_ptr<const Item> item = Item::ITEMS[_source.getItemIdAt(i)];
        if (isItemCorrectType(item->getEquipmentType())) totalDisplayedItems++;
    }

    _mousedOverScrollbar = false;
    if (totalDisplayedItems + 1 > 13) {
        constexpr float headerPadding = 12.f;

        sf::FloatRect scrollBar;
        float scrollBarMinHeight = 8.f;

        sf::Vector2f scrollBarPos = getRelativePos(_x + 21.5f, _originalY);
        float entryHeight = getRelativeHeight((float)ITEM_SPACING);
        float itemAmount = (float)totalDisplayedItems + 1;
        float allItemsHeight = itemAmount * entryHeight;
        float scrollDelta = (_originalY - _y);

        scrollBar.width = getRelativeWidth(1.f);
        scrollBar.height = getRelativeHeight(std::max(scrollBarMinHeight, (91.f * ((getRelativeHeight(100) - getRelativeHeight(headerPadding)) / allItemsHeight)) / 1.f));

        float relY = getRelativeHeight(scrollDelta);
        float yToDeltaHeightRatio = relY / (allItemsHeight - (getRelativeHeight(100) - getRelativeHeight(headerPadding)));
        float scrollBarHeight = getRelativeHeight(100) - getRelativeHeight(headerPadding);
        float handleHeight = scrollBar.height;
        float handleY = yToDeltaHeightRatio * (scrollBarHeight - handleHeight);

        scrollBarPos.y = handleY + getRelativeHeight(headerPadding);

        scrollBar.left = scrollBarPos.x;
        scrollBar.top = scrollBarPos.y;

        if (sf::FloatRect(mx, my, 5, 5).intersects(scrollBar)) {
            _mousedOverScrollbar = true;
        }
    }
}

void UIItemCatalogue::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
    // Check if mouse is within the inventory UI (for shop UI, because there's two menus)
    sf::FloatRect uiBounds(_background.getPosition().x, _background.getPosition().y, _background.getSize().x, _background.getSize().y);
    if (!uiBounds.contains(mouseWheelScroll.x, mouseWheelScroll.y)) return;

    if (_source.getCurrentSize() != 0) _y += mouseWheelScroll.delta * SCROLL_RATE;
}

void UIItemCatalogue::textEntered(const sf::Uint32 character) {}

void UIItemCatalogue::hide() {
    _isActive = false;
    _gamepadSelectedItemIndex = -1;
    //_gamepadShowTooltip = false;
    _y = _originalY;
}

void UIItemCatalogue::setSource(Inventory sourceInventory) {
    _source = sourceInventory;
}

Inventory UIItemCatalogue::getSource() {
    return _source;
}

