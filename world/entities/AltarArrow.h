#ifndef _ALTAR_ARROW_H
#define _ALTAR_ARROW_H

#include "../World.h"

class AltarArrow {
public:
    static void update();
    static void draw(sf::RenderTexture& surface);

    static void altarSpawned(sf::Vector2f pos);
    static void altarActivated();

    static void setWorld(World* world);
    static void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:
    static inline sf::Sprite _sprite;

    static inline World* _world;

    static inline bool _altarSpawned = false;
    static inline bool _isVisible = false;

    static inline sf::Vector2f _altarPos;

    static void checkIfAltarDespawned();
    static inline long long _lastAltarCheckTime = 0LL;
};

#endif