#ifndef _UI_ELEMENT_H
#define _UI_ELEMENT_H

#include <memory>
#include "../core/Globals.h"
#include <SFML/Graphics.hpp>
#include "../core/gamepad/GamePadListener.h"
#include "UIApperanceConfig.h"
#include "AppearanceConfigs.h"

class UIElement {
public:
    static sf::Vector2f getRelativePos(sf::Vector2f pos);
    static sf::Vector2f getRelativePos(float x, float y);
    static float getRelativeWidth(float size);
    static float getRelativeHeight(float size);

    UIElement(float x, float y, float width, float height, bool drawSprite, bool drawText,
        sf::Font font, bool centerOnCoords = false, bool useAppearanceSystem = false);

    virtual void update() = 0;
    virtual void draw(sf::RenderTexture& surface) = 0;
    void render(sf::RenderTexture& surface, const sf::RenderStates& states = sf::RenderStates::Default);

    virtual void controllerButtonReleased(GAMEPAD_BUTTON button);
    virtual void controllerButtonPressed(GAMEPAD_BUTTON button);

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

    bool blockControllerInput = false;

    virtual void setCharacterSize(float size);

    virtual void setAppearance(UIAppearanceConfig appearance, bool recenter = false);

    sf::FloatRect getBounds() const;

    friend class UIMenu;

protected:
    // these two are used for the appearanceSystem
    sf::Vector2f _pos;
    sf::Vector2f _size;

    float _x, _y;
    float _width, _height;

    sf::RectangleShape _shape;

    sf::Text _text;
    sf::Font _font;

    bool _disableAutomaticTextAlignment = false;

    bool _isActive = false;

    bool _isSelected = false;
    int _selectionId;
    bool _disableMouseMovementDeselection = false;

    bool _drawSprite = true;
    bool _drawText = true;

    virtual void constructShapes();
    virtual void drawShapes(sf::RenderTexture& surface, const sf::RenderStates& states = sf::RenderStates::Default);

    bool _useAppearanceSystem = false;
    UIAppearanceConfig _appearance = BASE_COMPONENT_CONFIG;
    sf::RectangleShape _leftEdge;
    sf::RectangleShape _leftTopCorner;
    sf::RectangleShape _leftBottomCorner;
    sf::RectangleShape _center;
    sf::RectangleShape _centerTop;
    sf::RectangleShape _centerBottom;
    sf::RectangleShape _rightEdge;
    sf::RectangleShape _rightTopCorner;
    sf::RectangleShape _rightBottomCorner;
};

#endif
