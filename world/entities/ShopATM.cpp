#include "ShopATM.h"
#include "../World.h"
#include "DroppedItem.h"
#include "../../core/InputBindings.h"
#include "../../core/SoundManager.h"
#include "../../statistics/StatManager.h"

constexpr int DIGITS_X = 928;
constexpr int DIGITS_Y = 128;
constexpr int DIGITS_WIDTH = 4;
constexpr int DIGITS_HEIGHT = 5;

ShopATM::ShopATM(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 32, 48, false) {
    _lastATMValue = StatManager::getOverallStat(ATM_AMOUNT);
    loadSprite(spriteSheet);

    _entityType = "shopatm";

   /* _hasColliders = true;
    sf::FloatRect collider;
    collider.left = _pos.x;
    collider.top = _pos.y + 20;
    collider.width = 96;
    collider.height = 10;
    _colliders.push_back(collider);*/

    _hitBox.left = _pos.x - 8;
    _hitBox.top = _pos.y;
    _hitBox.width = 48;
    _hitBox.height = 96;

    setMaxHitPoints(1000000000);
    heal(getMaxHitPoints());

    setDigits();
}

void ShopATM::update() {
    if (!getWorld()->playerIsInShop()) {
        deactivate();
        return;
    }

    for (const auto& entity : getWorld()->getEntities()) {
        if (entity->getSaveId() == DROPPED_ITEM && entity->isActive() && entity->getSprite().getGlobalBounds().intersects(getHitBox())) {
            DroppedItem* droppedItem = dynamic_cast<DroppedItem*>(entity.get());
            if (droppedItem->getItemId() == Item::PENNY.getId()) {
                const unsigned int droppedAmount = droppedItem->getAmount();
                const unsigned int maxValue = std::pow(10, _numDigits) - 1;
                if ((unsigned int)StatManager::getOverallStat(ATM_AMOUNT) + droppedAmount > maxValue) {
                    const unsigned int addAmount = maxValue - StatManager::getOverallStat(ATM_AMOUNT);
                    const unsigned int returnAmount = droppedAmount - addAmount;

                    StatManager::increaseStat(ATM_AMOUNT, addAmount);
                    getWorld()->getPlayer()->getInventory().addItem(Item::PENNY.getId(), returnAmount);

                    if (addAmount > 0) SoundManager::playSound("atmdeposit");
                } else {
                    StatManager::increaseStat(ATM_AMOUNT, droppedAmount);
                    SoundManager::playSound("atmdeposit");
                }

                droppedItem->deactivate();
            }
        }
    }

    if ((unsigned int)StatManager::getOverallStat(ATM_AMOUNT) != _lastATMValue) {
        setDigits();
        _lastATMValue = StatManager::getOverallStat(ATM_AMOUNT);
    }

    constexpr long long tipCooldown = 30000LL;
    if (currentTimeMillis() - _lastTouchTime >= tipCooldown && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())) {
        const std::string withdrawButtonName =
            GamePad::isConnected() ?
            InputBindingManager::getGamepadButtonName(InputBindingManager::getGamepadBinding(InputBindingManager::BINDABLE_ACTION::INTERACT))
            : InputBindingManager::getKeyName(InputBindingManager::getKeyboardBinding(InputBindingManager::BINDABLE_ACTION::INTERACT));

        MessageManager::displayMessage("Drop coins to deposit.\nPress/hold " + withdrawButtonName + " to withdraw one penny.", 10, NORMAL, "NONE");
        _lastTouchTime = currentTimeMillis();
    }
}

void ShopATM::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
    
    for (const auto& sprite : _digitSprites) {
        surface.draw(sprite);
    }
}

void ShopATM::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(58 * TILE_SIZE, 9 * TILE_SIZE, 32, 48)
    );
    _sprite.setPosition(getPosition());

    for (int i = 0; i < _numDigits; i++) {
        _digitSprites[i].setTexture(*spriteSheet);
        _digitSprites[i].setPosition(getPosition().x + 9 + i * 5, getPosition().y + 26);
    }
}

void ShopATM::setDigits() {
    unsigned int digits[_numDigits]{};
    unsigned int atm = StatManager::getOverallStat(ATM_AMOUNT);

    int index = _numDigits - 1;
    while (atm && index >= 0) {
        digits[index] = atm % 10;
        atm /= 10;
        index--;
    }

    for (int i = 0; i < _numDigits; i++) {
        _digitSprites[i].setTextureRect(sf::IntRect(
            DIGITS_X + digits[i] * 4, DIGITS_Y, DIGITS_WIDTH, DIGITS_HEIGHT
        ));
    }
}
