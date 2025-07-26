#ifndef _LASER_BEAM_H
#define _LASER_BEAM_H

#include "Entity.h"

class LaserBeam : public Entity {
public:
    LaserBeam(const Entity* parent, const float angle, const sf::Uint32 color, const unsigned int width, const unsigned int length, const unsigned int damage, 
        sf::Vector2f posOffset = {0, 0}, const bool onlyDamagePlayer = false, const long long lifeTime = 1250LL);

    LaserBeam(const Entity* parent, const sf::Vector2f targetPos, const sf::Uint32 color, const unsigned int width, const unsigned int length, const unsigned int damage,
        sf::Vector2f posOffset = { 0, 0 }, const bool onlyDamagePlayer = false, const long long lifeTime = 1250LL);

    void update();
    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
    void setTextureRect(sf::IntRect rect, int frameCount = 0, int ticksPerFrame = 1, bool useShader = false);
private:
    const Entity* _parent;
    const sf::Vector2f _posOffset;
    float _angle;
    sf::Vector2f _targetPos;
    const sf::Uint32 _color;
    const unsigned int _width;
    const unsigned int _length;
    const unsigned int _damage;
    const long long _lifeTime;
    const long long _spawnTime;
    
    const bool _onlyDamagePlayer;

    bool _isAnimated = false;
    sf::IntRect _textureRect;
    int _frameCount = 0;
    int _ticksPerFrame = 1;
    int _animCounter = 0;
    bool _useShader = false;

    sf::RectangleShape _laser;

    bool collidesWith(sf::FloatRect& hitBox) const;
};

#endif 