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

    // Push to this once an item is created, then push address of just created item off _loadedItems and into Item::ITEMS
    // Might need to add bool parameter to Item that, when false, does not push the item onto Item::ITEMS
    // Will probably also need a bool isCustomItem or something that tells it whether or not to use the lambda use function
    // In the member use function, check that bool and call interpreter
    // This means we'd need to store the function name too
    // Strings are pointers too though so that's a problem 
    // Seems like it might be fine? We'll see
    static inline std::vector<Item> _loadedItems;

    static std::vector<std::string> tokenize(std::string line);
    static std::vector<std::string> splitOperators(std::string bareToken);
    static inline const std::vector<std::string> _operators = { "!=", "==", ">=", "<=", "+", "-", "*", "/", "=", ";", ",", ":", "(", ")", "{", "}", ">", "<", "!", "%" };
};

#endif