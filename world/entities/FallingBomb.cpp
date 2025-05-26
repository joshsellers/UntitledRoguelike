#include "FallingBomb.h"
#include "../World.h"
#include "ImpactExplosion.h"

FallingBomb::FallingBomb(sf::Vector2f targetPos) : Entity(NO_SAVE, {targetPos.x, targetPos.y - 300}, 1.f, TILE_SIZE * 2, TILE_SIZE * 3, false), _targetPos(targetPos) {
    _displayOnTop = true;
}

void FallingBomb::update() {
    if (!_landed) {
        constexpr float dropSpeed = 8.f;
        move(0, dropSpeed);
        _sprite.setPosition(getPosition());
        if (_pos.y >= _targetPos.y) _landed = true;
    } else {
        const auto& impact = std::shared_ptr<ImpactExplosion>(new ImpactExplosion(_targetPos, 4, 2, false, true));
        impact->setWorld(getWorld());
        getWorld()->addEntity(impact);

        deactivate();
    }
}

void FallingBomb::draw(sf::RenderTexture& surface) {
    if (!_landed) surface.draw(_sprite);
}

void FallingBomb::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(1904, 656, TILE_SIZE * 2, TILE_SIZE * 3));
    _sprite.setOrigin(TILE_SIZE, 0);
    _sprite.setPosition(getPosition());
}
