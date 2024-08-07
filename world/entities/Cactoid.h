#ifndef _CACTOID_H
#define _CACTOID_H

#include "Entity.h"

class Cactoid : public Entity {
public:
    Cactoid(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    virtual std::string getSaveData() const;

    friend class SaveManager;

protected:
    virtual void damage(int damage);
private:
    bool _isAggro = false;

    sf::Sprite _wavesSprite;
};

#endif