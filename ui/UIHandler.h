#ifndef _UI_HANDLER_H
#define _UI_HANDLER_H

#include <vector>
#include <memory>
#include "UIMenu.h"

class UIHandler : public GamePadListener {
public:
    UIHandler();

    void update();
    void draw(sf::RenderTexture& surface);

    void addMenu(std::shared_ptr<UIMenu> menu);

    void controllerButtonReleased(GAMEPAD_BUTTON button);
    void controllerButtonPressed(GAMEPAD_BUTTON button);
    void gamepadDisconnected();

    void keyPressed(sf::Keyboard::Key& key);
    void keyReleased(sf::Keyboard::Key& key);
    void mouseButtonPressed(const int mx, const int my, const int button);
    void mouseButtonReleased(const int mx, const int my, const int button);
    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);

    void textEntered(sf::Uint32 character);

    static std::shared_ptr<sf::Texture> getUISpriteSheet();

private:
    std::vector<std::shared_ptr<UIMenu>> _menus;

    static inline std::shared_ptr<sf::Texture> _spriteSheet = std::shared_ptr<sf::Texture>(new sf::Texture());
};

#endif
