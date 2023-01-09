#ifndef _UI_COMMAND_PROMPT_H
#define _UI_COMMAND_PROMPT_H

#include "UIElement.h"
#include "World.h"

class UICommandPrompt : public UIElement {
public:
    UICommandPrompt(World* world, sf::Font font);

    void update();
    void draw(sf::RenderTexture& surface);

    void mouseButtonPressed(const int mx, const int my, const int button);
    void mouseButtonReleased(const int mx, const int my, const int button);
    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    void textEntered(const sf::Uint32 character);

private:
    World* _world;

    sf::RectangleShape _bg;

    std::string processCommand(sf::String command);
};

#endif