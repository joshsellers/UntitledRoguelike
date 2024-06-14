#include "DroppedItem.h"
#include "../World.h"
#include "../../core/SoundManager.h"
#include "../../core/Tutorial.h"

DroppedItem::DroppedItem(sf::Vector2f pos, float originOffset, unsigned int itemId, unsigned int amount, sf::IntRect textureRect) :
    _itemId(itemId), _amount(amount), _textureRect(textureRect), _minY(pos.y - _hoverDist), _originalY(pos.y),
    Entity(DROPPED_ITEM, pos, 1, textureRect.width, textureRect.height, false) {

    _pos.y = _originalY - originOffset;
    _down = _pos.y >= _minY || _pos.y > _originalY;

    _usesDormancyRules = true;
    _maxTimeOutOfChunk = 60;
    _dormancyTimeout = 60 * 5 * 60;

    _entityType = "droppeditem";
}

void DroppedItem::update() {
    if (isActive()) {
        if (_world->getPlayer()->getHitBox().intersects(getSprite().getGlobalBounds())) {
            _world->getPlayer()->getInventory().addItem(_itemId, _amount);
            if (_itemId == Item::PENNY.getId()) SoundManager::playSound("coinpickup");
            _isActive = false;

            if (!Tutorial::isCompleted() && _itemId == Item::SLIME_BALL.getId()) Tutorial::completeStep(TUTORIAL_STEP::PICK_UP_SLIMEBALL);
            else if (!Tutorial::isCompleted() && _itemId == Item::WOOD.getId()) Tutorial::completeStep(TUTORIAL_STEP::CUT_DOWN_TREE);
            return;
        }

        if (_itemId == Item::PENNY.getId()) moveTowardPlayer();

        for (auto& entity : getWorld()->getCollectorMobs()) {
            if (entity->canPickUpItems() && entity->isActive() && 
                entity->getSprite().getGlobalBounds().intersects(getSprite().getGlobalBounds())) {
                entity->getInventory().addItem(_itemId, _amount);
                _isActive = false;
                return;
            }
        }
    }

    if (_itemId != Item::PENNY.getId()) {
        _animCounter++;
        if ((_animCounter & 1) == 0)
            if (!_down)
                if (_pos.y > _minY) _pos.y--;
                else _down = true;
            else
                if (_pos.y < _originalY) _pos.y++;
                else _down = false;
    }

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

std::string DroppedItem::getSaveData() const {
    return std::to_string(_itemId) + ":" + std::to_string(_amount);
}

void DroppedItem::moveTowardPlayer() {
    if (getWorld()->getPlayer()->getCoinMagnetCount() > 0) {
        sf::Vector2f playerPos(
            (int)getWorld()->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, 
            (int)getWorld()->getPlayer()->getPosition().y + PLAYER_WIDTH
        );

        sf::Vector2f currentPos(getPosition().x + TILE_SIZE / 2, getPosition().y + TILE_SIZE / 2);

        float dx = playerPos.x - currentPos.x;
        float dy = playerPos.y - currentPos.y;
        float distSquared = (dx * dx) + (dy * dy);

        const float minDistanceSquared = 250.f * 250.f;

        if (distSquared <= minDistanceSquared) {
            const float attractionSpeed = 1000.f * (1 / (distSquared / getWorld()->getPlayer()->getCoinMagnetCount()));
            float xa = 0, ya = 0;
            if (playerPos.y < currentPos.y) ya = -attractionSpeed;
            else if (playerPos.y > currentPos.y) ya = attractionSpeed;
            if (playerPos.x < currentPos.x) xa = -attractionSpeed;
            else if (playerPos.x > currentPos.x) xa = attractionSpeed;
            
            move(xa, ya);
        }
    }
}
