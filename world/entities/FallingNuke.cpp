#include "FallingNuke.h"
#include "../World.h"
#include "../../core/Util.h"
#include "Explosion.h"

FallingNuke::FallingNuke(sf::Vector2f playerPos) : Entity(NO_SAVE, playerPos, 0, 3 * TILE_SIZE, 5 * TILE_SIZE, false) {
    constexpr int maxDist = 75;

    _impactPos.x = playerPos.x + randomInt(-maxDist, maxDist);
    _impactPos.y = playerPos.y + randomInt(-maxDist, maxDist);
    _sprite.setPosition(_impactPos);

    _displayOnTop = true;

    _dropSprite.setPosition(sf::Vector2f(_impactPos.x, playerPos.y - 300));
}

void FallingNuke::update() {
    if (!_landed) {
        constexpr float dropSpeed = 5.f;
        _dropSprite.move(0, dropSpeed);
    }

    constexpr long long timeBetweenLayers = 50LL;
    constexpr int maxLayers = 20;
    if (!_landed && _dropSprite.getPosition().y + TILE_SIZE * 5 >= _sprite.getPosition().y + TILE_SIZE) {
        _landed = true;
        _shadow->deactivate();
    } else if (_landed && _explosionLayersSpawned < maxLayers && currentTimeMillis() - _lastLayerSpawnTime > timeBetweenLayers) {
        const sf::Vector2f centerPoint(_dropSprite.getPosition().x, _dropSprite.getPosition().y + (float)TILE_SIZE * 5.f / 2);

        _explosionLayersSpawned++;
        const float explosionRadius = 12.f * _explosionLayersSpawned;
        for (int i = 0; i < 6 * _explosionLayersSpawned; i++) {
            float angle = i * (360.f / (6 * _explosionLayersSpawned));

            const sf::Vector2f explosionPos(centerPoint.x + explosionRadius * std::cos(degToRads(angle)), centerPoint.y + explosionRadius * std::sin(degToRads(angle)));
            const auto& explosion = std::shared_ptr<Explosion>(new Explosion(explosionPos, 16, true));
            explosion->setWorld(getWorld());
            explosion->loadSprite(getWorld()->getSpriteSheet());
            getWorld()->addEntity(explosion);
        }

        _lastLayerSpawnTime = currentTimeMillis();
    } else if (_explosionLayersSpawned >= maxLayers) deactivate();

    _pos.x = _dropSprite.getPosition().x;
    _pos.y = _dropSprite.getPosition().y;
}

void FallingNuke::draw(sf::RenderTexture& surface) {
    if (!_landed) {
        surface.draw(_dropSprite);
    }
}

void FallingNuke::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _dropSprite.setTexture(*spriteSheet);
    _dropSprite.setTextureRect(sf::IntRect(116 * TILE_SIZE, 40 * TILE_SIZE, TILE_SIZE * 3, TILE_SIZE * 5));
    _dropSprite.setOrigin((float)TILE_SIZE * 3 / 2, 0);

    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(896, 1696, TILE_SIZE * 3, TILE_SIZE));
    _sprite.setOrigin((float)TILE_SIZE * 3 / 2, 0);
}

void FallingNuke::setWorld(World* world) {
    _world = world;

    _shadow = std::shared_ptr<FallingObjectShadow>(new FallingObjectShadow(_impactPos));
    _shadow->loadSprite(getWorld()->getSpriteSheet());
    getWorld()->addEntity(_shadow);
}
