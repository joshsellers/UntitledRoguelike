#ifndef _CYCLOPS_H
#define _CYCLOPS_H

#include "Entity.h"
class Cyclops : public Entity {
public:
    Cyclops(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
private:
    sf::Sprite _wavesSprite;

    float _desiredDist = 512.f;
};

#endif
