#ifndef _FUNGUY_H
#define _FUNGUY_H

#include "Entity.h"

class Funguy : public Entity {
public:
    Funguy(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
private:
    sf::Sprite _wavesSprite;
};

#endif