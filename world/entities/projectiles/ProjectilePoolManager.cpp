#include "ProjectilePoolManager.h"
#include "../../../core/MessageManager.h"

void ProjectilePoolManager::init() {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        ProjectileData projData = {0, 0, sf::IntRect(), false};
        _pool[i] = new Projectile(sf::Vector2f(0, 0), nullptr, 0, 0, projData);
        _pool[i]->_isActive = false;
    }
}

void ProjectilePoolManager::shutdown() {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        delete _pool[i];
    }
}

void ProjectilePoolManager::update() {
    for (auto& projectile : _pool) {
        if (projectile->isActive()) {
            projectile->update();
        }
    }
}

void ProjectilePoolManager::draw(sf::RenderTexture& surface) {
    for (auto& projectile : _pool) {
        if (projectile->isActive()) {
            projectile->draw(surface);
        }
    }
}

void ProjectilePoolManager::addProjectile(sf::Vector2f pos, Entity* parent, float directionAngle, float velocity, const ProjectileData data, 
    bool onlyHitPlayer, int damageBoost, bool addParentVelocity, int passThroughCount) {

    bool foundAvailable = false;
    Projectile* lastProjectile = nullptr;
    for (auto& projectile : _pool) {
        if (!projectile->isActive()) {
            projectile->setWorld(parent->getWorld());
            projectile->reset(pos, parent, directionAngle, velocity, data, onlyHitPlayer, damageBoost, addParentVelocity, passThroughCount);
            foundAvailable = true;
            return;
        }
        
        lastProjectile = projectile;
    }

    if (!foundAvailable && lastProjectile != nullptr) {
        lastProjectile->setWorld(parent->getWorld());
        lastProjectile->reset(pos, parent, directionAngle, velocity, data, onlyHitPlayer, damageBoost, addParentVelocity, passThroughCount);
    }
}
