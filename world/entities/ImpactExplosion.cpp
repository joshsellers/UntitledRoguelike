#include "ImpactExplosion.h"
#include "../../core/Util.h"
#include "Explosion.h"
#include "../World.h"

ImpactExplosion::ImpactExplosion(sf::Vector2f pos, unsigned int layerCount, unsigned int propogationRate, bool onlyDamagePlayer, bool onlyDamageEnemies) 
    : Entity(NO_SAVE, pos, 0.f, 0, 0, false), 
_layerCount(layerCount), _propogationRate(propogationRate),
_onlyDamagePlayer(onlyDamagePlayer), _onlyDamageEnemies(onlyDamageEnemies) {
    if (propogationRate == 0) {
        MessageManager::displayMessage("Invalid propogation rate for ImpactExplosion", WARN, 5);
        deactivate();
    }
}

void ImpactExplosion::update() {
    const long long timeBetweenLayers = 100LL / _propogationRate;
    if (_explosionLayersSpawned < _layerCount && currentTimeMillis() - _lastLayerSpawnTime > timeBetweenLayers) {
        const sf::Vector2f centerPoint = getPosition();

        _explosionLayersSpawned++;
        const float explosionRadius = 12.f * _explosionLayersSpawned;
        for (int i = 0; i < 6 * _explosionLayersSpawned; i++) {
            float angle = i * (360.f / (6 * _explosionLayersSpawned));

            const sf::Vector2f explosionPos(centerPoint.x + explosionRadius * std::cos(degToRads(angle)), centerPoint.y + explosionRadius * std::sin(degToRads(angle)));
            const auto& explosion = std::shared_ptr<Explosion>(new Explosion(explosionPos, 16, _onlyDamagePlayer, _onlyDamageEnemies));
            explosion->setWorld(getWorld());
            explosion->loadSprite(getWorld()->getSpriteSheet());
            getWorld()->addEntity(explosion);
        }

        _lastLayerSpawnTime = currentTimeMillis();
    } else if (_explosionLayersSpawned >= _layerCount) deactivate();
}

void ImpactExplosion::draw(sf::RenderTexture& surface) {
}

void ImpactExplosion::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
}
