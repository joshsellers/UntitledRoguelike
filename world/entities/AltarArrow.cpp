#include "AltarArrow.h"

void AltarArrow::update() {
    if (_altarSpawned) {
        sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_HEIGHT / 2);
        auto altarPos = _altarPos;
        altarPos.x += 96 / 2;
        altarPos.y += 72;

        float dist = std::sqrt(std::pow(altarPos.x - playerPos.x, 2) + std::pow(altarPos.y - playerPos.y, 2));

        if (dist <= 300.f) {
            _isVisible = false;
            return;
        }

        float desiredDist = 100.f;
        float distanceRatio = desiredDist / dist;

        sf::Vector2f goalPos((1.f - distanceRatio) * playerPos.x + distanceRatio * (altarPos.x), (1.f - distanceRatio) * playerPos.y + distanceRatio * (altarPos.y));

        double x = (double)(goalPos.x - playerPos.x);
        double y = (double)(goalPos.y - playerPos.y);

        float angle = (float)(std::atan2(y, x) * (180. / PI)) + 90.f;

        _sprite.setPosition(goalPos);
        _sprite.setRotation(angle);

        _isVisible = true;

        constexpr long long altarCheckRate = 2000LL;
        if (currentTimeMillis() - _lastAltarCheckTime >= altarCheckRate) {
            checkIfAltarDespawned();
            _lastAltarCheckTime = currentTimeMillis();
        }
    }
}

void AltarArrow::draw(sf::RenderTexture& surface) {
    if (_isVisible && !_world->playerIsInShop()) surface.draw(_sprite);
}

void AltarArrow::altarSpawned(sf::Vector2f pos) {
    _altarPos = pos;
    _altarSpawned = true;

    _lastAltarCheckTime = currentTimeMillis();
}

void AltarArrow::altarActivated() {
    _altarSpawned = false;
    _isVisible = false;
}

void AltarArrow::setWorld(World* world) {
    _world = world;
}

void AltarArrow::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(80, 208, TILE_SIZE, TILE_SIZE));
    _sprite.setPosition(0, 0);
    _sprite.setOrigin(_sprite.getTextureRect().width / 2, _sprite.getTextureRect().height / 2);
}

void AltarArrow::checkIfAltarDespawned() {
    if (_world != nullptr) {
        bool foundAltar = false;
        for (const auto& prop : _world->getEntities()) {
            if (prop->getEntityType() == "altar") {
                foundAltar = true;
                break;
            }
        }

        if (!foundAltar) altarActivated();
    }
}
