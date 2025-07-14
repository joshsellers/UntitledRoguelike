#ifndef _ABILITY_H
#define _ABILITY_H

#include <vector>
#include <string>
#include <map>
#include <functional>
#include "../../world/entities/Player.h"

class Ability {
public:
    static const Ability DAMAGE_AURA;
    static const Ability HEALILNG_MIST;
    static const Ability THIRD_EYE;
    static const Ability LIGHTNING;
    static const Ability STOPWATCH;
    static const Ability ALTAR_CHANCE;
    static const Ability BETTER_RELOAD;
    static const Ability EXPLOSIVE_ROUNDS;
    static const Ability CRIT_CHANCE;
    static const Ability OCTOPUS;
    static const Ability ORDER_FORM;
    static const Ability SPLITTING_PROJECTILES;
    static const Ability FEAR;
    static const Ability CASSIDYS_HEAD;
    static const Ability BLESSING;
    static const Ability DEBIT_CARD;
    static const Ability COUPON;
    static const Ability BOUNCING_PROJECTILES;
    static const Ability PERMANENT_ARMOR;
    static const Ability AIR_STRIKE;
    static const Ability BIG_BULLETS;
    static const Ability CONTACT_DAMAGE;

    static std::vector<Ability*> ABILITIES;

    Ability(const unsigned int id, const std::string name, std::map<std::string, float> parameters,
        const std::function<void(Player*, Ability*)> update, const std::function<void(Player*, Ability*, sf::RenderTexture&)> draw);

    const unsigned int getId() const;
    const std::string getName() const;

    float getParameter(std::string key) const;
    void setParameter(std::string key, float value);
    std::map<std::string, float>& getParameters();

    float getParameterDefault(std::string key) const;
    const std::map<std::string, float> getParametersDefaults() const;

    bool playerHasAbility() const;
    void givePlayerAbility();

    void reset();

    void update(Player* player, Ability* ability);
    void draw(Player* player, Ability* ability, sf::RenderTexture& surface);

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

private:
    static void fireTargetedProjectile(float angle, const ProjectileData projData, sf::Vector2f projSpawnPoint, Player* player, int damageBoost = 0, bool addParentVelocity = false);

    const unsigned int _id;
    const std::string _name;

    std::map<std::string, float> _parameters;
    const std::map<std::string, float> _parameterDefaults;

    bool _playerHasAbility = false;

    const std::function<void(Player*, Ability*) > _update;
    const std::function<void(Player*, Ability*, sf::RenderTexture&)> _draw;

    sf::Sprite _sprite;

    // Some of these are specific to certain abilities
    // DAMAGE_AURA
    long long _lastAttackTimeMillis = 0LL;

    std::vector<std::string> _hitEntities;
    //
    
    // HEALING_MIST
    long long _lastHealTimeMillis = 0LL;
    //

    // THIRD_EYE & LIGHTNING & OCTOPUS
    long long _lastFireTimeMillis = 0LL;
    //
};

#endif