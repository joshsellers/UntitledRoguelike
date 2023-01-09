#include "UIInventoryInterface.h"
#include "Item.h"
#include <iostream>

UIInventoryInterface::UIInventoryInterface(Inventory& source, sf::Font font, std::shared_ptr<sf::Texture> spriteSheet) :
    _source(source), _spriteSheet(spriteSheet), UIElement(5, 11, 3, 3, false, true, font), _originalY(_y) {
    _disableAutomaticTextAlignment = true;
    
    float fontSize = 4;
    int relativeFontSize = (float)WINDOW_WIDTH * (fontSize / 100);
    _text.setFont(_font);
    _text.setCharacterSize(relativeFontSize);
    _text.setFillColor(sf::Color::White);
    _text.setString("INVENTORY");
    sf::Vector2f basePos(getRelativePos(sf::Vector2f(_x, _y)));
    _text.setPosition(basePos.x - _text.getGlobalBounds().width / 8, 0);

    fontSize = 1;
    relativeFontSize = (float)WINDOW_WIDTH * (fontSize / 100);
    _tooltipText.setFont(_font);
    _tooltipText.setCharacterSize(relativeFontSize);
    _tooltipText.setFillColor(sf::Color(0x000023FF));

    _tooltipBg.setFillColor(sf::Color(0xFFFFCAFF));
    _tooltipBg.setOutlineColor(sf::Color(0xEEEEB9FF));
    _tooltipBg.setOutlineThickness(getRelativeSize(0.25f));
}

void UIInventoryInterface::update() {}

void UIInventoryInterface::draw(sf::RenderTexture& surface) {
    int mousedOverItemIndex = -1;
    for (int i = 0; i < _source.getCurrentSize(); i++) {
        const Item* item = Item::ITEMS[_source.getItemIdAt(i)];
        sf::Vector2f itemPos(getRelativePos(sf::Vector2f(_x, _y + (ITEM_SPACING * i))));

        sf::Text label;
        float fontSize = 1.5;
        int relativeFontSize = (float)WINDOW_WIDTH * (fontSize / 100);
        label.setFont(_font);
        label.setCharacterSize(relativeFontSize);
        label.setFillColor(sf::Color::White);
        label.setPosition(itemPos.x - (_width / 8) + _width + (_width / 8) * 3, itemPos.y + _height / 6);
        label.setString(item->getName() + (
            item->isStackable() ? " (" + std::to_string(_source.getItemAmountAt(i)) + ")" : ""
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

        if (bg.getGlobalBounds().contains(_mousePos)) {
            bg.setFillColor(sf::Color(0x0000FFFF));
        } else bg.setFillColor(sf::Color(0x000099FF));

        surface.draw(labelBg);
        surface.draw(bg);
        surface.draw(itemSprite);
        surface.draw(label);

        if (labelBg.getGlobalBounds().contains(_mousePos) && !bg.getGlobalBounds().contains(_mousePos))
            mousedOverItemIndex = i;
    }

    if (mousedOverItemIndex >= 0) {
        const Item* item = Item::ITEMS[_source.getItemIdAt(mousedOverItemIndex)];

        float textXOffset = (float)WINDOW_WIDTH * (2.f / 100);

        _tooltipText.setString(item->getDescription());
        float textWidth = _tooltipText.getGlobalBounds().width;
        float textHeight = _tooltipText.getGlobalBounds().height;
        sf::Vector2f pos(_mousePos.x + textXOffset, _mousePos.y - textHeight / 2);
        _tooltipText.setPosition(pos);

        float padding = (float)WINDOW_HEIGHT * (1.f / 100);

        float bgWidth = textWidth + padding * 2;
        float bgHeight = textHeight + padding * 2;
        _tooltipBg.setPosition(pos.x - padding, pos.y - padding);
        _tooltipBg.setSize(sf::Vector2f(bgWidth, bgHeight));

        surface.draw(_tooltipBg);
        surface.draw(_tooltipText);
    }
}

void UIInventoryInterface::mouseButtonPressed(const int mx, const int my, const int button) {}

void UIInventoryInterface::mouseButtonReleased(const int mx, const int my, const int button) {
    for (int i = 0; i < _source.getCurrentSize(); i++) {
        sf::Vector2f itemPos(getRelativePos(sf::Vector2f(_x, _y + (ITEM_SPACING * i))));
        sf::IntRect itemBounds(itemPos.x - (_width / 8), itemPos.y - (_width / 8), _width + (_width / 8) * 2, _height + (_height / 8) * 2);

        if (itemBounds.contains(mx, my)) {
            const Item* item = Item::ITEMS[_source.getItemIdAt(i)];

            switch (button) {
            case sf::Mouse::Left:
                if (item->isConsumable()) {
                    item->use(_source.getParent());
                    _source.removeItemAt(i, 1);
                } else if (item->getEquipmentType() != EQUIPMENT_TYPE::NOT_EQUIPABLE) {
                    if (_source.isEquipped(i)) {
                        _source.deEquip(item->getEquipmentType());
                    } else {
                        _source.equip(i, item->getEquipmentType());
                    }
                }
                break;
            case sf::Mouse::Right:
                _source.dropItem(_source.getItemIdAt(i), 1);
                _source.removeItemAt(i, 1);
                break;
            case sf::Mouse::Middle:
                _source.dropItem(_source.getItemIdAt(i), _source.getItemAmountAt(i));
                _source.removeItemAt(i, _source.getItemAmountAt(i));
                break;
            }

            if (_source.getCurrentSize() == 0) _y = _originalY;
        }
    }
}

void UIInventoryInterface::mouseMoved(const int mx, const int my) {
    _mousePos = sf::Vector2f(mx, my);
}

void UIInventoryInterface::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
    if (_source.getCurrentSize() != 0) _y += mouseWheelScroll.delta * SCROLL_RATE;
}

void UIInventoryInterface::textEntered(const sf::Uint32 character) {}

void UIInventoryInterface::hide() {
    _isActive = false;
    _y = _originalY;
}
