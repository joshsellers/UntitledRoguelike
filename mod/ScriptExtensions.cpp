#include "ScriptExtensions.h"
#include "../core/MessageManager.h"
#include "../inventory/abilities/AbilityManager.h"
#include "../inventory/abilities/Ability.h"
#include "../world/World.h"
#include "../world/entities/orbiters/Orbiter.h"
#include "../world/entities/Scythe.h"
#include "../core/Tutorial.h"
#include "../world/entities/LaserBeam.h"

bool ScriptExtensions::execute(const std::string functionName, Entity* entity, Interpreter* interpreter) {
    if (_functions.find(functionName) != _functions.end()) {
        return _functions.at(functionName)(entity, interpreter);
    }

    MessageManager::displayMessage("No built-in function named \"" + functionName + "\"", 5, WARN);
    return false;
}

std::map<const std::string, const std::function<bool(Entity*, Interpreter*)>> ScriptExtensions::_functions = {
    {
        "increaseCritChance",
        [](Entity* parent, Interpreter* interpreter) {
            const float increaseAmount = interpreter->pop();
            const float currentChance = AbilityManager::getParameter(Ability::CRIT_CHANCE.getId(), "chance");
            AbilityManager::setParameter(Ability::CRIT_CHANCE.getId(), "chance", currentChance + increaseAmount);
            return false;
        }
    },

    {
        "increaseOrbiterDist",
        [](Entity* parent, Interpreter* interpreter) {
            constexpr float increaseAmount = 8.f;
            for (const auto& entity : parent->getWorld()->getEntities()) {
                if (entity->isOrbiter()) {
                    Orbiter* orbiter = dynamic_cast<Orbiter*>(entity.get());
                    if (orbiter->getParent()->getSaveId() == PLAYER) {
                        orbiter->setDistance(orbiter->getDistance() + increaseAmount);
                    }
                }
            }

            return true;
        }
    },

    {
        "addOrbiter",
        [](Entity* parent, Interpreter* interpreter) {
            const std::string orbiterName = interpreter->strPop();
            int orbiterId = -1;
            for (int i = 0; i < OrbiterType::ORBITER_TYPES.size(); i++) {
                if (OrbiterType::ORBITER_TYPES.at(i)->getName() == orbiterName) {
                    orbiterId = i;
                    break;
                }
            }

            if (orbiterId == -1) {
                MessageManager::displayMessage("No orbiter named \"" + orbiterName + "\"", 5, ERR);
                return false;
            }

            const auto& orbiter = std::shared_ptr<Orbiter>(new Orbiter(90.f, orbiterId, parent));
            orbiter->setWorld(parent->getWorld());
            orbiter->loadSprite(parent->getWorld()->getSpriteSheet());
            parent->getWorld()->addEntity(orbiter);

            return true;
        }
    },

    {
        "spawnScythe",
        [](Entity* parent, Interpreter* interpreter) {
            const auto& scythe = std::shared_ptr<Scythe>(new Scythe({
                parent->getWorld()->getPlayer()->getPosition().x + (float)TILE_SIZE / 2.f, parent->getWorld()->getPlayer()->getPosition().y + TILE_SIZE
            }));
            scythe->setWorld(parent->getWorld());
            scythe->loadSprite(parent->getWorld()->getSpriteSheet());
            parent->getWorld()->addEntity(scythe);
            return true;
        }
    },

    {
        "completeTutorialStep",
        [](Entity* parent, Interpreter* interpreter) {
            const int step = interpreter->pop();
            if (!Tutorial::isCompleted()) {
                Tutorial::completeStep((TUTORIAL_STEP)step);
            }
            return true;
        }
    },

    {
        "fireLaser",
        [](Entity* parent, Interpreter* interpreter) {
            const bool useTexture = interpreter->pop();
            const unsigned int alpha = interpreter->pop();
            const unsigned int blue = interpreter->pop();
            const unsigned int green = interpreter->pop();
            const unsigned int red = interpreter->pop();
            const int length = interpreter->pop();
            const int widthParameter = interpreter->pop();

            const int damage = Item::ITEMS[parent->getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL)]->getDamage() * parent->getDamageMultiplier();
            const int width = AbilityManager::playerHasAbility(Ability::BIG_BULLETS.getId()) ? widthParameter * 4 : widthParameter;
            const unsigned int color = (((((red << 8) + green) << 8) + blue) << 8) + alpha;
            const auto& laser = std::shared_ptr<LaserBeam>(
                new LaserBeam(
                    parent, parent->getTargetPos() + parent->getVelocity(), color, width, length, damage, 
                    {parent->getVelocity().x, parent->getVelocity().y}, false, 16LL, true
                ));
            laser->setWorld(parent->getWorld());
            if (useTexture) {
                laser->setTextureRect(
                    sf::IntRect(
                        2128, 2017, 2 * TILE_SIZE, 2 * TILE_SIZE
                    ), 2, 1, true
                );
            }
            parent->getWorld()->addEntity(laser);

            parent->decrementMagazine();
            return false;
        }
    }
};