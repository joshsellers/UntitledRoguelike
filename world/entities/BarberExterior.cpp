#include "BarberExterior.h"
#include "../World.h"

BarberExterior::BarberExterior(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, 128, 80, true) {
    _pos = pos;
    loadSprite(spriteSheet);

    _hitBoxXOffset = 24;
    _hitBoxYOffset = 48;
    _hitBox.width = TILE_SIZE * 2;
    _hitBox.height = 4;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    setMaxHitPoints(10000000);
    heal(getMaxHitPoints());

    _entityType = "barberext";

    _hasColliders = true;
    sf::FloatRect colliderLeft;
    colliderLeft.left = getSprite().getPosition().x + 16;
    colliderLeft.top = getSprite().getPosition().y + (27);
    colliderLeft.width = 8;
    colliderLeft.height = 30;

    sf::FloatRect colliderRight;
    colliderRight.left = getSprite().getPosition().x + 57;
    colliderRight.top = getSprite().getPosition().y + (27);
    colliderRight.width = 54;
    colliderRight.height = 30;

    sf::FloatRect colliderTop;
    colliderTop.left = getSprite().getPosition().x + 24;
    colliderTop.top = getSprite().getPosition().y + (27);
    colliderTop.width = 32;
    colliderTop.height = 20;

    _colliders.push_back(colliderLeft);
    _colliders.push_back(colliderRight);
    _colliders.push_back(colliderTop);
}

void BarberExterior::update() {
    if (!getWorld()->playerIsInShop()) {
        _hasColliders = true;
        auto& entity = getWorld()->getPlayer();
        if (entity->isActive() && entity->getEntityType() == "player" && entity->getHitBox().intersects(getHitBox())) {
            getWorld()->enterBuilding("barber", sf::Vector2f(_pos.x + 8, _pos.y));
        }
    } else _hasColliders = false;
}

void BarberExterior::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void BarberExterior::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(736, 480, 128, 80)
    );
    _sprite.setPosition(getPosition());
}
