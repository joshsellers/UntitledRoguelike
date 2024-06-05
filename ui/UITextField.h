#ifndef _UI_TEXT_FIELD_H
#define _UI_TEXT_FIELD_H

#include "UIElement.h"

class UITextField : public UIElement {
public:
    UITextField(const sf::String label, float x, float y, sf::Font font);

    void update();
    void draw(sf::RenderTexture& surface);

    void setDefaultText(sf::String text);

    void mouseButtonPressed(const int mx, const int my, const int button);
    void mouseButtonReleased(const int mx, const int my, const int button);

    void controllerButtonPressed(GAMEPAD_BUTTON button);
    void controllerButtonReleased(GAMEPAD_BUTTON button);

    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    void keyReleased(sf::Keyboard::Key& key);
    void textEntered(const sf::Uint32 character);

    std::string getText() const;

    void setCharacterSize(float size);
private:
    sf::Text _label;
    sf::RectangleShape _bg;

    sf::Vector2i _mPos;
    bool _mouseDown = false;

    bool _lastSelected = false;

    bool _isArmed = false;
};

#endif