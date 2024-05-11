#ifndef _UI_INVENTORY_INTERFACE_H
#define _UI_INVENTORY_INTERFACE_H

#include "UIElement.h"
#include "Inventory.h"
#include "UIButtonListener.h"

constexpr int ITEM_SPACING = 7;
constexpr float SCROLL_RATE = 2.f;

constexpr long long DPAD_HOLD_TIME = 250LL;

enum class FILTER_TYPE {
    NONE,
    APPAREL,
    WEAPONS,
    AMMO,
    MISC
};

class UIInventoryInterface : public UIElement, public UIButtonListener {
public:
    UIInventoryInterface(Inventory& source, sf::Font, std::shared_ptr<sf::Texture> spriteSheet);
    UIInventoryInterface(float x, float y, Inventory& source, sf::Font, std::shared_ptr<sf::Texture> spriteSheet);

    void update();
    void draw(sf::RenderTexture& surface);

    void controllerButtonReleased(CONTROLLER_BUTTON button);
    void controllerButtonPressed(CONTROLLER_BUTTON button);

    void mouseButtonPressed(const int mx, const int my, const int button);
    void mouseButtonReleased(const int mx, const int my, const int button);
    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    void textEntered(const sf::Uint32 character);

    void hide();

    void setFilter(FILTER_TYPE filter);
    FILTER_TYPE getFilter();
    virtual void buttonPressed(std::string buttonCode);

    void setSource(Inventory& sourceInventory);
    Inventory& getSource();

protected:
    Inventory& _source;

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
    bool _gamepadShowTooltip = false;

    long long _lastDPadPressTime = 0LL;

    FILTER_TYPE _filter = FILTER_TYPE::NONE;
    bool isItemCorrectType(EQUIPMENT_TYPE type);

    virtual void useItem(int index);
    virtual void dropItem(int index);
    virtual void dropStack(int index);

    void gamepadScrollDown();
    void gamepadScrollUp();
};

#endif