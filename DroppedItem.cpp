#include "DroppedItem.h"
#include "World.h"

DroppedItem::DroppedItem(sf::Vector2f pos, float originOffset, unsigned int itemId, unsigned int amount, sf::IntRect textureRect) :
    _itemId(itemId), _amount(amount), _textureRect(textureRect), _minY(pos.y - _hoverDist), _originalY(pos.y),
    Entity(pos, 0, textureRect.width, textureRect.height, false) {

    _pos.y = _originalY - originOffset;
    _down = _pos.y >= _minY || _pos.y > _originalY;
}

void DroppedItem::update() {
    if (isActive()) {
        for (auto& entity : getWorld()->getEntities()) {
            if (entity->canPickUpItems() && entity->isActive() && 
                entity->getSprite().getGlobalBounds().intersects(getSprite().getGlobalBounds())) {
                entity->getInventory().addItem(_itemId, _amount);
                _isActive = false;
            }
        }
    }

    _animCounter++;
    if ((_animCounter & 1) == 0) 
        if (!_down) 
            if (_pos.y > _minY) _pos.y--;
            else _down = true;
        else
            if (_pos.y < _originalY) _pos.y++;
            else _down = false;

    _sprite.setPosition(getPosition());
}

void DroppedItem::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void DroppedItem::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(_textureRect);
    _sprite.setPosition(getPosition());
}
