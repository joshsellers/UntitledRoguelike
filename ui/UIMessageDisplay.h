#ifndef _UI_MESSAGE_DISPLAY_H
#define _UI_MESSAGE_DISPLAY_H

#include "UIElement.h"

class UIMessageDisplay : public UIElement {
public:
    UIMessageDisplay(sf::Font font);

    void update();
    void draw(sf::RenderTexture& surface);

    void mouseButtonPressed(const int mx, const int my, const int button);
    void mouseButtonReleased(const int mx, const int my, const int button);
    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    void keyReleased(sf::Keyboard::Key& key);
    void textEntered(const sf::Uint32 character);
private:

};

#endif
