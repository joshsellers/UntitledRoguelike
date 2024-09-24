#include "CreamDrop.h"
#include "../World.h"
#include "../../core/Util.h"
#include "projectiles/ProjectileDataManager.h"

CreamDrop::CreamDrop(sf::Vector2f playerPos) : Entity(NO_SAVE, playerPos, 0, 3 * TILE_SIZE, 3 * TILE_SIZE, false) {
    constexpr int maxDist = 100;

    _impactPos.x = playerPos.x + randomInt(-maxDist, maxDist);
    _impactPos.y = playerPos.y + randomInt(-maxDist, maxDist);
    _sprite.setPosition(_impactPos);

    _dropSprite.setPosition(sf::Vector2f(_impactPos.x, playerPos.y - 300));
}

void CreamDrop::update() {
    constexpr float dropSpeed = 3.f;

    _dropSprite.move(0, dropSpeed);

    if (_dropSprite.getPosition().y + TILE_SIZE * 3 >= _sprite.getPosition().y + TILE_SIZE) {
        constexpr int numProj = 8;
        for (int i = 0; i < numProj; i++) {
            float angle = i * 45;
            fireTargetedProjectile(degToRads(angle), ProjectileDataManager::getData("_PROJECTILE_CREAM"), "NONE", true, false, { 0, 0 });
        }

        if (_sprite.getGlobalBounds().intersects(getWorld()->getPlayer()->getHitBox())) {
            getWorld()->getPlayer()->takeDamage(20);
        }

        deactivate();
    }

    _pos.x = _dropSprite.getPosition().x;
    _pos.y = _dropSprite.getPosition().y;
}

void CreamDrop::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
    surface.draw(_dropSprite);
}

void CreamDrop::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _dropSprite.setTexture(*spriteSheet);
    _dropSprite.setTextureRect(sf::IntRect(896, 1648, TILE_SIZE * 3, TILE_SIZE * 3));
    _dropSprite.setOrigin((float)TILE_SIZE * 3 / 2, 0);

    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(896, 1696, TILE_SIZE * 3, TILE_SIZE));
    _sprite.setOrigin((float)TILE_SIZE * 3 / 2, 0);
}
