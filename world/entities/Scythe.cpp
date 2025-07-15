#include "Scythe.h"
#include "../World.h"
#include "../../core/Viewport.h"

Scythe::Scythe(sf::Vector2f pos) : Entity(SCYTHE, pos, 4.f, 3 * TILE_SIZE, 3 * TILE_SIZE, false) {
    setMaxHitPoints(1000000000);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(3.f * (float)TILE_SIZE / 2.f) + 10;
    _hitBoxYOffset = -(3.f * (float)TILE_SIZE / 2.f) + 10;
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
    _hitBox.width = 28;
    _hitBox.height = 28;

    const float angle = degToRads((float)randomInt(0, 360));
    _velocity.x = _baseSpeed * std::cos(angle);
    _velocity.y = _baseSpeed * std::sin(angle);
}

void Scythe::update() {
    const sf::Vector2f playerPos(getWorld()->getPlayer()->getPosition().x + (float)TILE_SIZE / 2.f, getWorld()->getPlayer()->getPosition().y + TILE_SIZE);

    const sf::FloatRect viewport = Viewport::getBounds();
    constexpr float vpExtension = 100.f;
    const sf::FloatRect bounceBounds(viewport.left - vpExtension, viewport.top - vpExtension, viewport.width + vpExtension * 2, viewport.height + vpExtension * 2);

    const sf::Vector2i spriteSize(_hitBox.width, _hitBox.height);

    _pos.x += _velocity.x;
    _pos.y += _velocity.y;

    if (_pos.x - (float)spriteSize.x / 2.f <= bounceBounds.left) {
        _pos.x = bounceBounds.left + (float)spriteSize.x / 2.f;
        _velocity.x = -_velocity.x;
    } else if (_pos.x + (float)spriteSize.x / 2.f >= bounceBounds.left + bounceBounds.width) {
        _pos.x = bounceBounds.left + bounceBounds.width - (float)spriteSize.x / 2.f;
        _velocity.x = -_velocity.x;
    }

    if (_pos.y - (float)spriteSize.y / 2 <= bounceBounds.top) {
        _pos.y = bounceBounds.top + (float)spriteSize.y / 2;
        _velocity.y = -_velocity.y;
    } else if (_pos.y + (float)spriteSize.y / 2 >= bounceBounds.top + bounceBounds.height) {
        _pos.y = bounceBounds.top + bounceBounds.height - (float)spriteSize.y / 2;
        _velocity.y = -_velocity.y;
    }


    _sprite.rotate(5);

    _sprite.setPosition(getPosition());
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    constexpr long long damageRateMillis = 100LL;
    if (currentTimeMillis() - _lastDamageTimeMillis >= damageRateMillis) {
        for (const auto& enemy : getWorld()->getEnemies()) {
            if (enemy->isActive() && enemy->getHitBox().intersects(getHitBox())) {
                constexpr int damage = 4;
                enemy->takeDamage(damage);
                _lastDamageTimeMillis = currentTimeMillis();
                break;
            }
        }
    }
}

void Scythe::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void Scythe::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(58 << SPRITE_SHEET_SHIFT, 0, 3 * TILE_SIZE, 3 * TILE_SIZE));
    _sprite.setOrigin(3.f * (float)TILE_SIZE / 2.f, 3.f * (float)TILE_SIZE / 2.f);
    _sprite.setPosition(getPosition());
    _sprite.setRotation((float)randomInt(0, 360));
}
