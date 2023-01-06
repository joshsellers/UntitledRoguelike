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
