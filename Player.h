#ifndef _PLAYER_H
#define _PLAYER_H

#include <SFML/Graphics.hpp>

class Player {
public:
    Player(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void move(int xa, int ya);

    sf::Vector2f getPosition();

private:
    sf::Vector2f _pos;

    sf::CircleShape tempSprite = sf::CircleShape(5);
};

#endif