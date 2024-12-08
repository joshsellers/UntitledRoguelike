#ifndef _COMPILER_H
#define _COMPILER_H

#include <string>
#include <vector>
#include "../Instructions.h"

constexpr int MAX_VARIABLES = 256;
class Compiler {
public:
    static std::string compile(std::string assembly);
private:
    static std::string process(std::vector<int> bytecode);

    static inline std::vector<std::string> _variables;
    static bool findVar(std::string varName, int& varIndex);

    static inline std::vector<std::string> _strVariables;
    static bool findStrVar(std::string varName, int& varIndex, bool assignIfNotFound = true);
};

#endif
