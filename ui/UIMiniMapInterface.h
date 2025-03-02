#ifndef _UI_MINI_MAP_INTERFACE_H
#define _UI_MINI_MAP_INTERFACE_H

#include "UIElement.h"
#include "../world/entities/Player.h"

class UIMiniMapInterface : public UIElement {
public:
    UIMiniMapInterface(Player* player, sf::Font font);

    void update();
    void draw(sf::RenderTexture& surface);

    void controllerButtonPressed(GAMEPAD_BUTTON button);
    virtual void controllerButtonReleased(GAMEPAD_BUTTON button);

    void mouseButtonPressed(const int mx, const int my, const int button);
    virtual void mouseButtonReleased(const int mx, const int my, const int button);
    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    void textEntered(const sf::Uint32 character);
private:
    int _dispSize;
    int _dispScale;
    unsigned int _cameraX = 0;
    unsigned int _cameraY = 0;

    sf::RectangleShape _shopIcon;
    sf::RectangleShape _pinIcon;
    sf::RectangleShape _playerIcon;

    Player* _player;

    sf::Vector2i _mPosOnClick;
    sf::Vector2i _cameraPosOnClick;
    bool _middleButtonPressed = false;

    void centerOnPlayer();
    void zoom(float factor);

    long long _lastDpadPressTime = 0LL;
};

#endif