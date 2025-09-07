#include "ShopArrow.h"
#include "../../core/Tutorial.h"

void ShopArrow::update() {
    sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_HEIGHT / 2);

    bool foundShop = false;
    float closestDistance = CHUNK_SIZE * 4;
    std::shared_ptr<Entity> closestShop = nullptr;
    for (auto& entity : _world->getEntities()) {
        if (entity->getEntityType() == "shopext" && entity->isActive()) {
            foundShop = true;
            sf::Vector2f entityPos = entity->getPosition();
            entityPos.x += 6 * TILE_SIZE;
            entityPos.y += 4 * TILE_SIZE;
            float dist = std::sqrt(std::pow(playerPos.x - entity->getPosition().x, 2) + std::pow(playerPos.y - entity->getPosition().y, 2));
            if (dist < closestDistance) {
                closestDistance = dist;
                closestShop = entity;
            }
        }
    }

    if (foundShop && closestShop != nullptr) {
        sf::Vector2f shopPos = closestShop->getPosition();
        shopPos.x += 6 * TILE_SIZE;
        shopPos.y += 4 * TILE_SIZE;

        float dist = std::sqrt(std::pow(shopPos.x - playerPos.x, 2) + std::pow(shopPos.y - playerPos.y, 2));

        if (dist <= 200.f) {
            _isVisible = false;
            return;
        }

        float desiredDist = 100.f;
        float distanceRatio = desiredDist / dist;

        sf::Vector2f goalPos((1.f - distanceRatio) * playerPos.x + distanceRatio * shopPos.x, (1.f - distanceRatio) * playerPos.y + distanceRatio * shopPos.y);

        double x = (double)(goalPos.x - playerPos.x);
        double y = (double)(goalPos.y - playerPos.y);

        float angle = (float)(std::atan2(y, x) * (180. / PI)) + 90.f;

        _sprite.setPosition(goalPos);
        _sprite.setRotation(angle);
    }

    _isVisible = foundShop;
}

void ShopArrow::draw(sf::RenderTexture& surface) {
    if (_isVisible && !_world->playerIsInShop() && (Tutorial::isCompleted() || (int)Tutorial::getCurrentStep() >= (int)TUTORIAL_STEP::BUY_BOW)
        && _world->onEnemySpawnCooldown() && !_world->bossIsActive() && _world->getCurrentWaveNumber() < SHOPS_CLOSED_WAVE) surface.draw(_sprite);
}

void ShopArrow::setWorld(World* world) {
    _world = world;
}

void ShopArrow::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(64, 192, TILE_SIZE, TILE_SIZE));
    _sprite.setPosition(0, 0);
    _sprite.setOrigin(_sprite.getTextureRect().width / 2, _sprite.getTextureRect().height / 2);
    _sprite.setColor(sf::Color(0x00000055));
}
