#include "UIInventoryInterface.h"
#include <iostream>
#include "../core/gamepad/GamePad.h"
#include "../world/entities/Entity.h"

UIInventoryInterface::UIInventoryInterface(Inventory& source, sf::Font font, std::shared_ptr<sf::Texture> spriteSheet) :
    UIInventoryInterface(2, 11, source, font, spriteSheet) {}

UIInventoryInterface::UIInventoryInterface(float x, float y, Inventory& source, sf::Font font, std::shared_ptr<sf::Texture> spriteSheet) :
    _source(source), _spriteSheet(spriteSheet), UIElement(x, y, 3, 3, false, false, font), _originalY(_y) {

    _disableAutomaticTextAlignment = true;

    float fontSize = 4;
    int relativeFontSize = (float)WINDOW_WIDTH * (fontSize / 100);
    _text.setFont(_font);
    _text.setCharacterSize(relativeFontSize);
    _text.setFillColor(sf::Color::White);
    _text.setString("INVENTORY");
    sf::Vector2f basePos(getRelativePos(sf::Vector2f(_x + 3, _y)));
    _text.setPosition(basePos.x - _text.getGlobalBounds().width / 6, 0);

    fontSize = 1;
    relativeFontSize = (float)WINDOW_WIDTH * (fontSize / 100);
    _tooltipText.setFont(_font);
    _tooltipText.setCharacterSize(relativeFontSize);
    _tooltipText.setFillColor(sf::Color(0x000023FF));

    _tooltipBg.setFillColor(sf::Color(0xFFFFCAFF));
    _tooltipBg.setOutlineColor(sf::Color(0xEEEEB9FF));
    _tooltipBg.setOutlineThickness(getRelativeWidth(0.25f));

    _background.setFillColor(sf::Color(0x000044EE));
    _background.setOutlineColor(sf::Color(0x000066EE));
    _background.setOutlineThickness(getRelativeWidth(0.75f));
    _background.setPosition(getRelativePos(_x - 1.5f, 0.9f));
    _background.setSize(sf::Vector2f(getRelativeWidth(25.f), getRelativeHeight(98.3f)));

    _headerBg.setFillColor(sf::Color(0x000044FF));
    _headerBg.setPosition(
        _background.getPosition().x,
        0
    );
    _headerBg.setSize(sf::Vector2f(
        _background.getSize().x,
        getRelativeHeight(8.f)
    ));
}

void UIInventoryInterface::update() {
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

void UIInventoryInterface::draw(sf::RenderTexture& surface) {
    surface.draw(_background);

    int mousedOverItemIndex = -1;
    int filteredIndex = 0;
    for (int i = 0; i < _source.getCurrentSize(); i++) {
        std::shared_ptr<const Item> item = Item::ITEMS[_source.getItemIdAt(i)];

        const EQUIPMENT_TYPE itemType = item->getEquipmentType();
        if (!isItemCorrectType(itemType)) continue;

        sf::Vector2f itemPos(getRelativePos(sf::Vector2f(_x, _y + (ITEM_SPACING * filteredIndex))));

        sf::Text label;
        float fontSize = 1.5;
        int relativeFontSize = (float)WINDOW_WIDTH * (fontSize / 100);
        label.setFont(_font);
        label.setCharacterSize(relativeFontSize);
        label.setFillColor(sf::Color::White);
        label.setPosition(itemPos.x - (_width / 8) + _width + (_width / 8) * 3, itemPos.y + _height / 6);
        std::string itemName = item->getName();
        if (item->getId() == Item::PENNY.getId() && _source.getParent()->getEntityType() == "shopkeep") itemName = "Shopkeep's pennies";
        label.setString(itemName + (
            item->isStackable() && _source.getItemAmountAt(i) > 1 ? " (" + std::to_string(_source.getItemAmountAt(i)) + ")" : ""
        ));

        sf::RectangleShape labelBg(sf::Vector2f(_width + (_width / 8) * 3 + label.getGlobalBounds().width + (_width / 8), _height + (_height / 8) * 2));
        labelBg.setPosition(sf::Vector2f(itemPos.x - (_width / 8), itemPos.y - (_width / 8)));
        sf::Uint32 labelBgColor = _source.isEquipped(i) ? 0x0000FFFF : 0x000066FF;
        labelBg.setFillColor(sf::Color(labelBgColor));

        sf::RectangleShape bg(sf::Vector2f(_width + (_width / 8) * 2, _height + (_height / 8) * 2));
        bg.setPosition(sf::Vector2f(itemPos.x - (_width / 8), itemPos.y - (_width / 8)));

        sf::Sprite itemSprite;
        itemSprite.setTexture(*_spriteSheet);
        itemSprite.setTextureRect(item->getTextureRect());
        itemSprite.setScale(sf::Vector2f(_width / itemSprite.getGlobalBounds().width, _height / itemSprite.getGlobalBounds().height));
        itemSprite.setPosition(itemPos);

        if (bg.getGlobalBounds().contains(_mousePos) || filteredIndex == _gamepadSelectedItemIndex) {
            bg.setFillColor(sf::Color(0x0000FFFF));
        } else bg.setFillColor(sf::Color(0x000099FF));

        surface.draw(labelBg);
        surface.draw(bg);
        surface.draw(itemSprite);
        surface.draw(label);

        filteredIndex++;

        if (labelBg.getGlobalBounds().contains(_mousePos) || bg.getGlobalBounds().contains(_mousePos))
            mousedOverItemIndex = i;
    }

    if (filteredIndex + 1 > 13) {
        constexpr float headerPadding = 8.f;

        sf::RectangleShape scrollBar;
        float scrollBarMinHeight = 2.f;

        sf::Vector2f scrollBarPos = getRelativePos(_x + 22.5f, _originalY);
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

        scrollBar.setPosition(scrollBarPos.x, scrollBarPos.y + getRelativeHeight(headerPadding));
        scrollBar.setFillColor(sf::Color(_userIsDraggingScrollbar || _mousedOverScrollbar ? 0x0000FFEE : 0x0000FFAA));

        sf::RectangleShape scrollBg;
        scrollBg.setSize(getRelativePos(1.f, 91.f));
        scrollBg.setPosition(getRelativePos(_x + 22.5f, 8.f));
        scrollBg.setFillColor(sf::Color(0x000099AA));

        surface.draw(scrollBg);
        surface.draw(scrollBar);
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
                sf::Vector2f itemPos(getRelativePos(sf::Vector2f(_x, _y + (ITEM_SPACING * _gamepadSelectedItemIndex))));
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

            drawAdditionalTooltip(surface, mousedOverItemIndex);
        }
    } catch (std::exception ex) {
        MessageManager::displayMessage("Error in UIInventoryInterface::draw: " + (std::string)ex.what(), 5, ERR);
        return;
    }

    subDraw(surface);
}

void UIInventoryInterface::subDraw(sf::RenderTexture& surface) {}

void UIInventoryInterface::drawAdditionalTooltip(sf::RenderTexture& surface, int mousedOverItemIndex) {}

bool UIInventoryInterface::isItemCorrectType(EQUIPMENT_TYPE type) {
    if (getFilter() != FILTER_TYPE::NONE) {
        if (getFilter() == FILTER_TYPE::APPAREL
            && (type == EQUIPMENT_TYPE::CLOTHING_HEAD
                || type == EQUIPMENT_TYPE::CLOTHING_BODY
                || type == EQUIPMENT_TYPE::CLOTHING_LEGS
                || type == EQUIPMENT_TYPE::CLOTHING_FEET)) return true;
        else if (getFilter() == FILTER_TYPE::WEAPONS
            && (type == EQUIPMENT_TYPE::TOOL)) return true;
        else if (getFilter() == FILTER_TYPE::AMMO
            && (type == EQUIPMENT_TYPE::AMMO)) return true;
        else if (getFilter() == FILTER_TYPE::MISC
            && (type == EQUIPMENT_TYPE::NOT_EQUIPABLE)) return true;
        else return false;
    }

    return true;
}

void UIInventoryInterface::useItem(int index) {
    std::shared_ptr<const Item> item = Item::ITEMS[_source.getItemIdAt(index)];

    if (item->isConsumable()) {
        const int hpBeforeUse = _source.getParent()->getHitPoints();

        if (item->use(_source.getParent())) _source.removeItemAt(index, 1);

        const int hpAfterUse = _source.getParent()->getHitPoints();
        if (hpAfterUse > hpBeforeUse && item->hasTag("food") && _source.getEquippedItemId(EQUIPMENT_TYPE::CLOTHING_HEAD) == Item::getIdFromName("Chef's Hat")) {
            const int healAmount = hpAfterUse - hpBeforeUse;
            _source.getParent()->heal((float)healAmount * 0.25);
        }
    } else if (item->getEquipmentType() != EQUIPMENT_TYPE::NOT_EQUIPABLE) {
        if (_source.isEquipped(index)) {
            _source.deEquip(item->getEquipmentType());
        } else {
            _source.equip(index, item->getEquipmentType());
        }
    }
}

void UIInventoryInterface::dropItem(int index) {
    _source.dropItem(_source.getItemIdAt(index), 1);
    _source.removeItemAt(index, 1);
}

void UIInventoryInterface::dropStack(int index) {
    _source.dropItem(_source.getItemIdAt(index), _source.getItemAmountAt(index));
    _source.removeItemAt(index, _source.getItemAmountAt(index));
}

void UIInventoryInterface::controllerButtonReleased(GAMEPAD_BUTTON button) {
    if (_source.getParent()->isReloading()) return;

    int unFilteredIndex = 0;
    int filteredIndex = 0;
    if (getFilter() != FILTER_TYPE::NONE) {
        for (int i = 0; i < (int)_source.getCurrentSize(); i++) {
            if (!isItemCorrectType(Item::ITEMS[_source.getItemIdAt(i)]->getEquipmentType())) continue;
            if (filteredIndex == _gamepadSelectedItemIndex) {
                unFilteredIndex = i;
                break;
            }

            filteredIndex++;
        }
    } else unFilteredIndex = _gamepadSelectedItemIndex;

    if (_gamepadSelectedItemIndex != -1 && _gamepadSelectedItemIndex < (int)_source.getCurrentSize()) {
        switch (button) {
            case GAMEPAD_BUTTON::A:
                useItem(unFilteredIndex);
                break;
            case GAMEPAD_BUTTON::LEFT_STICK:
                _gamepadShowTooltip = !_gamepadShowTooltip;
                break;
            case GAMEPAD_BUTTON::Y:
                dropItem(unFilteredIndex);
                break;
            case GAMEPAD_BUTTON::RIGHT_STICK:
                dropStack(unFilteredIndex);
                break;
        }
    }
}

void UIInventoryInterface::controllerButtonPressed(GAMEPAD_BUTTON button) {
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

void UIInventoryInterface::gamepadScrollDown() {
    if (_source.getCurrentSize() > 0 && _gamepadSelectedItemIndex < (int)_source.getCurrentSize() - 1) {
        _gamepadSelectedItemIndex++;

        if (_gamepadSelectedItemIndex >= 12 && (int)_source.getCurrentSize() > 13) {
            _y -= ITEM_SPACING;
        }

        unfilterGamepadIndex();
    }
}

void UIInventoryInterface::gamepadScrollUp() {
    if (_source.getCurrentSize() > 0 && _gamepadSelectedItemIndex > 0) {
        _gamepadSelectedItemIndex--;

        if (_gamepadSelectedItemIndex >= 11 && (int)_source.getCurrentSize() > 13) {
            _y += ITEM_SPACING;
        }

        unfilterGamepadIndex();
    }
}

void UIInventoryInterface::unfilterGamepadIndex() {
    int filteredIndex = 0;
    if (getFilter() != FILTER_TYPE::NONE) {
        for (int i = 0; i < (int)_source.getCurrentSize(); i++) {
            if (!isItemCorrectType(Item::ITEMS[_source.getItemIdAt(i)]->getEquipmentType())) continue;
            if (filteredIndex == _gamepadSelectedItemIndex) {
                _gamepadUnfilteredSelectedItemIndex = i;
                break;
            }

            filteredIndex++;
        }
    } else _gamepadUnfilteredSelectedItemIndex = _gamepadSelectedItemIndex;
}

void UIInventoryInterface::mouseButtonPressed(const int mx, const int my, const int button) {
    if (button == sf::Mouse::Left) {
        int totalDisplayedItems = 0;
        for (int i = 0; i < _source.getCurrentSize(); i++) {
            std::shared_ptr<const Item> item = Item::ITEMS[_source.getItemIdAt(i)];
            if (isItemCorrectType(item->getEquipmentType())) totalDisplayedItems++;
        }

        if (totalDisplayedItems + 1 > 13) {
            constexpr float headerPadding = 8.f;

            sf::FloatRect scrollBar;
            float scrollBarMinHeight = 2.f;

            sf::Vector2f scrollBarPos = getRelativePos(_x + 22.5f, _originalY);
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

void UIInventoryInterface::mouseButtonReleased(const int mx, const int my, const int button) {
    if (_source.getParent()->isReloading()) return;

    int filteredIndex = 0;
    for (int i = 0; i < _source.getCurrentSize(); i++) {
        if (!isItemCorrectType(Item::ITEMS[_source.getItemIdAt(i)]->getEquipmentType())) continue;

        sf::Vector2f itemPos(getRelativePos(sf::Vector2f(_x, _y + (ITEM_SPACING * filteredIndex))));
        sf::IntRect itemBounds(itemPos.x - (_width / 8), itemPos.y - (_width / 8), (_width + (_width / 8) * 2) * 5.5f, _height + (_height / 8) * 2);
        filteredIndex++;

        if (itemBounds.contains(mx, my)) {
            switch (button) {
            case sf::Mouse::Left:
                useItem(i);
                break;
            case sf::Mouse::Right:
                dropItem(i);
                break;
            case sf::Mouse::Middle:
                dropStack(i);
                break;
            }

            if (_source.getCurrentSize() == 0) _y = _originalY;
        }
    }

    _userIsDraggingScrollbar = false;
}

void UIInventoryInterface::mouseMoved(const int mx, const int my) {
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
        constexpr float headerPadding = 8.f;

        sf::FloatRect scrollBar;
        float scrollBarMinHeight = 2.f;

        sf::Vector2f scrollBarPos = getRelativePos(_x + 22.5f, _originalY);
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

void UIInventoryInterface::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
    // Check if mouse is within the inventory UI (for shop UI, because there's two menus)
    sf::FloatRect uiBounds(_background.getPosition().x, _background.getPosition().y, _background.getSize().x, _background.getSize().y);
    if (!uiBounds.contains(mouseWheelScroll.x, mouseWheelScroll.y)) return;

    if (_source.getCurrentSize() != 0) _y += mouseWheelScroll.delta * SCROLL_RATE;
}

void UIInventoryInterface::textEntered(const sf::Uint32 character) {}

void UIInventoryInterface::hide() {
    _isActive = false;
    _gamepadSelectedItemIndex = -1;
    //_gamepadShowTooltip = false;
    _y = _originalY;
    setFilter(FILTER_TYPE::NONE);
}

void UIInventoryInterface::setFilter(FILTER_TYPE filter) {
    _filter = filter;
}

FILTER_TYPE UIInventoryInterface::getFilter() {
    return _filter;
}

void UIInventoryInterface::buttonPressed(std::string buttonCode) {
    _y = _originalY;
    if (buttonCode == "filter_none") setFilter(FILTER_TYPE::NONE);
    else if (buttonCode == "filter_apparel") setFilter(FILTER_TYPE::APPAREL);
    else if (buttonCode == "filter_weapons") setFilter(FILTER_TYPE::WEAPONS);
    else if (buttonCode == "filter_ammo") setFilter(FILTER_TYPE::AMMO);
    else if (buttonCode == "filter_misc") setFilter(FILTER_TYPE::MISC);
}

void UIInventoryInterface::setSource(Inventory& sourceInventory) {
    _source = sourceInventory;
}

Inventory& UIInventoryInterface::getSource() {
    return _source;
}

