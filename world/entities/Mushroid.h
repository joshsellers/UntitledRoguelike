#ifndef _MUSHROID_H
#define _MUSHROID_H

#include "Entity.h"

class Mushroid : public Entity {
public:
    Mushroid(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    virtual std::string getSaveData() const;

    friend class SaveManager;

protected:
    virtual void damage(int damage);
private:
    bool _isAggro = false;
    bool _isTransforming = false;

    sf::Sprite _wavesSprite;

    unsigned int _animCounter = 0;
};

#endif