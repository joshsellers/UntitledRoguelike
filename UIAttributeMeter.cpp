#include "UIAttributeMeter.h"

UIAttributeMeter::UIAttributeMeter(const sf::String attName, float x, float y, float width, float height, 
    int& attribute, int& attributeMax, sf::Font font) : 
    _attName(attName), _attribute(attribute), _attributeMax(attributeMax),
    UIElement(x, y, width, height, false, false, font) {

    _disableAutomaticTextAlignment = true;
    _sprite.setPosition(getRelativePos(sf::Vector2f(_x, _y)));

    _bar.setSize(sf::Vector2f(_width * ((float)_attribute / (float)_attributeMax), _height));
    _bar.setFillColor(sf::Color(_barColor));
    _bar.setPosition(_sprite.getPosition().x - _width / 2, _sprite.getPosition().y);

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
}

void UIAttributeMeter::update() {
    _bar.setSize(sf::Vector2f(_width * ((float)_attribute / (float)_attributeMax), _height));

    _text.setString(_attName + ": " + std::to_string(_attribute) + "/" + std::to_string(_attributeMax));
    _text.setPosition(
        _background.getPosition().x + _background.getSize().x / 2 - _text.getGlobalBounds().width / 2,
        _background.getPosition().y + _background.getSize().y / 2 - _text.getGlobalBounds().height / 2
    );
}

void UIAttributeMeter::draw(sf::RenderTexture& surface) {
    surface.draw(_background);
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

void UIAttributeMeter::setColor(sf::Uint32 color) {
    _barColor = color;
    _bar.setFillColor(sf::Color(_barColor));
}

void UIAttributeMeter::setBackgroundColor(sf::Uint32 color) {
    _bgColor = color;
    _background.setFillColor(sf::Color(_bgColor));
}
