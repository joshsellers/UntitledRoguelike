#include "UIAchievementDisplay.h"

#include "../core/gamepad/GamePad.h"
#include "UIHandler.h"

#include "../statistics/LocalAchievementManager.h"

constexpr int ITEM_SPACING = 7;
constexpr float SCROLL_RATE = 2.f;

constexpr long long DPAD_HOLD_TIME = 250LL;

UIAchievementDisplay::UIAchievementDisplay(float x, float y, int unlockedCount, int unlockableCount, sf::Font font) :
    UIElement(x, y, 3, 3, false, false, font), _originalY(_y) {

    _unlockedCount = unlockedCount;
    _totalCount = unlockableCount;

    _disableAutomaticTextAlignment = true;

    float fontSize = 2.f;
    int relativeFontSize = (float)WINDOW_WIDTH * (fontSize / 100);
    _text.setFont(_font);
    _text.setCharacterSize(relativeFontSize);
    _text.setFillColor(sf::Color(0x81613DFF));
    _text.setString(std::to_string(unlockedCount) + "/" + std::to_string(unlockableCount) + " achievements");
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

    _background.setPosition(getRelativePos(_x - 1.5f, 0.9f));
    _background.setSize(sf::Vector2f(getRelativeWidth(25.f), getRelativeHeight(98.3f)));
    _background.setTexture(UIHandler::getUISpriteSheet().get());
    _background.setTextureRect(sf::IntRect(0, 32, 80, 160));

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

    loadAchivementResources();
}

void UIAchievementDisplay::update() {
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

void UIAchievementDisplay::draw(sf::RenderTexture& surface) {
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
    for (int i = 0; i < _unlockedCount; i++) {
        sf::Vector2f itemPos(getRelativePos(sf::Vector2f(_x, _y + (ITEM_SPACING * filteredIndex) + 1.f)));

        sf::Text label;
        float fontSize = 1.25;
        int relativeFontSize = (float)WINDOW_WIDTH * (fontSize / 100);
        label.setFont(_font);
        label.setCharacterSize(relativeFontSize);
        label.setFillColor(sf::Color::White);
        label.setPosition(itemPos.x - (_width / 8) + _width + (_width / 8) * 3, itemPos.y + _height / 6);
        std::string itemName = _achievementNames.at(i);

        label.setString(itemName);

        sf::RectangleShape labelBg(sf::Vector2f(_width + (_width / 8) * 3 + label.getGlobalBounds().width + (_width / 8), _height + (_height / 8) * 2));
        labelBg.setPosition(sf::Vector2f(itemPos.x - (_width / 8), itemPos.y - (_width / 8)));
        sf::Uint32 labelBgColor = 0x000066FF;
        labelBg.setFillColor(sf::Color(labelBgColor));

        sf::RectangleShape bg(sf::Vector2f(_width + (_width / 8) * 2, _height + (_height / 8) * 2));
        bg.setPosition(sf::Vector2f(itemPos.x - (_width / 8), itemPos.y - (_width / 8)));
        bg.setTexture(UIHandler::getUISpriteSheet().get());

        sf::Sprite itemSprite;
        itemSprite.setTexture(*_textures.at(i));
        itemSprite.setScale(sf::Vector2f((_width) / itemSprite.getGlobalBounds().width, (_height) / itemSprite.getGlobalBounds().height));
        itemSprite.setPosition(itemPos);

        const sf::FloatRect interactionBox(itemPos.x - (_width / 8), itemPos.y - (_width / 8), (_width + (_width / 8) * 2) * 5.5f, _height + (_height / 8) * 2);
        const bool touchingItem = interactionBox.contains(_mousePos) || filteredIndex == _gamepadSelectedItemIndex;

        if (touchingItem) {
            bg.setTextureRect(sf::IntRect(224, 64, 24, 24));
            surface.draw(bg);
        }

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


        scrollBar.setPosition(scrollBarPos.x, scrollBarPos.y + getRelativeHeight(headerPadding));
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

        surface.draw(scrollBar);
        surface.draw(handleTop);
        surface.draw(handleBottom);
    }

    surface.draw(_headerBg);
    surface.draw(_text);
}

void UIAchievementDisplay::controllerButtonReleased(GAMEPAD_BUTTON button) {

}

void UIAchievementDisplay::controllerButtonPressed(GAMEPAD_BUTTON button) {
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

void UIAchievementDisplay::gamepadScrollDown() {
    if (_unlockedCount > 0 && _gamepadSelectedItemIndex < (int)_unlockedCount - 1) {
        _gamepadSelectedItemIndex++;

        if (_gamepadSelectedItemIndex >= 12 && (int)_unlockedCount > 13) {
            _y -= ITEM_SPACING;
        }

        unfilterGamepadIndex();
    }
}

void UIAchievementDisplay::gamepadScrollUp() {
    if (_unlockedCount > 0 && _gamepadSelectedItemIndex > 0) {
        _gamepadSelectedItemIndex--;

        if (_gamepadSelectedItemIndex >= 11 && (int)_unlockedCount > 13) {
            _y += ITEM_SPACING;
        }

        unfilterGamepadIndex();
    }
}

void UIAchievementDisplay::unfilterGamepadIndex() {
    _gamepadUnfilteredSelectedItemIndex = _gamepadSelectedItemIndex;
}

void UIAchievementDisplay::loadAchivementResources() {
    for (int i = 0; i < _totalCount; i++) {
        if (LocalAchievementManager::isUnlocked((ACHIEVEMENT)i)) {
            _textures.push_back(std::shared_ptr<sf::Texture>(new sf::Texture()));
            _textures.back()->create(256, 256);
            if (!_textures.back()->loadFromFile("res/achievements/" + std::to_string(i) + ".png")) {
                MessageManager::displayMessage("Failed to load achievement image for achievement " + std::to_string(i), 10, WARN);
            }

            _achievementNames.push_back(LocalAchievementManager::getAchievementName((ACHIEVEMENT)i));
        }
    }
}

void UIAchievementDisplay::mouseButtonPressed(const int mx, const int my, const int button) {
    if (button == sf::Mouse::Left) {
        int totalDisplayedItems = 0;
        totalDisplayedItems = _unlockedCount;

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

void UIAchievementDisplay::mouseButtonReleased(const int mx, const int my, const int button) {
    _userIsDraggingScrollbar = false;
}

void UIAchievementDisplay::mouseMoved(const int mx, const int my) {
    _gamepadSelectedItemIndex = -1;
    //_gamepadShowTooltip = false;
    _mousePos = sf::Vector2f(mx, my);

    blockControllerInput = false;

    int totalDisplayedItems = _unlockedCount;

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

void UIAchievementDisplay::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
    // Check if mouse is within the inventory UI (for shop UI, because there's two menus)
    sf::FloatRect uiBounds(_background.getPosition().x, _background.getPosition().y, _background.getSize().x, _background.getSize().y);
    if (!uiBounds.contains(mouseWheelScroll.x, mouseWheelScroll.y)) return;

    if (_unlockedCount != 0) _y += mouseWheelScroll.delta * SCROLL_RATE;
}

void UIAchievementDisplay::textEntered(const sf::Uint32 character) {}

void UIAchievementDisplay::show() {
    _isActive = true;

    if (GamePad::isConnected() && _unlockedCount > 0) {
        _gamepadSelectedItemIndex = 0;
        _gamepadUnfilteredSelectedItemIndex = 0;
    }
}

void UIAchievementDisplay::hide() {
    _isActive = false;
    _gamepadSelectedItemIndex = -1;
    //_gamepadShowTooltip = false;
    _y = _originalY;
}
