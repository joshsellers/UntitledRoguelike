#include "Bouncer.h"
#include "../World.h"
#include "../../core/Viewport.h"

Bouncer::Bouncer(sf::Vector2f pos) : Entity(NO_SAVE, pos, 1, TILE_SIZE, TILE_SIZE, false) {
    setMaxHitPoints(20);
    heal(getMaxHitPoints());

    _hitBox.left = pos.x;
    _hitBox.top = pos.y;
    _hitBox.width = TILE_SIZE;
    _hitBox.height = TILE_SIZE;

    _velocity.x = randomInt(0, 1);
    _velocity.y = randomInt(0, 1);
    if (_velocity.x == 0) _velocity.x = -1;
    if (_velocity.y == 0) _velocity.y = -1;

    _velocity.x *= (float)randomInt(100, 400) / 100.f;
    _velocity.y *= (float)randomInt(100, 400) / 100.f;
    
    sf::FloatRect viewport = Viewport::getBounds();
    if (_pos.x < viewport.left || _pos.x > viewport.left + viewport.width || _pos.y < viewport.top || _pos.y > viewport.top + viewport.height) {
        _pos.x = randomInt(viewport.left, viewport.left + viewport.width - TILE_SIZE);
        _pos.y = randomInt(viewport.top, viewport.top + viewport.height - TILE_SIZE);
    }

    _isMob = true;
}

void Bouncer::update() {
    sf::FloatRect viewport = Viewport::getBounds();

    move(_velocity.x, _velocity.y);
    if (_pos.x <= viewport.left) {
        _pos.x = viewport.left;
        _velocity.x = -_velocity.x;
    } else if (_pos.x + TILE_SIZE >= viewport.left + viewport.width) {
        _pos.x = viewport.left + viewport.width - TILE_SIZE;
        _velocity.x = -_velocity.x;
    }

    if (_pos.y <= viewport.top) {
        _pos.y = viewport.top;
        _velocity.y = -_velocity.y;
    } else if (_pos.y + TILE_SIZE >= viewport.top + viewport.height) {
        _pos.y = viewport.top + viewport.height - TILE_SIZE;
        _velocity.y = -_velocity.y;
    }

    _hitBox.left = _pos.x;
    _hitBox.top = _pos.y;

    _sprite.setPosition(getPosition());
}

void Bouncer::draw(sf::RenderTexture& surface) {
    sf::CircleShape circle;
    circle.setFillColor(sf::Color::Red);
    circle.setRadius(TILE_SIZE / 2);
    circle.setPosition(getPosition());
    surface.draw(circle);
}

void Bouncer::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(4 * TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
    _sprite.setPosition(getPosition());
    //_sprite.setOrigin(TILE_SIZE, 0);
}
