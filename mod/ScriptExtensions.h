#ifndef _SCRIPT_EXTENSIONS_H
#define _SCRIPT_EXTENSIONS_H

#include <functional>
#include <string>
#include <map>
#include "../world/entities/Entity.h"
#include "Interpreter.h"

class ScriptExtensions {
public:
    static bool execute(const std::string functionName, Entity* entity, Interpreter* interpreter);
private:
    static std::map<const std::string, const std::function<bool(Entity*, Interpreter*)>> _functions;
};

#endif