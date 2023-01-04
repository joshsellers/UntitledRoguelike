#ifndef _UI_INVENTORY_INTERFACE
#define _UI_INVENTORY_INTERFACE

#include "UIElement.h"
#include "Inventory.h"

// Basically do what YTY does but what YTY does in tick() should be done in mouseButtonReleased
// Also text width from text.getLocalBounds() for displaying item info and having nice little 
// properly sized background
// Also use this UIElement's size for the size of the rectangles that will be taking the place 
// of actual buttons

// Might need to copy relativeFontSize stuff from UIButton
// Probably going to have to calculate relative positions for item "buttons" and won't be able
// to do it based on _sprite's position, maybe make a util function for calculating relative
// position
class UIInventoryInterface : public UIElement {
public:
    UIInventoryInterface(Inventory& source, sf::Font);

private:
    Inventory& _source;
};

#endif