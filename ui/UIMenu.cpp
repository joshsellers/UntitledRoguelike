#include "UIMenu.h"
#include <iostream>

UIMenu::UIMenu() {}

void UIMenu::update() {
    for (auto& element : _elements) {
        if (element->isActive()) element->update();
    }

    if (_pendingActivation) {
        for (auto& element : _elements) {
            element->show();

            if (!USING_MOUSE && useGamepadConfiguration) {
                if (element->isActive()
                    && element->_selectionId == _selectionGrid.at(_selectedItemY).at(_selectedItemX)) {
                    element->_isSelected = true;
                } else element->_isSelected = false;
            }
        }
        _pendingActivation = false;
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

std::vector<std::shared_ptr<UIElement>> UIMenu::getElements() const {
    return _elements;
}

void UIMenu::clearElements() {
    _elements.clear();
}

void UIMenu::controllerButtonReleased(GAMEPAD_BUTTON button) {
    for (auto& element : _elements) {
        if (element->isActive() && !element->blockControllerInput) element->controllerButtonReleased(button);
    }
}

void UIMenu::controllerButtonPressed(GAMEPAD_BUTTON button) {
    if (useGamepadConfiguration && (int)button >= (int)GAMEPAD_BUTTON::DPAD_UP) {
        if (_selectedItemX == -1) _selectedItemX = 0;
        if (_selectedItemY == -1) _selectedItemY = 0;

        switch (button) {
            case GAMEPAD_BUTTON::DPAD_UP:
                if (_selectedItemY > 0) {
                    _selectedItemY--;
                    if (_selectedItemX > (int)_selectionGrid.at(_selectedItemY).size() - 1) _selectedItemX = 0;
                }
                break;
            case GAMEPAD_BUTTON::DPAD_DOWN:
                if (_selectedItemY < (int)_selectionGrid.size() - 1) {
                    _selectedItemY++;
                }
                if (_selectedItemX > (int)_selectionGrid.at(_selectedItemY).size() - 1) _selectedItemX = 0;
                break;
            case GAMEPAD_BUTTON::DPAD_LEFT:
                if (_selectedItemX > 0) _selectedItemX--;
                break;
            case GAMEPAD_BUTTON::DPAD_RIGHT:
                if (_selectedItemX < (int)_selectionGrid.at(_selectedItemY).size() - 1) {
                    _selectedItemX++;
                }
                break;
        }
        for (auto& element : _elements) {
            if (element->isActive()
                && element->_selectionId == _selectionGrid.at(_selectedItemY).at(_selectedItemX)) {
                element->_isSelected = true;
            } else element->_isSelected = false;
        }
    }

    for (auto& element : _elements) {
        if (element->isActive() && !element->blockControllerInput) element->controllerButtonPressed(button);
    }
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
    _selectedItemX = -1;
    _selectedItemY = -1;

    for (auto& element : _elements) {
        if (element->isActive()) {
            element->mouseMoved(mx, my);
            if (!element->_disableMouseMovementDeselection) element->_isSelected = false;
        }
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
    //for (auto& element : _elements) {
    //    element->show();
    //}
    _pendingActivation = true;

    if (!USING_MOUSE && useGamepadConfiguration) {
        if (_selectedItemX == -1) _selectedItemX = 0;
        if (_selectedItemY == -1) _selectedItemY = 0;
    }
}

void UIMenu::hide() {
    _isActive = false;
    for (auto& element : _elements) {
        element->hide();
        element->_isSelected = false;
    }
    _selectedItemX = -1;
    _selectedItemY = -1;
}

bool UIMenu::isActive() const {
    return _isActive;
}

void UIMenu::defineSelectionGrid(std::vector<std::vector<int>> grid) {
    _selectionGrid = grid;
}
