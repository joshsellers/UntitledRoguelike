#include "UIButton.h"
#include <thread>
#include <iostream>

UIButton::UIButton(float x, float y, float width, float height, sf::String labelText, sf::Font font,
    UIButtonListener* listener, std::string buttonCode)
    : UIElement(
        x, y, width, height, true, true, font
    ) {

    int padding = getRelativeWidth(0.5f);
    int w = (int)_width;
    int h = (int)_height;
    sf::Uint8* mainPixels = new sf::Uint8[w * h * 4];
    for (int py = 0; py < h; py++) {
        for (int px = 0; px < w; px++) {
            if (px > padding / 2 && py > padding
                && px < w - padding && py < h - padding) {
                mainPixels[px * 4 + py * w * 4 + 0] = BUTTON_COLOR[0];
                mainPixels[px * 4 + py * w * 4 + 1] = BUTTON_COLOR[1];
                mainPixels[px * 4 + py * w * 4 + 2] = BUTTON_COLOR[2];
                mainPixels[px * 4 + py * w * 4 + 3] = 0xFF;
            } else {
                mainPixels[px * 4 + py * w * 4 + 0] = BUTTON_COLOR_BORDER[0];
                mainPixels[px * 4 + py * w * 4 + 1] = BUTTON_COLOR_BORDER[1];
                mainPixels[px * 4 + py * w * 4 + 2] = BUTTON_COLOR_BORDER[2];
                mainPixels[px * 4 + py * w * 4 + 3] = 0xFF;
            }
        }
    }

    sf::Uint8* hoverPixels = new sf::Uint8[w * h * 4];
    for (int py = 0; py < h; py++) {
        for (int px = 0; px < w; px++) {
            if (px > padding && py > padding
                && px < w - padding && py < h - padding) {
                hoverPixels[px * 4 + py * w * 4 + 0] = BUTTON_COLOR[0];
                hoverPixels[px * 4 + py * w * 4 + 1] = BUTTON_COLOR[1];
                hoverPixels[px * 4 + py * w * 4 + 2] = BUTTON_COLOR[2];
                hoverPixels[px * 4 + py * w * 4 + 3] = 0xFF;
            } else {
                hoverPixels[px * 4 + py * w * 4 + 0] =
                    BUTTON_COLOR_BORDER_HOVER[0];
                hoverPixels[px * 4 + py * w * 4 + 1] =
                    BUTTON_COLOR_BORDER_HOVER[1];
                hoverPixels[px * 4 + py * w * 4 + 2] =
                    BUTTON_COLOR_BORDER_HOVER[2];
                hoverPixels[px * 4 + py * w * 4 + 3] = 0xFF;
            }
        }
    }

    sf::Uint8* clickPixels = new sf::Uint8[w * h * 4];
    for (int py = 0; py < h; py++) {
        for (int px = 0; px < w; px++) {
            clickPixels[px * 4 + py * w * 4 + 0] = BUTTON_COLOR_CLICK[0];
            clickPixels[px * 4 + py * w * 4 + 1] = BUTTON_COLOR_CLICK[1];
            clickPixels[px * 4 + py * w * 4 + 2] = BUTTON_COLOR_CLICK[2];
            clickPixels[px * 4 + py * w * 4 + 3] = 0xFF;
        }
    }


    _texture->create(_width, _height);
    _texture->update(mainPixels);

    _hoverTexture->create(_width, _height);
    _hoverTexture->update(hoverPixels);

    _clickTexture->create(_width, _height);
    _clickTexture->update(clickPixels);

    _sprite.setTexture(*_texture);


    float fontSize = 1.5;
    int relativeFontSize = (float)WINDOW_WIDTH * (fontSize / 100);
    _text.setFont(_font);
    _text.setCharacterSize(relativeFontSize);
    _text.setFillColor(sf::Color::White);
    _text.setString(labelText);

    delete[] mainPixels;
    delete[] hoverPixels;
    delete[] clickPixels;

    _buttonCode = buttonCode;
    _listener = listener;
}

void UIButton::update() {
    sf::FloatRect bounds = _sprite.getGlobalBounds();
    if (!_mouseDown && (bounds.contains(_mx, _my) || _isSelected)) {
        _sprite.setTexture(*_hoverTexture);
    } else if (bounds.contains(_mx, _my) || (_mouseDown && _isSelected)) {
        _sprite.setTexture(*_clickTexture);
    } else {
        _sprite.setTexture(*_texture);
    }

    if (pressWhenSelected && _isSelected && !_wasJustSelected) {
        _listener->buttonPressed(_buttonCode);
    }
    _wasJustSelected = _isSelected;
}

void UIButton::draw(sf::RenderTexture& surface) {}

void UIButton::setLabelText(std::string labelText) {
    _text.setString(labelText);
}

void UIButton::controllerButtonPressed(GAMEPAD_BUTTON button) {
    if (_isSelected && button == GAMEPAD_BUTTON::A && !pressWhenSelected) _mouseDown = true;
}

void UIButton::controllerButtonReleased(GAMEPAD_BUTTON button) {
    if (_isSelected && button == GAMEPAD_BUTTON::A && !pressWhenSelected) {
        _mouseDown = false;
        _listener->buttonPressed(_buttonCode);
    }
}

void UIButton::mouseButtonPressed(const int mx, const int my, const int button) {
    _mouseDown = true;
}

void UIButton::mouseButtonReleased(const int mx, const int my, const int button) {
    _mouseDown = false;
    sf::FloatRect bounds = _sprite.getGlobalBounds();
    if (bounds.contains(mx, my)) {
        /*std::thread callbackThread(
            &UIButtonListener::buttonPressed, _listener, _buttonCode
        );
        callbackThread.detach();*/
        _listener->buttonPressed(_buttonCode);
    }
}

void UIButton::mouseMoved(const int mx, const int my) {
    _mx = mx;
    _my = my;
}

void UIButton::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
}

void UIButton::textEntered(const sf::Uint32 character) {}

std::shared_ptr<sf::Texture> UIButton::getHoverTexture() {
    return _hoverTexture;
}

std::shared_ptr<sf::Texture> UIButton::getClickTexture() {
    return _clickTexture;
}

sf::Vector2i UIButton::getMousePos() {
    return sf::Vector2i(_mx, _my);
}
