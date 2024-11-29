#ifndef _FALLING_NUKE_H
#define _FALLING_NUKE_H

#include "Entity.h"
#include "FallingObjectShadow.h"

class FallingNuke : public Entity {
public:
    FallingNuke(sf::Vector2f playerPos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    virtual void setWorld(World* world);
private:
    sf::Sprite _dropSprite;

    sf::Vector2f _impactPos;

    bool _landed = false;
    int _explosionLayersSpawned = 0;
    long long _lastLayerSpawnTime = 0;

    std::shared_ptr<FallingObjectShadow> _shadow = nullptr;
};

#endif