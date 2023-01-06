#include "DroppedItem.h"

DroppedItem::DroppedItem(sf::Vector2f pos, unsigned int itemId, sf::IntRect textureRect) : 
    _itemId(itemId), _textureRect(textureRect),
    Entity(pos, 0, textureRect.width, textureRect.height, false) {
}

void DroppedItem::update() {
}

void DroppedItem::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void DroppedItem::move(float xa, float ya) {
}

void DroppedItem::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(_textureRect);
    _sprite.setPosition(getPosition());
}
