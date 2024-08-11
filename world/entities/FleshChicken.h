#ifndef _FLESH_CHICKEN_H
#define _FLESH_CHICKEN_H

#include "Entity.h"

class FleshChicken : public Entity {
public:
    FleshChicken(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
private:
    sf::Sprite _wavesSprite;
};

#endif

