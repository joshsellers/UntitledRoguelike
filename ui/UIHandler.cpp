#include "UIHandler.h"
#include "../core/MessageManager.h"

UIHandler::UIHandler() {
    _spriteSheet->create(256, 256);
    if (!_spriteSheet->loadFromFile("res/ui_sprite_sheet.png")) {
        MessageManager::displayMessage("Failed to load UI sprite sheet!", 10, WARN);
    }
}

void UIHandler::update() {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->update();
    }
}

void UIHandler::draw(sf::RenderTexture& surface) {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->draw(surface);
    }
}

void UIHandler::addMenu(std::shared_ptr<UIMenu> menu) {
    _menus.push_back(menu);
}

void UIHandler::controllerButtonReleased(GAMEPAD_BUTTON button) {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->controllerButtonReleased(button);
    }
}

void UIHandler::controllerButtonPressed(GAMEPAD_BUTTON button) {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->controllerButtonPressed(button);
    }
}

void UIHandler::gamepadConnected() {
}

void UIHandler::gamepadDisconnected() {}

void UIHandler::keyPressed(sf::Keyboard::Key& key) {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->keyPressed(key);
    }
}

void UIHandler::keyReleased(sf::Keyboard::Key& key) {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->keyReleased(key);
    }
}

void UIHandler::mouseButtonPressed(const int mx, const int my, const int button) {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->mouseButtonPressed(mx, my, button);
    }
}

void UIHandler::mouseButtonReleased(const int mx, const int my, const int button) {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->mouseButtonReleased(mx, my, button);
    }
}

void UIHandler::mouseMoved(const int mx, const int my) {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->mouseMoved(mx, my);
    }
}

void UIHandler::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->mouseWheelScrolled(mouseWheelScroll);
    }
}

void UIHandler::textEntered(sf::Uint32 character) {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->textEntered(character);
    }
}

std::shared_ptr<sf::Texture> UIHandler::getUISpriteSheet() {
    return _spriteSheet;
}
