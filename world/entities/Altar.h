#ifndef _ALTAR_H
#define _ALTAR_H

#include "Entity.h"

class Altar : public Entity {
public:
    Altar(sf::Vector2f pos, bool isActivated, std::shared_ptr<sf::Texture> spriteSheet);

    virtual void update();
    virtual void draw(sf::RenderTexture& surface);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
private:
    bool _isActivated = false;
    bool _isActivating = false;

    int _animationCounter = 0;
    int _activationStep = 0;

    void onActivation();
};

#endif