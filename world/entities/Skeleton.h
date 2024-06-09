#ifndef _SKELETON_H
#define _SKELETON_H

#include "Entity.h"

class Skeleton : public Entity {
public:
    Skeleton(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
private:
    sf::Sprite _wavesSprite;
};

#endif

