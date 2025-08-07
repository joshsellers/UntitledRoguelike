#ifndef _UI_CONTROLS_DISPLAY_H
#define _UI_CONTROLS_DISPLAY_H

#include "UIElement.h"

struct ControlLabel {
    ControlLabel(const GAMEPAD_BUTTON button, const std::string text, const bool press = false) 
        : button(button), text(text), press(press) {}

    const GAMEPAD_BUTTON button;
    const std::string text;
    // if press is true, offset texture Y by +2
    const bool press;
};

class UIControlsDisplay : public UIElement {
public:
    UIControlsDisplay(sf::Vector2f pos, sf::Font font);

    void update();
    void draw(sf::RenderTexture& surface);

    void addLabel(GAMEPAD_BUTTON button, std::string text, bool press = false);

    void setFontSize(float fontSize);
    void setFontColor(sf::Uint32 fontColor);
    void setIconSize(float iconSize);

    static sf::IntRect getButtonIcon(GAMEPAD_BUTTON button, bool stickPressed = false);

    void controllerButtonPressed(GAMEPAD_BUTTON button);
    virtual void controllerButtonReleased(GAMEPAD_BUTTON button);

    void mouseButtonPressed(const int mx, const int my, const int button);
    void mouseButtonReleased(const int mx, const int my, const int button);
    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    void textEntered(const sf::Uint32 character);
protected:

private:
    static inline const std::map<GAMEPAD_BUTTON, sf::Vector2i> _buttonIcons = {
        { GAMEPAD_BUTTON::A, sf::Vector2i(0, 13) },
        { GAMEPAD_BUTTON::B, {3, 13} },
        { GAMEPAD_BUTTON::Y, {1, 13} },
        { GAMEPAD_BUTTON::X, {2, 13} },
        { GAMEPAD_BUTTON::START, {2, 16} },
        { GAMEPAD_BUTTON::SELECT, {1, 16} },
        { GAMEPAD_BUTTON::LEFT_STICK, {1, 12} },
        { GAMEPAD_BUTTON::RIGHT_STICK, {0, 12} },
        { GAMEPAD_BUTTON::LEFT_BUMPER, {0, 15} },
        { GAMEPAD_BUTTON::RIGHT_BUMPER, {1, 15} },
        { GAMEPAD_BUTTON::LEFT_TRIGGER, {3, 16} },
        { GAMEPAD_BUTTON::RIGHT_TRIGGER, {0, 16} },
        { GAMEPAD_BUTTON::DPAD_UP, {2, 12} },
        { GAMEPAD_BUTTON::DPAD_DOWN, {3, 12} },
        { GAMEPAD_BUTTON::DPAD_LEFT, {4, 12} },
        { GAMEPAD_BUTTON::DPAD_RIGHT, {4, 13} }
    };

    std::vector<std::shared_ptr<ControlLabel>> _labels;

    float _fontSize = 1.5f;
    sf::Uint32 _fontColor = 0xFFFFFFFF;
    float _iconSize = 2.f;
};

#endif