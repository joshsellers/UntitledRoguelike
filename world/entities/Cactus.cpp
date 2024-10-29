#include "Cactus.h"
#include "../../core/Util.h"
#include "../World.h"

Cactus::Cactus(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 2, 2, true) {
    loadSprite(spriteSheet);
    setMaxHitPoints(15);
    heal(getMaxHitPoints());

    _hitBoxXOffset = 0;
    _hitBoxYOffset = 0;
    _hitBox.width = TILE_SIZE * 2;
    _hitBox.height = TILE_SIZE * 2;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
    _displayBottom = true;

    srand(currentTimeNano());
    if (randomInt(0, 10) == 0) {
        const unsigned int fleshAmount = 1;
        getInventory().addItem(Item::CACTUS_FLESH.getId(), fleshAmount);
    }
}

void Cactus::update() {
}

void Cactus::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void Cactus::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(CACTUS_SPRITE_POS_X + (randomInt(0, 1) * TILE_SIZE * 2), CACTUS_SPRITE_POS_Y, TILE_SIZE * 2, TILE_SIZE * 2)
    );
    _sprite.setPosition(getPosition());
}

void Cactus::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        getWorld()->propDestroyedAt(sf::Vector2f(_pos.x + (_spriteWidth * TILE_SIZE) / 2, _pos.y + (_spriteHeight * TILE_SIZE)));
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}
