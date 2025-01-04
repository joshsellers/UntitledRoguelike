#ifndef _UI_ATTRIBUTE_METER_H
#define _UI_ATTRIBUTE_METER_H

#include "UIElement.h"

class UIAttributeMeter : public UIElement {
public:
    UIAttributeMeter(const sf::String attName, float x, float y, float width, float height, int& attribute, int& attributeMax, sf::Font font);

    void update();
    void draw(sf::RenderTexture& surface);

    void mouseButtonPressed(const int mx, const int my, const int button);
    void mouseButtonReleased(const int mx, const int my, const int button);
    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    void textEntered(const sf::Uint32 character);

    void showText();
    void hideText();
    void setText(const sf::String text);

    void useDefaultLabel(bool useDefaultLabel);
    void useAttributes(bool useAttributes);
    void fitWidthToText(bool fitWidthToText);

    void setColor(sf::Uint32 color);
    void setBackgroundColor(sf::Uint32 color);

    void setPercentFull(float percentage);
    
    void setWidth(float width);
    void setHeight(float height);

private:
    int& _attribute;
    int& _attributeMax;

    float _percentFull = 1.f;
    
    const sf::String _attName;

    sf::Uint32 _barColor = 0x0000FFFF;
    sf::Uint32 _bgColor = 0x0000DDFF;

    sf::RectangleShape _bar;
    sf::RectangleShape _background;

    bool _showText = true;
    bool _useDefaultLabel = true;
    bool _useAttributes = true;

    bool _fitWidthToText = false;

    sf::RenderTexture _rTexture;
    sf::RectangleShape _center;
    sf::Sprite _leftPad;
    sf::Sprite _rightPad;
};

#endif