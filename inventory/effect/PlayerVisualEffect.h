#ifndef _PLAYER_VISUAL_EFFECT_H
#define _PLAYER_VISUAL_EFFECT_H

#include <string>
#include <SFML/System/Vector2.hpp>

struct PlayerVisualEffect {
    PlayerVisualEffect(std::string name, unsigned int id, sf::Vector2i texturePos) {
        this->name = name;
        this->id = id;
        this->texturePos = texturePos;
    }

    std::string name;
    unsigned int id;
    sf::Vector2i texturePos;
};

#endif