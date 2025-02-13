#include "DroppedItem.h"
#include "../World.h"
#include "../../core/SoundManager.h"
#include "../../core/Tutorial.h"
#include "../../statistics/StatManager.h"
#include "../../inventory/abilities/AbilityManager.h"
#include "../../inventory/abilities/Ability.h"

DroppedItem::DroppedItem(sf::Vector2f pos, float originOffset, unsigned int itemId, unsigned int amount, sf::IntRect textureRect, bool droppedByPlayer) :
    _itemId(itemId), _amount(amount), _textureRect(textureRect), _minY(pos.y - _hoverDist), _originalY(pos.y),
    Entity(DROPPED_ITEM, pos, 1, textureRect.width, textureRect.height, false) {

    _pos.y = _originalY - originOffset;
    _down = _pos.y >= _minY || _pos.y > _originalY;

    _usesDormancyRules = true;
    _maxTimeOutOfChunk = 60;
    _dormancyTimeout = 60 * 5 * 60;

    _entityType = "droppeditem";

    if (!HARD_MODE_ENABLED && !droppedByPlayer && _itemId == Item::PENNY.getId()) {
        _amount = _amount + ((float)_amount * 0.75f);
    }

    if (!droppedByPlayer && _itemId == Item::PENNY.getId() && AbilityManager::playerHasAbility(Ability::CASSIDYS_HEAD.getId())) {
        const float chance = AbilityManager::getParameter(Ability::CASSIDYS_HEAD.getId(), "chance");
        if (chance >= 1.f || randomChance(chance)) _amount *= 2;
    }
}

void DroppedItem::update() {
    if (isActive()) {
        if (_world->getPlayer()->getSprite().getGlobalBounds().intersects(getSprite().getGlobalBounds())) {
            _world->getPlayer()->getInventory().addItem(_itemId, _amount);
            if (_itemId == Item::PENNY.getId()) {
                SoundManager::playSound("coinpickup");
                StatManager::increaseStat(PENNIES_COLLECTED, _amount);
            }
            _isActive = false;

            if (!Tutorial::isCompleted() && _itemId == Item::SLIME_BALL.getId()) Tutorial::completeStep(TUTORIAL_STEP::PICK_UP_SLIMEBALL);
            return;
        }

        if (_itemId == Item::PENNY.getId()) moveTowardPlayer();

        /*for (auto& entity : getWorld()->getCollectorMobs()) {
            if (entity->canPickUpItems() && entity->isActive() && 
                entity->getSprite().getGlobalBounds().intersects(getSprite().getGlobalBounds())) {
                entity->getInventory().addItem(_itemId, _amount);
                _isActive = false;
                return;
            }
        }*/
    }

    _animCounter++;
    if (_itemId != Item::PENNY.getId()) {
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
    if (_itemId == Item::PENNY.getId()) {
        const sf::Vector2i textureCoords(128, 192);
        constexpr int ticksPerFrame = 4;
        constexpr int frameCount = 13;
        int xOffset = ((_animCounter / ticksPerFrame) % frameCount) * TILE_SIZE;

        _sprite.setTextureRect(sf::IntRect(
            textureCoords.x + xOffset,
            textureCoords.y, 
            TILE_SIZE,
            TILE_SIZE
        ));
    }
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

unsigned int DroppedItem::getItemId() const {
    return _itemId;
}

unsigned int DroppedItem::getAmount() const {
    return _amount;
}

void DroppedItem::moveTowardPlayer() {
    if (getWorld()->getPlayer()->getCoinMagnetCount() > 0) {
        const sf::Vector2f playerPos(
            (int)getWorld()->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, 
            (int)getWorld()->getPlayer()->getPosition().y + PLAYER_WIDTH
        );

        const sf::Vector2f currentPos(getPosition().x + TILE_SIZE / 2, getPosition().y + TILE_SIZE / 2);

        const float dx = playerPos.x - currentPos.x;
        const float dy = playerPos.y - currentPos.y;
        const float distSquared = (dx * dx) + (dy * dy);

        constexpr float minDistanceSquared = 250.f * 250.f;

        if (distSquared <= minDistanceSquared) {
            const float attractionSpeed = 1000.f * (1 / (distSquared / getWorld()->getPlayer()->getCoinMagnetCount()));
            const float angle = std::atan2(dy, dx);
            const float xa = attractionSpeed * std::cos(angle);
            const float ya = attractionSpeed * std::sin(angle);
            
            move(xa, ya);
        }
    }
}
