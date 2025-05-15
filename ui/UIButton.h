#ifndef _UI_BUTTON_H
#define _UI_BUTTON_H

#include "UIElement.h"
#include "UIButtonListener.h"

const sf::Uint32 BUTTON_COLOR[] = { 0xC7, 0x77, 0x1A };
const sf::Uint32 BUTTON_COLOR_BORDER[] = { 0xA3, 0x62, 0x15 };
const sf::Uint32 BUTTON_COLOR_BORDER_HOVER[] = {
    BUTTON_COLOR[0],
    BUTTON_COLOR[1],
    BUTTON_COLOR[2]
};
const sf::Uint32 BUTTON_COLOR_CLICK[] = {
    BUTTON_COLOR_BORDER[0],
    BUTTON_COLOR_BORDER[1],
    BUTTON_COLOR_BORDER[2]
};

class UIButton : public UIElement {
public:
    UIButton(float x, float y, float width, float height, sf::String labelText,
        sf::Font font, UIButtonListener* listener, std::string code, bool centerOnCoords = false);

    virtual void update();
    void draw(sf::RenderTexture& surface);

    void setLabelText(std::string labelText);

    void controllerButtonPressed(GAMEPAD_BUTTON button);
    virtual void controllerButtonReleased(GAMEPAD_BUTTON button);

    void mouseButtonPressed(const int mx, const int my, const int button);
    virtual void mouseButtonReleased(const int mx, const int my, const int button);
    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    void textEntered(const sf::Uint32 character);

    bool pressWhenSelected = false;

    virtual void show();
protected:
    sf::IntRect getHoverTexture();
    sf::IntRect getClickTexture();
    sf::IntRect getDefaultTexture();

    sf::Vector2i getMousePos();
    bool _mouseDown = false;
private:
    int _mx = 0, _my = 0;

    UIButtonListener* _listener = nullptr;
    std::string _buttonCode = "";

    bool _wasJustSelected = false;

    sf::IntRect _defaultTexture;
    sf::IntRect _hoverTexture;
    sf::IntRect _clickTexture;

    sf::RenderTexture _rTexture;
};

#endif
