#include "Lightning.h"
#include "../../core/Util.h"
#include <boost/random/uniform_int_distribution.hpp>
#include "../World.h"
#include "../../core/SoundManager.h"

Lightning::Lightning(sf::Vector2f pos) : Entity(NO_SAVE, pos, 0, 0, 0, false) {
    init(pos, 5, 25);
}

Lightning::Lightning(sf::Vector2f pos, int damage, float damageApothem) : Entity(NO_SAVE, pos, 0, 0, 0, false) {
    init(pos, damage, damageApothem);
}

void Lightning::init(sf::Vector2f pos, int damage, float damageApothem) {
    _spawnTime = currentTimeMillis();
    _seed = pos.x + pos.y * (pos.x - pos.y);

    _lifeTime = randomInt(500, 1000);

    _damage = damage;
    _damageApothem = damageApothem;

    _hitBox.left = getPosition().x + 5 - damageApothem;
    _hitBox.top = getPosition().y + 35 - damageApothem;
    _hitBox.width = damageApothem * 2;
    _hitBox.height = damageApothem * 2;

    const int lightningSoundIndex = randomInt(0, 3);
    std::string soundName = "lightning_" + std::to_string(lightningSoundIndex);
    SoundManager::playSound(soundName);
}

void Lightning::update() {
    const long long curTime = currentTimeMillis();

    if (curTime - _spawnTime >= _lifeTime) deactivate();

    constexpr long long redrawInterval = 150LL;
    if (curTime - _lastRedrawTime >= redrawInterval) {
        _seed += 1;
        _lastRedrawTime = curTime;
    }

    constexpr long long hitInterval = 100LL;
    if (curTime - _lastHitTime >= hitInterval) {
        for (const auto& enemy : getWorld()->getEnemies()) {
            if (enemy->isActive() && _hitBox.intersects(enemy->getHitBox())) {
                enemy->takeDamage(_damage);
            }
        }

        _lastHitTime = curTime;
    }
}

void Lightning::draw(sf::RenderTexture& surface) {
    _rng.seed(_seed);

    constexpr int segmentCount = 32;
    constexpr int lightningHeight = 600; 
    
    boost::random::uniform_int_distribution<> segmentLengthDist(30, 50);
    boost::random::uniform_int_distribution<> xOffset(-10, 10);
    float lastX = getPosition().x;
    float segmentLength = segmentLengthDist(_rng);;
    for (int i = 0; i < segmentCount; i++) {
        sf::ConvexShape segment;
        segment.setFillColor(sf::Color(0xFFFFFFFF));

        constexpr float segmentWidth = 10.f;
        segment.setPointCount(4);

        int segXOffset = xOffset(_rng);
        segment.setPoint(0, sf::Vector2f(lastX + segXOffset, getPosition().y - segmentLength * i));
        segment.setPoint(3, sf::Vector2f(lastX + segmentWidth + segXOffset, getPosition().y - segmentLength * i));
        segment.setPoint(1, sf::Vector2f(lastX, getPosition().y - segmentLength * i + segmentLength));
        segment.setPoint(2, sf::Vector2f(lastX + segmentWidth, getPosition().y - segmentLength * i + segmentLength));
        lastX += segXOffset;

        surface.draw(segment);
    }
}

void Lightning::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(0, 0, 10, 1200));
    _sprite.setPosition(getPosition().x, getPosition().y - 1200);
}