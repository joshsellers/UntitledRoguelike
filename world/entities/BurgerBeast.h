#ifndef _BURGER_BEAST_H
#define _BURGER_BEAST_H

#include "Entity.h"

class BurgerBeast : public Entity {
public:
    BurgerBeast(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
private:
    sf::Sprite _wavesSprite;
};

#endif