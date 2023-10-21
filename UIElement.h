#ifndef _UI_ELEMENT_H
#define _UI_ELEMENT_H

#include <memory>
#include "Globals.h"
#include <SFML/Graphics.hpp>
#include "GameControllerButtons.h"

class UIElement {
public:
    static sf::Vector2f getRelativePos(sf::Vector2f pos);
    static sf::Vector2f getRelativePos(float x, float y);
    static float getRelativeWidth(float size);
    static float getRelativeHeight(float size);

    UIElement(float x, float y, float width, float height, bool drawSprite, bool drawText,
        sf::Font font);

    virtual void update() = 0;
    virtual void draw(sf::RenderTexture& surface) = 0;
    void render(sf::RenderTexture& surface);

    virtual void controllerButtonReleased(CONTROLLER_BUTTON button);
    virtual void controllerButtonPressed(CONTROLLER_BUTTON button);

    virtual void keyPressed(sf::Keyboard::Key& key);
    virtual void keyReleased(sf::Keyboard::Key& key);
    virtual void mouseButtonPressed(const int mx, const int my, const int button) = 0;
    virtual void mouseButtonReleased(const int mx, const int my, const int button) = 0;
    virtual void mouseMoved(const int mx, const int my) = 0;
    virtual void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) = 0;
    virtual void textEntered(const sf::Uint32 character) = 0;

    virtual void show();
    virtual void hide();
    bool isActive() const;

    void setSelectionId(int selectionId);
    int getSelectionId();

    virtual void setCharacterSize(float size);

    friend class UIMenu;

protected:
    float _x, _y;
    float _width, _height;

    std::shared_ptr<sf::Texture> _texture =
        std::shared_ptr<sf::Texture>(new sf::Texture());
    sf::Sprite _sprite;

    sf::Text _text;
    sf::Font _font;

    bool _disableAutomaticTextAlignment = false;

    bool _isActive = false;

    bool _isSelected = false;
    int _selectionId;

private:
    bool _drawSprite = true;
    bool _drawText = true;
};

#endif
