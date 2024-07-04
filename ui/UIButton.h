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
        sf::Font font, UIButtonListener* listener, std::string code);

    void update();
    void draw(sf::RenderTexture& surface);

    void setLabelText(std::string labelText);

    void controllerButtonPressed(GAMEPAD_BUTTON button);
    void controllerButtonReleased(GAMEPAD_BUTTON button);

    void mouseButtonPressed(const int mx, const int my, const int button);
    void mouseButtonReleased(const int mx, const int my, const int button);
    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    void textEntered(const sf::Uint32 character);

    bool pressWhenSelected = false;

private:
    bool _mouseDown = false;
    int _mx = 0, _my = 0;

    std::shared_ptr<sf::Texture> _hoverTexture =
        std::shared_ptr<sf::Texture>(new sf::Texture());
    std::shared_ptr<sf::Texture> _clickTexture =
        std::shared_ptr<sf::Texture>(new sf::Texture());

    UIButtonListener* _listener = nullptr;
    std::string _buttonCode = "";

    bool _wasJustSelected = false;
};

#endif
