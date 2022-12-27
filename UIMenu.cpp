#include "UIMenu.h"

UIMenu::UIMenu() {}

void UIMenu::update() {
    for (auto& element : _elements) {
        if (element->isActive()) element->update();
    }
}

void UIMenu::draw(sf::RenderTexture& surface) {
    for (auto& element : _elements) {
        if (element->isActive()) element->render(surface);
    }
}

void UIMenu::addElement(std::shared_ptr<UIElement> element) {
    _elements.push_back(element);
}

void UIMenu::mouseButtonPressed(const int mx, const int my) {
    for (auto& element : _elements) {
        if (element->isActive()) element->mouseButtonPressed(mx, my);
    }
}

void UIMenu::mouseButtonReleased(const int mx, const int my) {
    for (auto& element : _elements) {
        if (element->isActive()) element->mouseButtonReleased(mx, my);
    }
}

void UIMenu::mouseMoved(const int mx, const int my) {
    for (auto& element : _elements) {
        if (element->isActive()) element->mouseMoved(mx, my);
    }
}

void UIMenu::textEntered(sf::Uint32 character) {
    for (auto& element : _elements) {
        if (element->isActive()) element->textEntered(character);
    }
}

void UIMenu::show() {
    _isActive = true;
    for (auto& element : _elements) {
        element->show();
    }
}

void UIMenu::hide() {
    _isActive = false;
    for (auto& element : _elements) {
        element->hide();
    }
}

bool UIMenu::isActive() {
    return _isActive;
}
