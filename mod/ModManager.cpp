#include "ModManager.h"
#include "../core/Util.h"
#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include "../core/MessageManager.h"
#include "../inventory/Item.h"

void ModManager::loadAll() {
    loadFunctions();
    loadItems();
    loadProjectiles();
}

void ModManager::loadFunctions() {
    const std::string dirName = "mods/functions";
    std::vector<std::string> functionFiles;
    for (const auto& entry : std::filesystem::directory_iterator(dirName)) {
        if (splitString(splitString(entry.path().string(), "\\")[1], ".").size() != 2) continue;
        else if (splitString(splitString(entry.path().string(), "\\")[1], ".")[1] != "rf") continue;
        functionFiles.push_back(splitString(entry.path().string(), "\\")[1]);
    }

    for (std::string file : functionFiles) {
        std::string inputPath = dirName + "/" + file;
        std::ifstream in(inputPath, std::ios::binary);
        std::ostringstream ostrm;
        ostrm << in.rdbuf();
        std::string inString(ostrm.str());
        in.close();

        std::vector<int> bytecode;
        for (int i = 0; i < inString.size(); i++) {
            bytecode.push_back((int)inString.at(i));
        }

        std::string functionName = splitString(file, ".")[0];
        _functions[functionName] = bytecode;
    }
}

void ModManager::loadItems() {
    const std::string dirName = "mods/items";
    std::vector<std::string> itemFiles;
    for (const auto& entry : std::filesystem::directory_iterator(dirName)) {
        if (splitString(splitString(entry.path().string(), "\\")[1], ".").size() != 2) continue;
        else if (splitString(splitString(entry.path().string(), "\\")[1], ".")[1] != "item") continue;
        itemFiles.push_back(splitString(entry.path().string(), "\\")[1]);
    }

    for (std::string file : itemFiles) {
        std::ifstream in(dirName + "/" + file);

        if (in.good()) {
            try {
                loadItem(in);
            } catch (std::exception ex) {
                MessageManager::displayMessage("Error loading " + file + ": \n" + ex.what(), 5, ERR);
            }
        }
        in.close();
    }
}

void ModManager::loadItem(std::ifstream& in) {
    std::map<std::string, EQUIPMENT_TYPE> equipNames = {
        {"NOT_EQUIPABLE", EQUIPMENT_TYPE::NOT_EQUIPABLE},
        {"CLOTHING_HEAD", EQUIPMENT_TYPE::CLOTHING_HEAD},
        {"CLOTHING_BODY", EQUIPMENT_TYPE::CLOTHING_BODY},
        {"CLOTHING_LEGS", EQUIPMENT_TYPE::CLOTHING_LEGS},
        {"CLOTHING_FEET", EQUIPMENT_TYPE::CLOTHING_FEET},
        {"ARMOR_HEAD",    EQUIPMENT_TYPE::ARMOR_HEAD},
        {"ARMOR_BODY",    EQUIPMENT_TYPE::ARMOR_BODY},
        {"ARMOR_LEGS",    EQUIPMENT_TYPE::ARMOR_LEGS},
        {"ARMOR_FEET",    EQUIPMENT_TYPE::ARMOR_FEET},
        {"AMMO",          EQUIPMENT_TYPE::AMMO},
        {"TOOL",          EQUIPMENT_TYPE::TOOL},
        {"BOAT",          EQUIPMENT_TYPE::BOAT}
    };

    unsigned int itemId = Item::ITEMS.size() - 1;
    std::string name = "";
    sf::IntRect textureRect;
    bool isStackable = false;
    unsigned int stackLimit = 0;
    bool isConsumable = false;
    std::string description = "";
    EQUIPMENT_TYPE equipType = EQUIPMENT_TYPE::NOT_EQUIPABLE;
    int damage = 0;
    float hitBoxPos = 0;
    int hitBoxSize = 0;
    sf::Vector2f barrelPos;
    bool isGun = false;
    int value = 0;
    bool isBuyable = false;
    std::string functionName = "NONE";
    int magazineSize = 0;
    bool isAutomatic = false;
    unsigned int fireRateMillis = 0;
    unsigned int reloadTimeMillis = 0;
    unsigned int unlockWaveNumber = 0;
    bool isAnimated = false;
    unsigned int ticksPerFrame = 0;
    unsigned int frameCount = 0;

    std::string line;
    while (getline(in, line)) {
        
    }

    Item item(itemId, name, textureRect, isStackable, stackLimit, isConsumable,
        description, equipType, damage, hitBoxPos, hitBoxSize,
        barrelPos,
        isGun,
        value,
        isBuyable,
        [](Entity* parent) { /*return Interpreter::interpret(ModManger::getFunction(functionName))*/return false; },
        magazineSize,
        isAutomatic,
        fireRateMillis,
        reloadTimeMillis, true, functionName);

    //Item::ITEMS.push_back(&item);
    _loadedItems.push_back(item);
    Item::ITEMS.push_back(&_loadedItems.at(_loadedItems.size() - 1));
        
    Item::ITEM_UNLOCK_WAVE_NUMBERS[itemId] = unlockWaveNumber;

    if (isAnimated) {
        Item::ANIMATION_CONFIGS[itemId] = WeaponAnimationConfig{ itemId, ticksPerFrame, frameCount };
    }
}

void ModManager::loadProjectiles() {
}

std::vector<int> ModManager::getFunction(std::string functionName) {
    if (_functions.find(functionName) == _functions.end()) {
        std::cout << "No function named: " + functionName << std::endl;
        return { 0x01, 0xFF, 0x00, 0x00, 0x00, 0x1F };
    }
    return _functions.at(functionName);
}