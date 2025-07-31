#include "LaserBeam.h"
#include "../World.h"
#include "../../core/ShaderManager.h"

LaserBeam::LaserBeam(const Entity* parent, const float angle, const sf::Uint32 color, const unsigned int width, const unsigned int length, 
    const unsigned int damage, const sf::Vector2f posOffset, const bool onlyDamagePlayer, const long long lifeTime) :
    Entity(NO_SAVE, parent->getPosition() + posOffset, 0, width, width, false), _angle(angle), _color(color), 
    _width(width), _length(length), _damage(damage), _onlyDamagePlayer(onlyDamagePlayer), _posOffset(posOffset), _parent(parent), 
    _lifeTime(lifeTime), _spawnTime(currentTimeMillis()) {

    _ignoreViewport = true;

    _targetPos = { getPosition().x + _length * std::cos(degToRads(_angle)), getPosition().y + _length * std::sin(degToRads(_angle)) };

    _laser.setOrigin({ _width / 2.f, 0 });
    _laser.setFillColor(sf::Color(_color));
    _laser.setPosition(getPosition());
    _laser.setSize({ (float)_width, (float)_length });
    _laser.setRotation(_angle - 90.f);
}

LaserBeam::LaserBeam(const Entity* parent, const sf::Vector2f targetPos, const sf::Uint32 color, 
    const unsigned int width, const unsigned int length, const unsigned int damage, 
    sf::Vector2f posOffset, const bool onlyDamagePlayer, const long long lifeTime) :
    Entity(NO_SAVE, parent->getPosition() + posOffset, 0, width, width, false), _color(color),
    _width(width), _length(length), _damage(damage), _onlyDamagePlayer(onlyDamagePlayer), _posOffset(posOffset), _parent(parent),
    _lifeTime(lifeTime), _spawnTime(currentTimeMillis()) {

    _ignoreViewport = true;

    _targetPos = targetPos;
    _pos = _parent->getPosition() + _posOffset;
    const float dX = _pos.x - _targetPos.x;
    const float dY = _pos.y - _targetPos.y;
    _angle = radsToDeg(std::atan2(dY, dX)) + 180.f;

    _laser.setOrigin({ _width / 2.f, 0 });
    _laser.setFillColor(sf::Color(_color));
    _laser.setPosition(getPosition());
    _laser.setSize({ (float)_width, (float)_length });
    _laser.setRotation(_angle - 90.f);
}

void LaserBeam::update() {
    if (_parent == nullptr || currentTimeMillis() - _spawnTime >= _lifeTime) {
        deactivate();
        return;
    }

    _pos = _parent->getPosition() + _posOffset;
    const float dX = _pos.x - _targetPos.x;
    const float dY = _pos.y - _targetPos.y;
    _targetPos = { getPosition().x + _length * std::cos(degToRads(_angle)), getPosition().y + _length * std::sin(degToRads(_angle)) };

    _laser.setPosition(getPosition());

    if (_onlyDamagePlayer) {
        if (collidesWith(getWorld()->getPlayer()->getHitBox())) getWorld()->getPlayer()->takeDamage(_damage);
    }

    if (_isAnimated) _animCounter++;
}

void LaserBeam::draw(sf::RenderTexture& surface) {
    if (_isAnimated) {
        const int xOffset = ((_animCounter / _ticksPerFrame) % _frameCount);
        _laser.setTextureRect({
            _textureRect.left + xOffset * _textureRect.width,
            _textureRect.top,
            _textureRect.width,
            _textureRect.height
        });
    }

    surface.draw(_laser, _useShader ? ShaderManager::getShader("genericwaves_frag") : sf::RenderStates::Default);
}

bool LaserBeam::collidesWith(sf::FloatRect& hitBox) const {
    const int lineCount = _width / 3 + (((_width / 3) & 1) - 1);
    const float distance = _width / ((float)lineCount - 1);

    const float startDistance = distance * (int)(lineCount / 2);
    const sf::Vector2f startingLine[2] = { 
        {getPosition().x + startDistance * std::cos(degToRads(_angle - 90)), getPosition().y + startDistance * std::sin(degToRads(_angle - 90))},
        {_targetPos.x + startDistance * std::cos(degToRads(_angle - 90)), _targetPos.y + startDistance * std::sin(degToRads(_angle - 90))}
    };

    for (int i = 0; i < lineCount; i++) {
        const sf::Vector2f currentLine[2] = {
            {startingLine[0].x + (distance * std::cos(degToRads(_angle + 90))) * i, startingLine[0].y + (distance * std::sin(degToRads(_angle + 90))) * i},
            {startingLine[1].x + (distance * std::cos(degToRads(_angle + 90))) * i, startingLine[1].y + (distance * std::sin(degToRads(_angle + 90))) * i},
        };

        const bool intersectsLeft = linesIntersect(
            currentLine[0].x, currentLine[0].y,
            currentLine[1].x, currentLine[1].y,
            hitBox.left, hitBox.top,
            hitBox.left, hitBox.top + hitBox.height
        );

        const bool intersectsTop = linesIntersect(
            currentLine[0].x, currentLine[0].y,
            currentLine[1].x, currentLine[1].y,
            hitBox.left, hitBox.top,
            hitBox.left + hitBox.width, hitBox.top
        );

        const bool intersectsRight = linesIntersect(
            currentLine[0].x, currentLine[0].y,
            currentLine[1].x, currentLine[1].y,
            hitBox.left + hitBox.width, hitBox.top,
            hitBox.left + hitBox.width, hitBox.top + hitBox.height
        );

        const bool intersectsBottom = linesIntersect(
            currentLine[0].x, currentLine[0].y,
            currentLine[1].x, currentLine[1].y,
            hitBox.left, hitBox.top + hitBox.height,
            hitBox.left + hitBox.width, hitBox.top + hitBox.height
        );

        if (intersectsLeft || intersectsTop || intersectsRight || intersectsBottom) return true;
    }

    return false;
}

void LaserBeam::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _laser.setTexture(spriteSheet.get());
}

void LaserBeam::setTextureRect(sf::IntRect rect, int frameCount, int ticksPerFrame, bool useShader) {
    loadSprite(getWorld()->getSpriteSheet());

    _textureRect = rect;
    _laser.setTextureRect(rect);
    if (frameCount != 0) {
        _isAnimated = true;
        _frameCount = frameCount;
        _ticksPerFrame = ticksPerFrame;
    }

    _useShader = useShader;
}
