#include "UIInventoryInterface.h"

UIInventoryInterface::UIInventoryInterface(Inventory& source, sf::Font font) : 
    _source(source), UIElement(0, 0, 20, 20, false, false, font) {
}
