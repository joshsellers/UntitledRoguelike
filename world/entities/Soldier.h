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

    sf::Sprite _clothingHeadSprite;
    sf::Sprite _clothingBodySprite;
    sf::Sprite _clothingLegsSprite;
    sf::Sprite _clothingFeetSprite;

    int _equippedApparel[4] = {
        NOTHING_EQUIPPED, NOTHING_EQUIPPED,
        NOTHING_EQUIPPED, NOTHING_EQUIPPED
    };
    void drawApparel(sf::Sprite& sprite, EQUIPMENT_TYPE equipType, sf::RenderTexture& surface);
};

#endif