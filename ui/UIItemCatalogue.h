#ifndef _UI_ITEM_CATALOGUE_H
#define _UI_ITEM_CATALOGUE_H

#include "../inventory/Inventory.h"
#include "UIElement.h"

class UIItemCatalogue : public UIElement {
public:
    UIItemCatalogue(float x, float y, int unlockedCount, int unlockableCount, Inventory source, sf::Font, std::shared_ptr<sf::Texture> spriteSheet);

    void update();
    void draw(sf::RenderTexture& surface);

    void controllerButtonReleased(GAMEPAD_BUTTON button);
    void controllerButtonPressed(GAMEPAD_BUTTON button);

    void mouseButtonPressed(const int mx, const int my, const int button);
    void mouseButtonReleased(const int mx, const int my, const int button);
    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    void textEntered(const sf::Uint32 character);

    void show();
    void hide();

    void setSource(Inventory sourceInventory);
    Inventory getSource();

protected:
    Inventory _source;

    std::shared_ptr<sf::Texture> _spriteSheet;

    sf::Vector2f _mousePos;

    sf::RectangleShape _background;
    sf::RectangleShape _headerBg;

    sf::Text _tooltipText;
    sf::RectangleShape _tooltipBg;

    virtual void subDraw(sf::RenderTexture& surface);
    virtual void drawAdditionalTooltip(sf::RenderTexture& surface, int mousedOverItemIndex);

    const float _originalY;

    int _gamepadSelectedItemIndex = -1;
    int _gamepadUnfilteredSelectedItemIndex = -1;
    bool _gamepadShowTooltip = true;

    long long _lastDPadPressTime = 0LL;

    bool _userIsDraggingScrollbar = false;
    float _mouseYWhenClickedInScrollbar = 0.f;
    float _scrollBarPosY = 0.f;
    bool _mousedOverScrollbar = false;

    sf::RenderTexture _scrollBarRTexture;

    bool isItemCorrectType(EQUIPMENT_TYPE type);
    bool _itemsOutsideWindow = false;

    virtual void useItem(int index);
    virtual void dropItem(int index);
    virtual void dropStack(int index);

    void gamepadScrollDown();
    void gamepadScrollUp();
    void unfilterGamepadIndex();
};

#endif