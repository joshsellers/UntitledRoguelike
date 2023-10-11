#ifndef _UI_MENU_H
#define _UI_MENU_H

#include <vector>
#include <memory>
#include "UIElement.h"

class UIMenu {
public:
    UIMenu();

    void update();
    void draw(sf::RenderTexture& surface);

    void addElement(std::shared_ptr<UIElement> element);

    void keyPressed(sf::Keyboard::Key& key);
    void keyReleased(sf::Keyboard::Key& key);
    void mouseButtonPressed(const int mx, const int my, const int button);
    void mouseButtonReleased(const int mx, const int my, const int button);
    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);

    void textEntered(sf::Uint32 character);

    void show();
    void hide();

    bool isActive() const;
private:
    bool _isActive = false;

    std::vector<std::shared_ptr<UIElement>> _elements;
};

#endif 
