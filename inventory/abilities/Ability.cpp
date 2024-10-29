#include "Ability.h"
#include "../../world/World.h"
#include "../../world/entities/projectiles/ProjectilePoolManager.h"
#include "../../world/entities/Lightning.h"

const Ability Ability::DAMAGE_AURA(0, "Bad Vibes", 
    { {"damage", 3.f}, {"radius", 64.f}, {"damagefreq", 10.f}, {"anim", 0.f}, {"expansion rate", 1.f} },
    [](Player* player, Ability* ability) {
        if (currentTimeMillis() - ability->_lastAttackTimeMillis >= ability->getParameter("damagefreq")) {
            float maxRadius = ability->getParameter("radius");
            float radius = ((int)(ability->getParameter("anim") / 1) % ((int)maxRadius));

            if (radius == 0) {
                ability->setParameter("anim", 0);
                ability->_hitEntities.clear();
            } else if (radius < 5) {
                ability->_hitEntities.clear();
            }

            bool attacked = false;
            for (auto& enemy : player->getWorld()->getEnemies()) {
                if (enemy->isActive()) {
                    bool alreadyHit = false;
                    for (const auto& uid : ability->_hitEntities) {
                        if (uid == enemy->getUID()) {
                            alreadyHit = true;
                            break;
                        }
                    }
                    if (alreadyHit) continue;

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
                            ability->_hitEntities.push_back(enemy->getUID());
                            break;
                        }
                    }
                }
            }

            if (attacked) ability->_lastAttackTimeMillis = currentTimeMillis();
        }

        ability->setParameter("anim", ability->getParameter("anim") + ability->getParameter("expansion rate"));
    },

    [](Player* player, Ability* ability, sf::RenderTexture& surface) {
        float maxRadius = ability->getParameter("radius");

        sf::Vector2f playerCenter = player->getPosition();
        playerCenter.x += (float)TILE_SIZE / 2;
        playerCenter.y += (float)TILE_SIZE;

        float pulseRad = ((int)(ability->getParameter("anim") / 1) % ((int)maxRadius));

        sf::CircleShape pulse;
        pulse.setRadius(pulseRad);
        pulse.setFillColor(sf::Color(0xFF000000));
        pulse.setOutlineColor(sf::Color(0xFF000033));
        pulse.setOutlineThickness(2);
        pulse.setOrigin(pulseRad, pulseRad);
        pulse.setPosition(playerCenter);

        surface.draw(pulse);
    }
);

const Ability Ability::HEALILNG_MIST(1, "Healthy Stench",
    { {"amt", 5.f}, {"freq", 5000.f}, {"duration", 2.f * 60.f * 1000.f}, {"elapsed", 0.f} },
    [](Player* player, Ability* ability) {
        if (currentTimeMillis() - ability->_lastHealTimeMillis >= ability->getParameter("freq")) {
            player->heal(player->getMaxHitPoints() * (ability->getParameter("amt") / 100.f));
            ability->_lastHealTimeMillis = currentTimeMillis();
            ability->setParameter("elapsed", ability->getParameter("elapsed") + ability->getParameter("freq"));
            if (ability->getParameter("elapsed") >= ability->getParameter("duration")) {
                ability->reset();
            }
        }
    },

    [](Player* player, Ability* ability, sf::RenderTexture& surface) {}
);

const Ability Ability::THIRD_EYE(2, "Third Eye",
    { {"damage", 1.f}, {"fire rate", 500.f} },
    [](Player* player, Ability* ability) {
        const float fireRate = ability->getParameter("fire rate");
        if (!player->isDodging() && !player->isSwimming() && !player->isInBoat() && currentTimeMillis() - ability->_lastFireTimeMillis >= fireRate) {
            for (const auto& enemy : player->getWorld()->getEnemies()) {
                if (enemy->isInitiallyDocile() && !enemy->isHostile()) continue;
                constexpr float maxDistSquared = 300.f * 300.f;
                float distSquared = std::pow(player->getPosition().x - enemy->getPosition().x, 2) + std::pow(player->getPosition().y - enemy->getPosition().y, 2);

                if (distSquared <= maxDistSquared) {
                    MOVING_DIRECTION plFacingDir = player->getFacingDir();
                    float fireAngle = 0.f;
                    float xOffset = 0.f;
                    float yOffset = 0.f;
                    switch (plFacingDir) {
                        case UP:
                            fireAngle = 270.f;
                            yOffset = -8.f;
                            break;
                        case DOWN:
                            fireAngle = 90.f;
                            break;
                        case LEFT:
                            fireAngle = 180.f;
                            xOffset = -6.f;
                            yOffset = -2.f;
                            break;
                        case RIGHT:
                            fireAngle = 0.f;
                            xOffset = 6.f;
                            yOffset = -2.f;
                            break;
                    }

                    sf::Vector2f spawnPos;
                    spawnPos.x = player->getPosition().x + xOffset;
                    spawnPos.y = player->getPosition().y + (float)TILE_SIZE / 2 + yOffset;

                    fireTargetedProjectile(degToRads(fireAngle), ProjectileDataManager::getData("_PROJECTILE_THIRD_EYE"), spawnPos, player, ability->getParameter("damage"));
                    ability->_lastFireTimeMillis = currentTimeMillis();
                    break;
                }
            }
        }
    },

    [](Player* player, Ability* ability, sf::RenderTexture& surface) {}
);

const Ability Ability::LIGHTNING(3, "Lightning",
    { {"damage", 25.f}, {"strike rate", 1000.f}, {"strike chance", 50}},
    [](Player* player, Ability* ability) {
        const int strikeChance = (int)ability->getParameter("strike chance") - 1;
        const float strikeRate = ability->getParameter("strike rate");
        if (currentTimeMillis() - ability->_lastFireTimeMillis >= strikeRate && player->getWorld()->getEnemyCount() > 0 && randomInt(0, strikeChance) == 0) {
            const sf::Vector2f playerPos = player->getPosition();
            sf::Vector2f pos(playerPos.x + randomInt(-WIDTH / 2, WIDTH / 2), playerPos.y + randomInt(-HEIGHT / 2, HEIGHT / 2));

            std::shared_ptr<Lightning> lightning = std::shared_ptr<Lightning>(new Lightning(pos, ability->getParameter("damage"), 25));
            lightning->setWorld(player->getWorld());
            lightning->loadSprite(player->getWorld()->getSpriteSheet());
            player->getWorld()->addEntity(lightning);

            ability->_lastFireTimeMillis = currentTimeMillis();
        }
    },

    [](Player* player, Ability* ability, sf::RenderTexture& surface) {}
);

std::vector<Ability*> Ability::ABILITIES;

Ability::Ability(const unsigned int id, const std::string name, std::map<std::string, float> parameters,
    const std::function<void(Player*, Ability*)> update, const std::function<void(Player*, Ability*, sf::RenderTexture&)> draw) :
_id(id), _name(name), _parameters(parameters), _update(update), _draw(draw), _parameterDefaults(_parameters)
{
    ABILITIES.push_back(this);
}

void Ability::fireTargetedProjectile(float angle, const ProjectileData projData, sf::Vector2f projSpawnPoint, Player* player, int damageBoost, bool addParentVelocity) {
    const sf::Vector2f centerPoint(projSpawnPoint.x, projSpawnPoint.y);
    sf::Vector2f spawnPos(centerPoint.x, centerPoint.y);

    ProjectilePoolManager::addProjectile(spawnPos, player, angle, projData.baseVelocity, projData,
        false, damageBoost, addParentVelocity);
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
