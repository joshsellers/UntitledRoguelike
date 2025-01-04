#include "UIAttributeMeter.h"
#include "UIHandler.h"

UIAttributeMeter::UIAttributeMeter(const sf::String attName, float x, float y, float width, float height, 
    int& attribute, int& attributeMax, sf::Font font) : 
    _attName(attName), _attribute(attribute), _attributeMax(attributeMax),
    UIElement(x, y, width, height, false, false, font) {

    _disableAutomaticTextAlignment = true;
    _shape.setPosition(getRelativePos(sf::Vector2f(_x, _y)));

    _bar.setSize(sf::Vector2f(_width * ((float)_attribute / (float)_attributeMax), _height));
    _bar.setFillColor(sf::Color(_barColor));
    _bar.setPosition(_shape.getPosition().x - _width / 2, _shape.getPosition().y);

    float padding = getRelativeWidth(0.5f);
    _background.setPosition(_bar.getPosition().x - padding, _bar.getPosition().y - padding);
    _background.setSize(sf::Vector2f(_width + padding * 2, _height + padding * 2));
    _background.setFillColor(sf::Color(_bgColor));

    float fontSize = 1;
    int relativeFontSize = getRelativeWidth(fontSize);
    _text.setFont(_font);
    _text.setCharacterSize(relativeFontSize);
    _text.setFillColor(sf::Color::White);
    _text.setString(_attName + ": " + std::to_string(_attribute) + "/" + std::to_string(_attributeMax));
    _text.setPosition(
        _background.getPosition().x + _background.getSize().x / 2 - _text.getGlobalBounds().width / 2,
        _background.getPosition().y + _text.getGlobalBounds().width / 2
    );

    _rTexture.create(_width, 14);
    sf::RectangleShape centerShape;
    centerShape.setPosition(0, 0);
    centerShape.setSize(sf::Vector2f(_width, 14));
    centerShape.setTexture(UIHandler::getUISpriteSheet().get());
    centerShape.setTextureRect(sf::IntRect(192, 17, 1, 14));
    _rTexture.draw(centerShape);

    _center.setPosition(_bar.getPosition().x, _background.getPosition().y);
    _center.setSize(sf::Vector2f(_width, _background.getSize().y));
    _center.setTexture(&_rTexture.getTexture());
    _center.setTextureRect(sf::IntRect(0, 0, _width, 14));

    _leftPad.setTexture(*UIHandler::getUISpriteSheet());
    _leftPad.setTextureRect(sf::IntRect(161, 17, 3, 14));
    _leftPad.setScale(getRelativeWidth(0.5f) / 3.f, _center.getSize().y / 14.f);
    _leftPad.setPosition(_bar.getPosition().x + padding - _leftPad.getGlobalBounds().width * 2, _center.getPosition().y);

    _rightPad.setTexture(*UIHandler::getUISpriteSheet());
    _rightPad.setTextureRect(sf::IntRect(176, 17, 3, 14));
    _rightPad.setScale(getRelativeWidth(0.5f) / 3.f, _center.getSize().y / 14.f);
    _rightPad.setPosition(_bar.getPosition().x - padding + _width + _rightPad.getGlobalBounds().width, _center.getPosition().y);
}

void UIAttributeMeter::update() {
    if (_attribute < 0) _attribute = 0;

    if (_fitWidthToText) {
        float textWidth = _text.getGlobalBounds().width;
        float padding = getRelativeWidth(0.5f);
        _width = textWidth + padding;

        _bar.setPosition(_shape.getPosition().x - _width / 2, _shape.getPosition().y);
        _background.setPosition(_bar.getPosition().x - padding, _bar.getPosition().y - padding);
        _background.setSize(sf::Vector2f(_width + padding * 2, _height + padding * 2));
    }

    float widthMultiplier = ((float)_attribute / (float)_attributeMax);
    if (!_useAttributes) widthMultiplier = _percentFull;

    _bar.setSize(sf::Vector2f(_width * widthMultiplier, _height));

    if (_useDefaultLabel) _text.setString(_attName + ": " + std::to_string(_attribute) + "/" + std::to_string(_attributeMax));
    _text.setPosition(
        _background.getPosition().x + _background.getSize().x / 2 - _text.getGlobalBounds().width / 2,
        _background.getPosition().y + _background.getSize().y / 2 - _text.getGlobalBounds().height / 1.5
    );
}

void UIAttributeMeter::draw(sf::RenderTexture& surface) {

    surface.draw(_center);
    surface.draw(_leftPad);
    surface.draw(_rightPad);
    //surface.draw(_background);
    surface.draw(_bar);
    if (_showText) surface.draw(_text);
}

void UIAttributeMeter::mouseButtonPressed(const int mx, const int my, const int button) {
}

void UIAttributeMeter::mouseButtonReleased(const int mx, const int my, const int button) {
}

void UIAttributeMeter::mouseMoved(const int mx, const int my) {
}

void UIAttributeMeter::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
}

void UIAttributeMeter::textEntered(const sf::Uint32 character) {
}

void UIAttributeMeter::showText() {
    _showText = true;
}

void UIAttributeMeter::hideText() {
    _showText = false;
}

void UIAttributeMeter::setText(const sf::String text) {
    _text.setString(text);
}

void UIAttributeMeter::useDefaultLabel(bool useDefaultLabel) {
    _useDefaultLabel = useDefaultLabel;
}

void UIAttributeMeter::useAttributes(bool useAttributes) {
    _useAttributes = useAttributes;
}

void UIAttributeMeter::fitWidthToText(bool fitWidthToText) {
    _fitWidthToText = fitWidthToText;
}

void UIAttributeMeter::setColor(sf::Uint32 color) {
    _barColor = color;
    _bar.setFillColor(sf::Color(_barColor));
}

void UIAttributeMeter::setBackgroundColor(sf::Uint32 color) {
    _bgColor = color;
    _background.setFillColor(sf::Color(_bgColor));
}

void UIAttributeMeter::setPercentFull(float percentage) {
    _percentFull = percentage;
}

void UIAttributeMeter::setWidth(float width) {
    float padding = getRelativeWidth(0.5f);
    _width = WINDOW_WIDTH * (width / 100);
    _background.setSize(sf::Vector2f(_width + padding * 2, _height + padding * 2));
}

void UIAttributeMeter::setHeight(float height) {
    float padding = getRelativeWidth(0.5f);
    _height = WINDOW_WIDTH * (height / 100); 
    _background.setSize(sf::Vector2f(_width + padding * 2, _height + padding * 2));
    _bar.setSize(sf::Vector2f(_width * ((float)_attribute / (float)_attributeMax), _height));
}
