#include "UIHandler.h"

UIHandler::UIHandler() {}

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

void UIHandler::mouseButtonPressed(const int mx, const int my) {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->mouseButtonPressed(mx, my);
    }
}

void UIHandler::mouseButtonReleased(const int mx, const int my) {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->mouseButtonReleased(mx, my);
    }
}

void UIHandler::mouseMoved(const int mx, const int my) {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->mouseMoved(mx, my);
    }
}

void UIHandler::textEntered(sf::Uint32 character) {
    for (auto& menu : _menus) {
        if (menu->isActive()) menu->textEntered(character);
    }
}
