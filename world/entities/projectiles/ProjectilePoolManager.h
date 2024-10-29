#ifndef _PROJECTILE_POOL_MANAGER_H
#define _PROJECTILE_POOL_MANAGER_H

#include "Projectile.h"

constexpr int MAX_PROJECTILES = 20000;

class ProjectilePoolManager {
public:
    static void init();
    static void shutdown();

    static void update();

    static void draw(sf::RenderTexture& surface);

    static void addProjectile(sf::Vector2f pos, Entity* parent, float directionAngle, float velocity, const ProjectileData data, 
        bool onlyHitPlayer = false, int damageBoost = 0, bool addParentVelocity = true, int passThroughCount = 1);

    static void removeAll();

    static void setDebug(bool debug);
private:
    static inline Projectile* _pool[MAX_PROJECTILES];

    static inline bool _debug = false;
};

#endif