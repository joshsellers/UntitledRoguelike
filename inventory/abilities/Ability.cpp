#include "Ability.h"
#include "../../world/World.h"

const Ability Ability::DAMAGE_AURA(0, "Damage Aura", 
    { {"damage", 1.f}, {"radius", 64.f}, {"damagefreq", 1000.f} },
    [](Player* player, Ability* ability) {
        bool attacked = false;
        for (auto& enemy : player->getWorld()->getEnemies()) {
            if (enemy->isActive()) {
                if (currentTimeMillis() - ability->_lastAttackTimeMillis >= ability->getParameter("damagefreq")) {
                    sf::Vector2f playerCenter = player->getPosition();
                    playerCenter.x += (float)TILE_SIZE / 2;
                    playerCenter.y += (float)TILE_SIZE;
    
                    const sf::FloatRect enemyHitBox = enemy->getHitBox();
                    const std::vector<sf::Vector2f> corners = {
                        {enemyHitBox.left, enemyHitBox.top},
                        {enemyHitBox.left + enemyHitBox.width, enemyHitBox.top},
                        {enemyHitBox.left, enemyHitBox.top + enemyHitBox.height},
                        {enemyHitBox.left + enemyHitBox.width, enemyHitBox.top + enemyHitBox.height}
                    };

                    for (const auto& corner : corners) {
                        float dx = std::abs(corner.x - playerCenter.x);
                        float dy = std::abs(corner.y - playerCenter.y);
                        float radius = ability->getParameter("radius");

                        bool hit = false;

                        if (dx > radius || dy > radius) continue;
                        else if (dx + dy <= radius) {
                            hit = true;
                        } else if ((dx * dx) + (dy * dy) <= radius * radius) {
                            hit = true;
                        } else continue;

                        if (hit) {
                            enemy->takeDamage(ability->getParameter("damage"));
                            attacked = true;
                            break;
                        }
                    }
                }
            }
        }

        if (attacked) ability->_lastAttackTimeMillis = currentTimeMillis();
    },

    [](Player* player, Ability* ability, sf::RenderTexture& surface) {
        sf::CircleShape circle;
        circle.setRadius(ability->getParameter("radius"));
        circle.setFillColor(sf::Color(0xFF000011));
        sf::Vector2f playerCenter = player->getPosition();
        playerCenter.x += (float)TILE_SIZE / 2;
        playerCenter.y += (float)TILE_SIZE;
        circle.setPosition(sf::Vector2f(playerCenter.x - circle.getRadius(), playerCenter.y - circle.getRadius()));

        surface.draw(circle);
    }
);

std::vector<Ability*> Ability::ABILITIES;

Ability::Ability(const unsigned int id, const std::string name, std::map<std::string, float> parameters,
    const std::function<void(Player*, Ability*)> update, const std::function<void(Player*, Ability*, sf::RenderTexture&)> draw) :
_id(id), _name(name), _parameters(parameters), _update(update), _draw(draw), _parameterDefaults(_parameters)
{
    ABILITIES.push_back(this);
}

const unsigned int Ability::getId() const {
    return _id;
}

const std::string Ability::getName() const {
    return _name;
}

float Ability::getParameter(std::string key) const {
    return _parameters.at(key);
}

void Ability::setParameter(std::string key, float value) {
    _parameters.at(key) = value;
}

std::map<std::string, float>& Ability::getParameters() {
    return _parameters;
}

float Ability::getParameterDefault(std::string key) const {
    return _parameterDefaults.at(key);
}

const std::map<std::string, float> Ability::getParametersDefaults() const {
    return _parameterDefaults;
}

bool Ability::playerHasAbility() const {
    return _playerHasAbility;
}

void Ability::givePlayerAbility() {
    _playerHasAbility = true;
}

void Ability::reset() {
    _playerHasAbility = false;
    for (const auto& parameter : _parameters) {
        setParameter(parameter.first, getParameterDefault(parameter.first));
    }
}

void Ability::update(Player* player, Ability* ability) {
    _update(player, ability);
}

void Ability::draw(Player* player, Ability* ability, sf::RenderTexture& surface) {
    _draw(player, ability, surface);
}
