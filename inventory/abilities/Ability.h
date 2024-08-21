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

private:
    const unsigned int _id;
    const std::string _name;

    std::map<std::string, float> _parameters;
    const std::map<std::string, float> _parameterDefaults;

    bool _playerHasAbility = false;

    const std::function<void(Player*, Ability*) > _update;
    const std::function<void(Player*, Ability*, sf::RenderTexture&)> _draw;

    long long _lastAttackTimeMillis = 0LL;
};

#endif