#include "BarberInterior.h"
#include "../World.h"

BarberInterior::BarberInterior(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) : Entity(NO_SAVE, pos, 0, BARBER_INTERIOR_WIDTH, BARBER_INTERIOR_HEIGHT, false) {
    loadSprite(spriteSheet);
    int posY = _pos.y;
    _pos.y -= BARBER_INTERIOR_HEIGHT * 2;
    _sprite.setPosition(_pos.x, posY);

    _entityType = "barberint";

    _hitBoxXOffset = TILE_SIZE;
    _hitBoxYOffset = (BARBER_INTERIOR_HEIGHT - 2) * 3;
    _hitBox.width = TILE_SIZE * 2;
    _hitBox.height = 2;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset + 4;

    setMaxHitPoints(100000000);
    heal(getMaxHitPoints());

    _hasColliders = true;
    sf::FloatRect topCollider;
    topCollider.left = getSprite().getPosition().x;
    topCollider.top = getSprite().getPosition().y - 10;
    topCollider.width = BARBER_INTERIOR_WIDTH;
    topCollider.height = 18;

    sf::FloatRect leftCollider;
    leftCollider.left = getSprite().getPosition().x - 10;
    leftCollider.top = getSprite().getPosition().y;
    leftCollider.width = 10;
    leftCollider.height = BARBER_INTERIOR_HEIGHT;

    sf::FloatRect rightCollider;
    rightCollider.left = getSprite().getPosition().x + BARBER_INTERIOR_WIDTH;
    rightCollider.top = getSprite().getPosition().y;
    rightCollider.width = 10;
    rightCollider.height = BARBER_INTERIOR_HEIGHT;

    sf::FloatRect bottomCollider;
    bottomCollider.left = getSprite().getPosition().x;
    bottomCollider.top = getSprite().getPosition().y + BARBER_INTERIOR_HEIGHT;
    bottomCollider.width = BARBER_INTERIOR_WIDTH;
    bottomCollider.height = 10;

    _colliders.push_back(topCollider);
    _colliders.push_back(leftCollider);
    _colliders.push_back(rightCollider);
    _colliders.push_back(bottomCollider);
}

void BarberInterior::update() {
    auto& entity = getWorld()->getPlayer();
    if (entity->isActive() && entity->getEntityType() == "player" && entity->getHitBox().intersects(getHitBox())) {
        getWorld()->exitBuilding();
        deactivate();
    }
}

void BarberInterior::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void BarberInterior::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(768, 608, BARBER_INTERIOR_WIDTH, BARBER_INTERIOR_HEIGHT)
    );
}

sf::Vector2f BarberInterior::getEntrancePos() {
    return sf::Vector2f(_hitBox.left + TILE_SIZE / 2, _hitBox.top - TILE_SIZE * 2 - 4);
}
