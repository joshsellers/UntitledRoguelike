#include "CreamDropper.h"
#include "../../core/Util.h"
#include "projectiles/ProjectileDataManager.h"
#include "../../core/MessageManager.h"
#include "CreamDrop.h"
#include "../World.h"

CreamDropper::CreamDropper(sf::Vector2f pos, CreamBoss* creamBoss, const unsigned int dropCount) : Entity(NO_SAVE, pos, 0, 3 * TILE_SIZE, 3 * TILE_SIZE, false),
    _creamBoss(creamBoss), _dropCount(dropCount) {
}

void CreamDropper::update() {
    constexpr int ticksPerFrame = 3;
    constexpr int frameCount = 11;
    _frameNumber = ((_animCounter / ticksPerFrame) % frameCount);

    if (_droppedSoFar < _dropCount && _frameNumber != _lastFrameNumber && _frameNumber == frameCount - 1) {
        _dropTimes.push(currentTimeMillis() + _dropRate);

        fireTargetedProjectile(degToRads(270), ProjectileDataManager::getData("_PROJECTILE_LARGE_CREAM_DROP"), "NONE", false, false, {-TILE_SIZE, 0});
        _droppedSoFar++;
    }

    if (!_dropTimes.empty() && currentTimeMillis() >= _dropTimes.front()) {
        auto drop = std::shared_ptr<CreamDrop>(new CreamDrop(getWorld()->getPlayer()->getPosition()));
        drop->loadSprite(getWorld()->getSpriteSheet());
        drop->setWorld(getWorld());
        getWorld()->addEntity(drop);

        _dropTimes.pop();
    }

    if (_droppedSoFar == _dropCount && _dropTimes.empty()) deactivate();

    _sprite.setPosition(sf::Vector2f(
        _creamBoss->getPosition().x, 
            _creamBoss->getPosition().y - (TILE_SIZE))
    );
    _pos.x = _sprite.getPosition().x;
    _pos.y = _sprite.getPosition().y;

    _animCounter++;
    _lastFrameNumber = _frameNumber;
}

void CreamDropper::draw(sf::RenderTexture& surface) {
    if (_droppedSoFar < _dropCount) {
        _sprite.setTextureRect(sf::IntRect(
            368 + _frameNumber * TILE_SIZE * 3,
            1648,
            TILE_SIZE * 3,
            TILE_SIZE * 8
        ));
    } else {
        _sprite.setTextureRect(sf::IntRect(
            368,
            1648,
            TILE_SIZE * 3,
            TILE_SIZE * 8
        ));
    }

    surface.draw(_sprite);
}

void CreamDropper::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(368, 1648, TILE_SIZE * 3, TILE_SIZE * 8));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 3 / 2, 0);
}
