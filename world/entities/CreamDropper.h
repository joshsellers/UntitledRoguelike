#ifndef _CREAM_DROPPER_H
#define _CREAM_DROPPER_H

#include "Entity.h"
#include "CreamBoss.h"
#include <queue>

class CreamDropper : public Entity {
public:
    CreamDropper(sf::Vector2f pos, CreamBoss* creamBoss, const unsigned int dropCount);

    void update();

    void draw(sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

private:
    CreamBoss* _creamBoss;
    
    const unsigned int _dropCount;
    unsigned int _droppedSoFar = 0;

    const long long _dropRate = 2000LL;
    std::queue<long long> _dropTimes;

    int _animCounter = 0;
    int _frameNumber = 0;
    int _lastFrameNumber = -1;
};

#endif