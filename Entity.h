#ifndef _ENTITY_H
#define _ENTITY_H

#include <SFML/Graphics.hpp>

class Entity {
public:
    Entity(sf::Vector2f pos, float baseSpeed);

    virtual void update() = 0;

    virtual void draw(sf::RenderTexture& surface) = 0;

    virtual void move(float xa, float ya);
    bool isMoving() const;

    sf::Vector2f getPosition() const;

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet) = 0;

protected:
    sf::Sprite _sprite;

    float _baseSpeed = 0;

    sf::Vector2f _pos;
    int _numSteps = 0;
    int _animSpeed = 3;

    bool _isMoving = false;
    unsigned int _movingDir = 1;

private:

};
#endif // !_ENTITY_H
