#ifndef _BEE_FAMILIAR_H
#define _BEE_FAMILIAR_H

#include "Entity.h"

class BeeFamiliar : public Entity {
public:
    BeeFamiliar(sf::Vector2f pos, float orbiterAngle, float orbiterDistance);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    virtual std::string getSaveData() const;

private:
    const float _orbiterAngle;
    const float _orbiterDistance;

    bool _hasSelectedEnemy = false;
    std::shared_ptr<Entity> _enemyTarget = nullptr;

    long long _lastDamageTime = 0LL;

    bool searchForEnemies();
};

#endif