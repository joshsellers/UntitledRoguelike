#ifndef _IMPACT_EXPLOSION_H
#define _IMPACT_EXPLOSION_H

#include "Entity.h"

class ImpactExplosion : public Entity {
public:
    ImpactExplosion(sf::Vector2f pos, unsigned int layerCount, unsigned int propogationRate = 2, bool onlyDamagePlayer = true, bool onlyDamageEnemies = false);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:
    const unsigned int _layerCount;
    const unsigned int _propogationRate;

    const bool _onlyDamagePlayer;
    const bool _onlyDamageEnemies;

    int _explosionLayersSpawned = 0;
    long long _lastLayerSpawnTime = 0;
};

#endif