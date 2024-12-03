#ifndef _MEGA_BOMB_BOY_H
#define _MEGA_BOMB_BOY_H

#include "Entity.h"

class MegaBombBoy : public Entity {
public:
    MegaBombBoy(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
private:
    sf::Sprite _wavesSprite;

    void explode();
};

#endif