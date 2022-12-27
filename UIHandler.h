#ifndef _UI_HANDLER_H
#define _UI_HANDLER_H

#include <vector>
#include <memory>
#include "UIMenu.h"

class UIHandler {
public:
    UIHandler();

    void update();
    void draw(sf::RenderTexture& surface);

    void addMenu(std::shared_ptr<UIMenu> menu);

    void mouseButtonPressed(const int mx, const int my);
    void mouseButtonReleased(const int mx, const int my);
    void mouseMoved(const int mx, const int my);

    void textEntered(sf::Uint32 character);

private:
    std::vector<std::shared_ptr<UIMenu>> _menus;
};

#endif
