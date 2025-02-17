#include "Blinker.h"
#include "../World.h"

Blinker::Blinker(sf::Vector2f pos) : Bouncer(BLINKER, pos, 2, TILE_SIZE, TILE_SIZE, randomInt(0, 359)) {
    _isEnemy = true;
    _entityType = "blinker";

    _hitBox.width = TILE_SIZE;
    _hitBox.height = TILE_SIZE;

    setMaxHitPoints(20);
    heal(getMaxHitPoints());

    if (randomChance(0.5f)) {
        getInventory().addItem(Item::PENNY.getId(), randomInt(1, 5));
    }
}

void Blinker::preupdate() {
    if (getHitBox().intersects(getWorld()->getPlayer()->getHitBox())) {
        getWorld()->getPlayer()->takeDamage(10);
    }

    if (randomChance(0.04f)) _isBlinking = true;
}

void Blinker::draw(sf::RenderTexture& surface) {
    if (_isBlinking) {
        constexpr int ticksPerFrame = 6;
        constexpr int frameCount = 7;
        const int frame = ((_numSteps / ticksPerFrame) % frameCount);
        _sprite.setTextureRect(sf::IntRect(
            (32 + frame) << SPRITE_SHEET_SHIFT, 5 << SPRITE_SHEET_SHIFT, TILE_SIZE, TILE_SIZE
        ));

        if (frame == frameCount - 1) {
            _sprite.setTextureRect(sf::IntRect(
                32 << SPRITE_SHEET_SHIFT, 5 << SPRITE_SHEET_SHIFT, TILE_SIZE, TILE_SIZE
            ));
            _isBlinking = false;
        }
    }

    surface.draw(_sprite);
}

void Blinker::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(
        32 << SPRITE_SHEET_SHIFT, 5 << SPRITE_SHEET_SHIFT, TILE_SIZE, TILE_SIZE
    ));
    _sprite.setOrigin((float)TILE_SIZE / 2.f, 0);
    _sprite.setPosition(_pos);
}

void Blinker::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}