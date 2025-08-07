#include "UIControlsDisplay.h"
#include "UIHandler.h"
#include "../world/entities/Entity.h"
#include "../core/gamepad/GamePad.h"

UIControlsDisplay::UIControlsDisplay(sf::Vector2f pos, sf::Font font) 
    : UIElement(pos.x, pos.y, 0, 0, false, false, font) {

    _shape.setTexture(UIHandler::getUISpriteSheet().get());

    _text.setFont(_font);
    setFontSize(_fontSize);
    setFontColor(_fontColor);
    setIconSize(_iconSize);
}

void UIControlsDisplay::update() {
}

void UIControlsDisplay::draw(sf::RenderTexture& surface) {
    if (!GamePad::isConnected()) return;

    const sf::Vector2f basePos = getRelativePos({_x, _y});
    const float padding = getRelativeHeight(0.25f);
    const float textSpacing = getRelativeWidth(0.5f) + _iconSize;

    for (int i = 0; i < _labels.size(); i++) {
        const auto& label = _labels.at(i);
        sf::Vector2i iconPos = _buttonIcons.at(label->button);
        if (GamePad::getVendorId() == SONY_VID) iconPos.y += 5;
        if (label->press) iconPos.y += 2;

        _shape.setTextureRect(sf::IntRect{
            iconPos.x << SPRITE_SHEET_SHIFT,
            iconPos.y << SPRITE_SHEET_SHIFT,
            TILE_SIZE,
            TILE_SIZE
        });

        _shape.setPosition(basePos.x, basePos.y + padding * i + _iconSize * i);

        _text.setString(label->text);
        _text.setPosition(basePos.x + textSpacing, basePos.y + padding * i + _iconSize * i);

        surface.draw(_shape);
        surface.draw(_text);
    }
}

void UIControlsDisplay::addLabel(GAMEPAD_BUTTON button, std::string text, bool press) {
    _labels.push_back(std::shared_ptr<ControlLabel>(new ControlLabel(button, text, press)));
}

void UIControlsDisplay::setFontSize(float fontSize) {
    _fontSize = getRelativeWidth(fontSize);
    _text.setCharacterSize(_fontSize);
    _text.setOutlineThickness(2.f);
}

void UIControlsDisplay::setFontColor(sf::Uint32 fontColor) {
    _fontColor = fontColor;
    _text.setColor(sf::Color(_fontColor));
}

void UIControlsDisplay::setIconSize(float iconSize) {
    _iconSize = getRelativeWidth(iconSize);
    _shape.setSize({ _iconSize, _iconSize });
}

sf::IntRect UIControlsDisplay::getButtonIcon(GAMEPAD_BUTTON button, bool stickPressed) {
    sf::Vector2i iconPos = _buttonIcons.at(button);
    if (GamePad::getVendorId() == SONY_VID) iconPos.y += 5;
    if (stickPressed) iconPos.y += 2;
    iconPos.x <<= SPRITE_SHEET_SHIFT;
    iconPos.y <<= SPRITE_SHEET_SHIFT;
    return { iconPos.x, iconPos.y, TILE_SIZE, TILE_SIZE };
}

void UIControlsDisplay::controllerButtonPressed(GAMEPAD_BUTTON button) {
}

void UIControlsDisplay::controllerButtonReleased(GAMEPAD_BUTTON button) {
}

void UIControlsDisplay::mouseButtonPressed(const int mx, const int my, const int button) {
}

void UIControlsDisplay::mouseButtonReleased(const int mx, const int my, const int button) {
}

void UIControlsDisplay::mouseMoved(const int mx, const int my) {
}

void UIControlsDisplay::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
}

void UIControlsDisplay::textEntered(const sf::Uint32 character) {
}
