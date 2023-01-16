#ifndef _PROJECTILE_H
#define _PROJECTILE_H

#include "Entity.h"

class Projectile : public Entity {
public:
    Projectile(sf::Vector2f pos, float directionAngle, float velocity, const ProjectileData data);

    void update();
    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

private:
    const sf::Vector2f _originalPos;

    float _directionAngle;
    float _velocity;
    sf::Vector2f _velocityComponents;

    const unsigned int _itemId;

    float _currentTime = 0;

    const ProjectileData _data;
};

#endif 
