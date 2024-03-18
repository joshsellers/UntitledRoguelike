#ifndef _ORBITER_H
#define _ORBITER_H

#include "Entity.h"
#include <memory>

class Orbiter : public Entity {
public:
    Orbiter(sf::Vector2f pos, float distance, float speed, Entity* parent);

    virtual void update();

    virtual void draw(sf::RenderTexture& surface) = 0;

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet) = 0;

    float getAngle();
    float getDistance();
    float getSpeed();

    Entity* getParent();
protected:
    float _angle = 0;
    float _distance;
    float _speed;

    virtual void subUpdate() = 0;

    void fireTargetedProjectile(sf::Vector2f targetPos, const ProjectileData projData, std::string soundName);
private:
    Entity* _parent;
};

#endif

