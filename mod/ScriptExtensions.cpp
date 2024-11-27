#include "ScriptExtensions.h"
#include "../core/MessageManager.h"
#include "../inventory/abilities/AbilityManager.h"
#include "../inventory/abilities/Ability.h"

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
    }
};