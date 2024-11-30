#ifndef _SOLDIER_H
#define _SOLDIER_H

#include "Entity.h"

class Soldier : public Entity {
public:
    Soldier(sf::Vector2f pos);

    void update();
    void draw(sf::RenderTexture& surface);

    virtual void decrementMagazine();

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:
    void drawWeapon(sf::RenderTexture& surface);

    sf::Sprite _wavesSprite;
    sf::Sprite _toolSprite;

    MOVING_DIRECTION _facingDir = UP;

    long long _lastFireTime = 0;

    const unsigned int _weaponId;
    int _shotsFired = 0;
};

#endif