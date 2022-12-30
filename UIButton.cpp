#include "UIButton.h"
#include <thread>
#include <iostream>

const sf::Uint32 BUTTON_COLOR[] = { 0xC7, 0x77, 0x1A };
const sf::Uint32 BUTTON_COLOR_BORDER[] = { 0xA3, 0x62, 0x15 };
const sf::Uint32 BUTTON_COLOR_BORDER_HOVER[] = {
    BUTTON_COLOR[0],
    BUTTON_COLOR[1],
    BUTTON_COLOR[2]
};
const sf::Uint32 BUTTON_COLOR_CLICK[] = {
    BUTTON_COLOR_BORDER[0],
    BUTTON_COLOR_BORDER[1],
    BUTTON_COLOR_BORDER[2]
};

UIButton::UIButton(float x, float y, float width, float height, sf::String labelText, sf::Font font,
    UIButtonListener* listener, std::string buttonCode)
    : UIElement(
        x, y, width, height, true, true, font
    ) {

    int padding = 10;
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
    if (!_mouseDown && bounds.contains(_mx, _my)) {
        _sprite.setTexture(*_hoverTexture);
    } else if (bounds.contains(_mx, _my)) {
        _sprite.setTexture(*_clickTexture);
    } else {
        _sprite.setTexture(*_texture);
    }
}

void UIButton::draw(sf::RenderTexture& surface) {}

void UIButton::mouseButtonPressed(const int mx, const int my) {
    _mouseDown = true;
}

void UIButton::mouseButtonReleased(const int mx, const int my) {
    _mouseDown = false;
    sf::FloatRect bounds = _sprite.getGlobalBounds();
    if (bounds.contains(_mx, _my)) {
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

void UIButton::textEntered(const sf::Uint32 character) {}
