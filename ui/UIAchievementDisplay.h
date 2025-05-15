#ifndef _UI_ACHIEVEMENT_DISPLAY_H
#define _UI_ACHIEVEMENT_DISPLAY_H

#include "UIElement.h"

class UIAchievementDisplay : public UIElement {
public:
    UIAchievementDisplay(float x, float y, int unlockedCount, int unlockableCount, sf::Font);

    void update();
    void draw(sf::RenderTexture& surface);

    void controllerButtonReleased(GAMEPAD_BUTTON button);
    void controllerButtonPressed(GAMEPAD_BUTTON button);

    void mouseButtonPressed(const int mx, const int my, const int button);
    void mouseButtonReleased(const int mx, const int my, const int button);
    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    void textEntered(const sf::Uint32 character);

    void show();
    void hide();

protected:
    sf::Vector2f _mousePos;

    sf::RectangleShape _background;
    sf::RectangleShape _headerBg;

    sf::Text _tooltipText;
    sf::RectangleShape _tooltipBg;

    const float _originalY;

    int _gamepadSelectedItemIndex = -1;
    int _gamepadUnfilteredSelectedItemIndex = -1;
    bool _gamepadShowTooltip = true;

    long long _lastDPadPressTime = 0LL;

    bool _userIsDraggingScrollbar = false;
    float _mouseYWhenClickedInScrollbar = 0.f;
    float _scrollBarPosY = 0.f;
    bool _mousedOverScrollbar = false;

    sf::RenderTexture _scrollBarRTexture;

    bool _itemsOutsideWindow = false;

    void gamepadScrollDown();
    void gamepadScrollUp();
    void unfilterGamepadIndex();

private:
    int _unlockedCount;
    int _totalCount;

    std::vector<std::string> _achievementNames;
    std::vector <std::shared_ptr<sf::Texture>> _textures;

    void loadAchivementResources();
};

#endif