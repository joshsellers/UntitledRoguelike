#include "ScriptExtensions.h"
#include "../core/MessageManager.h"
#include "../inventory/abilities/AbilityManager.h"
#include "../inventory/abilities/Ability.h"
#include "../world/World.h"
#include "../world/entities/orbiters/Orbiter.h"

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
    }
};