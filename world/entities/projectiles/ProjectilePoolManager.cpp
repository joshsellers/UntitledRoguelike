#include "ProjectilePoolManager.h"
#include "../../../core/MessageManager.h"
#include "../../../core/Viewport.h"

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
    const sf::FloatRect cameraBounds = Viewport::getBounds();
    for (auto& projectile : _pool) {
        if (projectile->isActive() && projectile->getHitBox().intersects(cameraBounds)) {
            projectile->draw(surface);
            
            if (_debug) {
                sf::RectangleShape hitBox;
                sf::FloatRect box = projectile->getHitBox();
                hitBox.setPosition(box.left, box.top);
                hitBox.setSize(sf::Vector2f(box.width, box.height));
                hitBox.setFillColor(sf::Color::Transparent);
                hitBox.setOutlineColor(sf::Color(0xFF0000FF));
                hitBox.setOutlineThickness(1.f);
                surface.draw(hitBox);
            }
        }
    }
}

Projectile* ProjectilePoolManager::addProjectile(sf::Vector2f pos, Entity* parent, float directionAngle, float velocity, const ProjectileData data, 
    bool onlyHitPlayer, int damageBoost, bool addParentVelocity, int passThroughCount, EXPLOSION_BEHAVIOR explosionBehavior) {

    bool foundAvailable = false;
    Projectile* lastProjectile = nullptr;
    for (auto& projectile : _pool) {
        if (!projectile->isActive()) {
            projectile->setWorld(parent->getWorld());
            projectile->reset(pos, parent, directionAngle, velocity, data, onlyHitPlayer, damageBoost, addParentVelocity, passThroughCount, explosionBehavior);
            foundAvailable = true;
            return projectile;
        }
        
        lastProjectile = projectile;
    }

    if (!foundAvailable && lastProjectile != nullptr) {
        lastProjectile->setWorld(parent->getWorld());
        lastProjectile->reset(pos, parent, directionAngle, velocity, data, onlyHitPlayer, damageBoost, addParentVelocity, passThroughCount, explosionBehavior);
        return lastProjectile;
    }
}

void ProjectilePoolManager::removeAll() {
    for (auto& projectile : _pool) {
        projectile->deactivate();
    }
}

void ProjectilePoolManager::setDebug(bool debug) {
    _debug = debug;
}
