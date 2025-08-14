#include "DevBoss.h"
#include "../World.h"

DevBoss::DevBoss(sf::Vector2f pos) : Boss(DEV_BOSS, pos, 1.f, 2 * TILE_SIZE, 3 * TILE_SIZE,
    {
        BossState(BEHAVIOR_STATE::RUN_COMMAND, 5000LL, 5000LL),
        BossState(BEHAVIOR_STATE::FIRE_PROJECILE, 5000LL, 5000LL)
    }) {
    setMaxHitPoints(179000);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -(TILE_SIZE * 2) / 2 + 5;
    _hitBoxYOffset = 5;
    _hitBox.width = 22;
    _hitBox.height = 42;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;

    _entityType = "devboss";
    _displayName = "The Developer";
}

void DevBoss::subUpdate() {
    if (_animationState == WALKING) {
        sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
        sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 3);

        float dist = std::sqrt(std::pow(cLoc.x - playerPos.x, 2) + std::pow(cLoc.y - playerPos.y, 2));
        constexpr float desiredDist = 100.f;
        float distanceRatio = desiredDist / dist;

        float xa = 0.f, ya = 0.f;

        sf::Vector2f goalPos((1.f - distanceRatio) * playerPos.x + distanceRatio * cLoc.x, (1.f - distanceRatio) * playerPos.y + distanceRatio * cLoc.y);
        if (goalPos.y < cLoc.y) {
            ya--;
            _movingDir = UP;
        } else if (goalPos.y > cLoc.y) {
            ya++;
            _movingDir = DOWN;
        }

        if (goalPos.x < cLoc.x) {
            xa--;
            _movingDir = LEFT;
        } else if (goalPos.x > cLoc.x) {
            xa++;
            _movingDir = RIGHT;
        }

        if (xa && ya) {
            xa *= 0.785398;
            ya *= 0.785398;

            if (std::abs(xa) > std::abs(ya)) {
                if (xa > 0) _movingDir = RIGHT;
                else _movingDir = LEFT;
            } else {
                if (ya > 0) _movingDir = DOWN;
                else _movingDir = UP;
            }
        }

        if (dist < desiredDist) {
            xa = 0;
            ya = 0;
        }

        move(xa, ya);

        if (!isMoving()) _movingDir = DOWN;

        _sprite.setPosition(getPosition());
        _headSprite.setPosition(getPosition().x, getPosition().y + 3);
        _bodySprite.setPosition(getPosition().x, getPosition().y + TILE_SIZE * 2);

        _hitBox.left = getPosition().x + _hitBoxXOffset;
        _hitBox.top = getPosition().y + _hitBoxYOffset;
    }
}

void DevBoss::draw(sf::RenderTexture& surface) {
    if (_animationState == WALKING) {
        const int yFrame = isMoving() ? (_numSteps >> _animSpeed & 3) : 0;
        _headSprite.setTextureRect({
            (98 + (_isBlinking ? 2 : 0)) << SPRITE_SHEET_SHIFT,
            (65 + yFrame * 2) << SPRITE_SHEET_SHIFT,
            TILE_SIZE * 2,
            TILE_SIZE * 2
        });

        _bodySprite.setTextureRect({
            (98 + (int)_movingDir) << SPRITE_SHEET_SHIFT,
            (73 + yFrame) << SPRITE_SHEET_SHIFT,
            TILE_SIZE,
            TILE_SIZE
        });

        surface.draw(_bodySprite);
        surface.draw(_headSprite);
    }
}

void DevBoss::onStateChange(const BossState previousState, const BossState newState) {
}

void DevBoss::runCurrentState() {
}

void DevBoss::blink() {
    const long long blinkDuration = 150LL;
    const int blinkChance = 600;
    if (!_isBlinking) {
        _isBlinking = randomInt(0, blinkChance) == 0;
        if (_isBlinking) _blinkStartTime = currentTimeMillis();
    } else if (currentTimeMillis() - _blinkStartTime > blinkDuration) _isBlinking = false;
}

void DevBoss::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect({102 << SPRITE_SHEET_SHIFT, 68 << SPRITE_SHEET_SHIFT, TILE_SIZE * 2, TILE_SIZE * 3});
    _sprite.setOrigin((float)TILE_SIZE * 2.f / 2.f, 0);
    _sprite.setPosition(getPosition());

    _headSprite.setTexture(*spriteSheet);
    _headSprite.setTextureRect({98 << SPRITE_SHEET_SHIFT, 65 << SPRITE_SHEET_SHIFT, TILE_SIZE * 2, TILE_SIZE * 2});
    _headSprite.setOrigin((float)TILE_SIZE * 2.f / 2.f, 0);
    _headSprite.setPosition(getPosition().x, getPosition().y + 3);

    _bodySprite.setTexture(*spriteSheet);
    _bodySprite.setTextureRect({98 << SPRITE_SHEET_SHIFT, 73 << SPRITE_SHEET_SHIFT, TILE_SIZE, TILE_SIZE});
    _bodySprite.setOrigin((float)TILE_SIZE / 2.f, 0);
    _bodySprite.setPosition(getPosition().x, getPosition().y + TILE_SIZE * 2);
}