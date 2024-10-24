#include "ScriptExtensions.h"
#include "../core/MessageManager.h"

bool ScriptExtensions::execute(const std::string functionName, Entity* entity, Interpreter* interpreter) {
    if (_functions.find(functionName) != _functions.end()) {
        return _functions.at(functionName)(entity, interpreter);
    }

    MessageManager::displayMessage("No built-in function named \"" + functionName + "\"", 5, WARN);
    return false;
}

std::map<const std::string, const std::function<bool(Entity*, Interpreter*)>> ScriptExtensions::_functions = {
    {
        
    }
};