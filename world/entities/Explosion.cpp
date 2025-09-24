#include "Explosion.h"
#include "../../core/Util.h"
#include "../../core/SoundManager.h"
#include "../World.h"

Explosion::Explosion(sf::Vector2f pos, bool onlyDamagePlayer, bool onlyDamageEnemies) : 
    Entity(NO_SAVE, pos, 0, 3 * TILE_SIZE, 3 * TILE_SIZE, false), _onlyDamagePlayer(onlyDamagePlayer), _onlyDamageEnemies(onlyDamageEnemies) {
    init(pos, 15);
}

Explosion::Explosion(sf::Vector2f pos, int damage, bool onlyDamagePlayer, bool onlyDamageEnemies) : 
    Entity(NO_SAVE, pos, 0, 3 * TILE_SIZE, 3 * TILE_SIZE, false), _onlyDamagePlayer(onlyDamagePlayer), _onlyDamageEnemies(onlyDamageEnemies) {
    init(pos, damage);
}

void Explosion::init(sf::Vector2f pos, int damage) {
    _damage = damage;
    setMaxHitPoints(100000000);
    heal(getMaxHitPoints());

    _hitBox.left = getPosition().x - (float)TILE_SIZE * 3.f / 2.f - (float) TILE_SIZE;
    _hitBox.top = getPosition().y - (float)TILE_SIZE;
    _hitBox.width = (float)TILE_SIZE * 5.f;
    _hitBox.height = (float)TILE_SIZE * 5.f;

    _entityType = "explosion";

    const int soundIndex = randomInt(0, 3);
    std::string soundName = "explosion_" + std::to_string(soundIndex);
    SoundManager::playSound(soundName);
}

void Explosion::update() {
    if (_currentFrame >= 1 && _currentFrame <= 6) {
        if (!_onlyDamagePlayer && !_onlyDamageEnemies) {
            for (const auto& entity : getWorld()->getNearbyEntites(getPosition())) {
                if (entity->getEntityType() != "explosion" && entity->getSaveId() != BABY_BOSS && entity->isActive() && entity->getHitBox().intersects(getHitBox())) {
                    entity->takeDamage(_damage);
                }
            }
        } else if (_onlyDamagePlayer && !_onlyDamageEnemies) {
            if (getWorld()->getPlayer()->getHitBox().intersects(getHitBox())) getWorld()->getPlayer()->takeDamage(_damage);
        } else if (_onlyDamagePlayer && _onlyDamageEnemies) {
            if (getWorld()->getPlayer()->getHitBox().intersects(getHitBox())) getWorld()->getPlayer()->takeDamage(_damage);
            for (const auto& enemy : getWorld()->getNearbyEntites(getPosition(), true)) {
                if (enemy->getSaveId() != BABY_BOSS && enemy->isActive() && enemy->getHitBox().intersects(getHitBox())) {
                    enemy->takeDamage(_damage);
                }
            }
        } else if (!_onlyDamagePlayer && _onlyDamageEnemies) {
            for (const auto& enemy : getWorld()->getNearbyEntites(getPosition(), true)) {
                if (enemy->getSaveId() != BABY_BOSS && enemy->isActive() && enemy->getHitBox().intersects(getHitBox())) {
                    enemy->takeDamage(_damage);
                }
            }
        }
    }

    if (_currentFrame == 13) deactivate();
    constexpr int ticksPerFrame = 2;
    constexpr int frameCount = 14;
    _currentFrame = ((_animCounter / ticksPerFrame) % frameCount);
    _animCounter++;
}

void Explosion::draw(sf::RenderTexture& surface) {
    const sf::Vector2i textureCoords(64 * TILE_SIZE, 24 * TILE_SIZE);

    _sprite.setTextureRect(sf::IntRect(
        textureCoords.x + _currentFrame * TILE_SIZE * 3,
        textureCoords.y,
        TILE_SIZE * 3,
        TILE_SIZE * 3
    ));

    surface.draw(_sprite);
}

void Explosion::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(64 * TILE_SIZE, 24 * TILE_SIZE, TILE_SIZE * 3, TILE_SIZE * 4));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((3.f * (float)TILE_SIZE) / 2, 0);
}
