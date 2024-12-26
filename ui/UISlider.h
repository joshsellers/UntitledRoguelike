#ifndef _UI_SLIDER_H
#define _UI_SLIDER_H

#include "UIElement.h"
#include "UIButtonListener.h"

class UISlider : public UIElement {
public:
    UISlider(const std::string label, float x, float y, float width, sf::Font font, std::string id);

    void update();
    void draw(sf::RenderTexture& surface);

    void mouseButtonPressed(const int mx, const int my, const int button);
    void mouseButtonReleased(const int mx, const int my, const int button);

    void controllerButtonPressed(GAMEPAD_BUTTON button);
    void controllerButtonReleased(GAMEPAD_BUTTON button);

    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    void keyReleased(sf::Keyboard::Key& key);
    void textEntered(const sf::Uint32 character);

    void setValue(float value);
    float getValue() const;

    const std::string getId() const;

    void setListener(UIButtonListener* listener);

private:
    std::string _id = "";

    float _lastValue = 0;
    float _value = 0;

    sf::Vector2i _mPos;
    bool _mouseDown = false;
    bool _mouseSelected = false;

    UIButtonListener* _listener = nullptr;

    sf::RectangleShape _bar;
    sf::RectangleShape _handle;    
    
    sf::IntRect _defaultTexture;
    sf::IntRect _hoverTexture;
    sf::IntRect _clickTexture;

    sf::RenderTexture _rTexture;
};

#endif