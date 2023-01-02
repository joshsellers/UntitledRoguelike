#ifndef _ENTITY_H
#define _ENTITY_H

#include <SFML/Graphics.hpp>

constexpr int SPRITE_SHEET_SHIFT = 4;
constexpr int TILE_SIZE = 16;

class Entity {
public:
    Entity(sf::Vector2f pos, float baseSpeed, const int spriteWidth, const int spriteHeight, const bool isProp);

    virtual void update() = 0;

    virtual void draw(sf::RenderTexture& surface) = 0;

    virtual void move(float xa, float ya);
    bool isMoving() const;

    sf::Vector2f getPosition() const;

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet) = 0;

    bool isProp() const;

    sf::Sprite getSprite() const;

    bool displayBottom() const;

    sf::Vector2i getSpriteSize() const;

protected:
    const int _spriteWidth, _spriteHeight;

    bool _displayBottom = false;

    sf::Sprite _sprite;

    float _baseSpeed = 0;

    sf::Vector2f _pos;
    int _numSteps = 0;
    int _animSpeed = 3;

    bool _isMoving = false;
    unsigned int _movingDir = 1;

    const bool _isProp;

private:

};
#endif // !_ENTITY_H
