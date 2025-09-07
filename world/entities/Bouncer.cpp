#include "Bouncer.h"
#include "../World.h"
#include "../../core/Viewport.h"

Bouncer::Bouncer(ENTITY_SAVE_ID saveId, sf::Vector2f pos, float baseSpeed, const int spriteWidth, const int spriteHeight, const float angle) : 
    Entity(saveId, pos, 1, spriteWidth, spriteHeight, false), _angle(angle) {
    _velocity.x = randomInt(0, 1);
    _velocity.y = randomInt(0, 1);
    if (_velocity.x == 0) _velocity.x = -1;
    if (_velocity.y == 0) _velocity.y = -1;

    _velocity.x *= baseSpeed * std::cos(degToRads(angle));
    _velocity.y *= baseSpeed * std::sin(degToRads(angle));
    
    sf::FloatRect viewport = Viewport::getBounds();
    if (_pos.x - (float)_spriteWidth / 2 < viewport.left 
        || _pos.x + (float)_spriteWidth / 2 > viewport.left + viewport.width 
        || _pos.y < viewport.top 
        || _pos.y + _spriteHeight > viewport.top + viewport.height) {
        _pos.x = randomInt(viewport.left + (float)_spriteWidth / 2, viewport.left + viewport.width - (float)_spriteWidth / 2);
        _pos.y = randomInt(viewport.top, viewport.top + viewport.height - _spriteHeight);
    }

    _isMob = true;
}

void Bouncer::update() {
    if (_isEnemy && getWorld() != nullptr && !_checkedPlayerCollision) {
        _hitBox.left = _pos.x + _hitBoxXOffset;
        _hitBox.top = _pos.y + _hitBoxYOffset;
        const sf::FloatRect& playerHitBox = getWorld()->getPlayer()->getHitBox();
        constexpr float expansion = 60.f;
        const sf::FloatRect expandedPlayerHitBox(playerHitBox.left - expansion, playerHitBox.top - expansion, playerHitBox.width + expansion * 2, playerHitBox.height + expansion * 2);
        if (expandedPlayerHitBox.intersects(getHitBox())) {
            deactivate();
        }
        _checkedPlayerCollision = true;
    }

    preupdate();

    sf::FloatRect viewport = Viewport::getBounds();

    move(_velocity.x, _velocity.y);
    if (_pos.x - (float)_spriteWidth / 2 <= viewport.left) {
        _pos.x = viewport.left + (float)_spriteWidth / 2;
        _velocity.x = -_velocity.x;
    } else if (_pos.x + (float)_spriteWidth / 2 >= viewport.left + viewport.width) {
        _pos.x = viewport.left + viewport.width - ((float)_spriteWidth / 2);
        _velocity.x = -_velocity.x;
    }

    if (_pos.y <= viewport.top) {
        _pos.y = viewport.top;
        _velocity.y = -_velocity.y;
    } else if (_pos.y + _spriteHeight >= viewport.top + viewport.height) {
        _pos.y = viewport.top + viewport.height - _spriteHeight;
        _velocity.y = -_velocity.y;
    }

    _hitBox.left = _pos.x + _hitBoxXOffset;
    _hitBox.top = _pos.y + _hitBoxYOffset;

    _sprite.setPosition(getPosition());
}

std::string Bouncer::getSaveData() const {
    return std::to_string(_angle);
}
