#include "SlimeBall.h"
#include "World.h"
#include "Item.h"

SlimeBall::SlimeBall(sf::Vector2f pos, Entity* parent) : Orbiter(pos, 32, 2, parent) {}

void SlimeBall::subUpdate() {
    const long long fireRate = 1000LL;

    if (currentTimeMillis() - _lastFireTime >= fireRate) {
        const float fireRange = 350.f;
        float closestDistance = fireRange;
        std::shared_ptr<Entity> closestEnemy = nullptr;
        for (auto& entity : getWorld()->getEntities()) {
            if (entity->isEnemy() && entity->isActive() && (!entity->isInitiallyDocile() || entity->isHostile())) {
                float dist = std::sqrt(std::pow(_pos.x - entity->getPosition().x, 2) + std::pow(_pos.y - entity->getPosition().y, 2));
                if (dist <= fireRange && dist < closestDistance) {
                    closestDistance = dist;
                    closestEnemy = entity;
                }
            }
        }

        if (closestEnemy != nullptr) {
            fireTargetedProjectile(
                sf::Vector2f(closestEnemy->getPosition().x, closestEnemy->getPosition().y + closestEnemy->getSpriteSize().y / 2), 
                Item::DATA_PROJECTILE_SLIME_BALL, "NONE"
            );
            _lastFireTime = currentTimeMillis();
        }
    }

    if (!getParent()->isActive()) deactivate();
}

void SlimeBall::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void SlimeBall::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(96, 64, TILE_SIZE, TILE_SIZE));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
}
