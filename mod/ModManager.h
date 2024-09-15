#ifndef _MOD_MANAGER_H
#define _MOD_MANAGER_H

#include <map>
#include <vector>
#include <string>

class Item;

class ModManager {
public:
    static void loadAll();

    static std::vector<int> getFunction(std::string functionName);
private:
    static void loadFunctions();
    static inline std::map<std::string, std::vector<int>> _functions;

    static void loadItems();
    static void loadItem(std::ifstream& in);
    static void loadProjectiles();
    static void loadProjectile(std::ifstream& in);

    static std::vector<std::string> tokenize(std::string line);
    static std::vector<std::string> splitOperators(std::string bareToken);
    static inline const std::vector<std::string> _operators = { "!=", "==", ">=", "<=", "+", "-", "*", "/", "=", ";", ",", ":", "(", ")", "{", "}", ">", "<", "!", "%" };
};

#endif