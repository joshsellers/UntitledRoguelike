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

void UIMenu::keyPressed(sf::Keyboard::Key& key) {
    for (auto& element : _elements) {
        if (element->isActive()) element->keyPressed(key);
    }
}

void UIMenu::keyReleased(sf::Keyboard::Key& key) {
    for (auto& element : _elements) {
        if (element->isActive()) element->keyReleased(key);
    }
}

void UIMenu::mouseButtonPressed(const int mx, const int my, const int button) {
    for (auto& element : _elements) {
        if (element->isActive()) element->mouseButtonPressed(mx, my, button);
    }
}

void UIMenu::mouseButtonReleased(const int mx, const int my, const int button) {
    for (auto& element : _elements) {
        if (element->isActive()) element->mouseButtonReleased(mx, my, button);
    }
}

void UIMenu::mouseMoved(const int mx, const int my) {
    for (auto& element : _elements) {
        if (element->isActive()) element->mouseMoved(mx, my);
    }
}

void UIMenu::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
    for (auto& element : _elements) {
        if (element->isActive()) element->mouseWheelScrolled(mouseWheelScroll);
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

bool UIMenu::isActive() const {
    return _isActive;
}
