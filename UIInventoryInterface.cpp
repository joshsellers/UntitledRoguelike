#include "UIInventoryInterface.h"
#include "Item.h"
#include <iostream>

UIInventoryInterface::UIInventoryInterface(Inventory& source, sf::Font font, std::shared_ptr<sf::Texture> spriteSheet) :
    _source(source), _spriteSheet(spriteSheet), UIElement(49, 49, 3, 3, false, true, font) {
    _disableAutomaticTextAlignment = true;

    float fontSize = 4;
    int relativeFontSize = (float)WINDOW_WIDTH * (fontSize / 100);
    _text.setFont(_font);
    _text.setCharacterSize(relativeFontSize);
    _text.setFillColor(sf::Color::White);
    _text.setString("INVENTORY");
    sf::Vector2f basePos(getRelativePos(sf::Vector2f(_x, _y)));
    _text.setPosition(basePos.x - _text.getGlobalBounds().width / 2, 0);
}

void UIInventoryInterface::update() {
}

void UIInventoryInterface::draw(sf::RenderTexture& surface) {
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
        labelBg.setFillColor(sf::Color(0x000066FF));

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
    }
}

void UIInventoryInterface::mouseButtonPressed(const int mx, const int my, const int button) {
}

void UIInventoryInterface::mouseButtonReleased(const int mx, const int my, const int button) {
    for (int i = 0; i < _source.getCurrentSize(); i++) {
        sf::Vector2f itemPos(getRelativePos(sf::Vector2f(_x, _y + (ITEM_SPACING * i))));
        sf::IntRect itemBounds(itemPos.x - (_width / 8), itemPos.y - (_width / 8), _width + (_width / 8) * 2, _height + (_height / 8) * 2);

        if (itemBounds.contains(mx, my)) {
            switch (button) {
            case sf::Mouse::Left:

                break;
            case sf::Mouse::Right:
                _source.removeItemAt(i, 1);
                break;
            case sf::Mouse::Middle:

                break;
            }
        }
    }
}

void UIInventoryInterface::mouseMoved(const int mx, const int my) {
    _mousePos = sf::Vector2f(mx, my);
}

void UIInventoryInterface::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
    _y += mouseWheelScroll.delta;
}

void UIInventoryInterface::textEntered(const sf::Uint32 character) {
}
