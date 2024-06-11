#ifndef _UI_LABEL_H
#define _UI_LABEL_H
#include "UIElement.h"

class UILabel : public UIElement {
public:
    UILabel(const sf::String label, float x, float y, float characterSize, sf::Font font);

    void update();
    void draw(sf::RenderTexture& surface);

    void setText(sf::String text);

    void mouseButtonPressed(const int mx, const int my, const int button);
    void mouseButtonReleased(const int mx, const int my, const int button);

    void controllerButtonPressed(GAMEPAD_BUTTON button);
    void controllerButtonReleased(GAMEPAD_BUTTON button);

    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    void keyReleased(sf::Keyboard::Key& key);
    void textEntered(const sf::Uint32 character);
};

#endif

