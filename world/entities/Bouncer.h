#ifndef _BOUNCER_H
#define _BOUNCER_H

#include "Entity.h"

class Bouncer : public Entity {
public:
    Bouncer(ENTITY_SAVE_ID saveId, sf::Vector2f pos, float baseSpeed, const int spriteWidth, const int spriteHeight, const float angle);

    void update();

    std::string getSaveData() const;

    friend class SaveManager;
protected:
    virtual void preupdate() = 0;
    float _angle;
private:

};

#endif