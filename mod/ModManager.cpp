#include "ModManager.h"
#include "../core/Util.h"
#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include "../core/MessageManager.h"
#include "../inventory/Item.h"
#include "../inventory/effect/PlayerVisualEffectManager.h"

void ModManager::loadAll() {
    const std::string dirName = "data";
    if (!std::filesystem::is_directory(dirName + "/")) {
        std::filesystem::create_directory(dirName);
        MessageManager::displayMessage("Created data directory", 5, DEBUG);
    }

    loadFunctions();
    loadItems();
    loadProjectiles();
    loadPlayerVisualEffects();
}

void ModManager::reloadFunctions() {
    loadFunctions();
}

void ModManager::loadFunctions() {
    const std::string dirName = "data/functions";
    if (!std::filesystem::is_directory(dirName + "/")) {
        std::filesystem::create_directory(dirName);
        MessageManager::displayMessage("Created data/functions directory", 5, DEBUG);
    }

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
    const std::string dirName = "data/items";
    if (!std::filesystem::is_directory(dirName + "/")) {
        std::filesystem::create_directory(dirName);
        MessageManager::displayMessage("Created data/items directory", 5, DEBUG);
    }

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

    unsigned int itemId = Item::ITEMS.size();
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
    std::string ammoItemName = "";
    bool isStartingItem = false;
    float shopChance = 100.f;
    std::vector<std::string> tags;
    bool conditionalUnlock = false;
    unsigned int rarityTier = 0;

    std::string line;
    while (getline(in, line)) {
        if (stringStartsWith(line, "#")) continue;

        const std::vector<std::string> unmergedTokens = tokenize(line);
        std::vector<std::string> tokens;
        for (int i = 0; i < unmergedTokens.size(); i++) {
            const std::string token = unmergedTokens.at(i);
            if (i < unmergedTokens.size() - 1 && token == "-" && isNumber(unmergedTokens.at(i + 1))) {
                tokens.push_back("-" + unmergedTokens.at(i + 1));
                i++;
                continue;
            }

            tokens.push_back(token);
        }

        if (tokens.size() > 2 && tokens.at(1) == "=") {
            if (tokens.at(0) == "name") {
                std::string strName = tokens.at(2);
                replaceAll(strName, "\"", "");
                name = strName;
            } else if (tokens.at(0) == "textureRect") {
                if (tokens.size() != 9) {
                    MessageManager::displayMessage("Bad texture rect data for item: \"" + name + "\"", 5, ERR);
                    continue;
                }
                sf::Vector2i coords;
                coords.x = std::stoi(tokens.at(2));
                coords.y = std::stoi(tokens.at(4));
                int width = std::stoi(tokens.at(6));
                int height = std::stoi(tokens.at(8));

                textureRect.left = coords.x;
                textureRect.top = coords.y;
                textureRect.width = width;
                textureRect.height = height;
            } else if (tokens.at(0) == "isStackable") {
                isStackable = tokens.at(2) == "1";
            } else if (tokens.at(0) == "stackLimit") {
                stackLimit = std::stoul(tokens.at(2));
            } else if (tokens.at(0) == "isConsumable") {
                isConsumable = tokens.at(2) == "1";
            } else if (tokens.at(0) == "description") {
                std::string strDesc = tokens.at(2);
                replaceAll(strDesc, "\"", "");
                replaceAll(strDesc, "\\n", "\n");
                description = strDesc;
            } else if (tokens.at(0) == "equipmentType") {
                std::string strEquipType = tokens.at(2);
                if (equipNames.find(strEquipType) == equipNames.end()) {
                    MessageManager::displayMessage("Invalid equipment type for item \"" + name + "\": \"" + strEquipType + "\"", 5, WARN);
                } else {
                    equipType = equipNames.at(strEquipType);
                }
            } else if (tokens.at(0) == "damage") {
                damage = std::stoi(tokens.at(2));
            } else if (tokens.at(0) == "hitBoxPos") {
                hitBoxPos = std::stof(tokens.at(2));
            } else if (tokens.at(0) == "hitBoxSize") {
                hitBoxSize = std::stoi(tokens.at(2));
            } else if (tokens.at(0) == "barrelPos") {
                if (tokens.size() != 5) {
                    MessageManager::displayMessage("Bad barrel pos data for item: \"" + name + "\"", 5, ERR);
                    continue;
                }
                barrelPos.x = std::stof(tokens.at(2));
                barrelPos.y = std::stof(tokens.at(4));
            } else if (tokens.at(0) == "isGun") {
                isGun = tokens.at(2) == "1";
            } else if (tokens.at(0) == "value") {
                value = std::stoi(tokens.at(2));
            } else if (tokens.at(0) == "isBuyable") {
                isBuyable = tokens.at(2) == "1";
            } else if (tokens.at(0) == "functionName") {
                std::string funcNameStr = tokens.at(2);
                replaceAll(funcNameStr, "\"", "");
                functionName = funcNameStr;
            } else if (tokens.at(0) == "magazineSize") {
                magazineSize = std::stoi(tokens.at(2));
            } else if (tokens.at(0) == "isAutomatic") {
                isAutomatic = tokens.at(2) == "1";
            } else if (tokens.at(0) == "fireRate") {
                fireRateMillis = std::stoul(tokens.at(2));
            } else if (tokens.at(0) == "reloadTime") {
                reloadTimeMillis = std::stoul(tokens.at(2));
            } else if (tokens.at(0) == "unlockWaveNumber") {
                unlockWaveNumber = std::stoul(tokens.at(2));
            } else if (tokens.at(0) == "isAnimated") {
                isAnimated = tokens.at(2) == "1";
            } else if (tokens.at(0) == "ticksPerFrame") {
                ticksPerFrame = std::stoul(tokens.at(2));
            } else if (tokens.at(0) == "frameCount") {
                frameCount = std::stoul(tokens.at(2));
            } else if (tokens.at(0) == "ammoName") {
                std::string ammoNameStr = tokens.at(2);
                replaceAll(ammoNameStr, "\"", "");
                ammoItemName = ammoNameStr;
            } else if (tokens.at(0) == "isStartingItem") {
                isStartingItem = tokens.at(2) == "1";
            } else if (tokens.at(0) == "shopChance") {
                shopChance = std::stof(tokens.at(2));
            } else if (tokens.at(0) == "tags") {
                for (int i = 2; i < tokens.size(); i++) {
                    std::string currentToken = tokens.at(i);
                    if (stringStartsWith(currentToken, "\"") || currentToken == ",") {
                        if (stringStartsWith(currentToken, "\"")) {
                            replaceAll(currentToken, "\"", "");
                            tags.push_back(currentToken);
                        }
                    } else {
                        MessageManager::displayMessage("Bad tag syntax for item \"" + name + "\": " + currentToken, 5, WARN);
                    }
                }
            } else if (tokens.at(0) == "conditionalUnlock") {
                conditionalUnlock = tokens.at(2) == "1";
            } else if (tokens.at(0) == "rarityTier") {
                rarityTier = std::stoul(tokens.at(2));
            } else {
                MessageManager::displayMessage("Unrecognized item parameter: \"" + tokens.at(0) + "\"", 5, WARN);
            }
        }
    }

    if (isGun && ammoItemName != "") {
        bool foundAmmo = false;
        for (const auto& item : Item::ITEMS) {
            if (item->getName() == ammoItemName) {
                stackLimit = item->getId();
                foundAmmo = true;
                break;
            }
        }
        if (!foundAmmo) {
            MessageManager::displayMessage("Did not find ammo item \"" + ammoItemName + "\" for item \"" + name + "\"", 5, WARN);
        }
    }

    Item::ITEMS.push_back(std::shared_ptr<Item>(new Item(itemId, name, textureRect, isStackable, stackLimit, isConsumable,
        description, equipType, damage, hitBoxPos, hitBoxSize,
        barrelPos,
        isGun,
        value,
        isBuyable,
        [](Entity* parent) { return false; },
        magazineSize,
        isAutomatic,
        fireRateMillis,
        reloadTimeMillis, isStartingItem, true, functionName, conditionalUnlock)));
        
    Item::ITEM_UNLOCK_WAVE_NUMBERS[itemId] = unlockWaveNumber;

    if (rarityTier != 0 && rarityTier < 4) {
        constexpr float tierChances[3] = { 100.f, 50.f, 5.f };
        shopChance = tierChances[rarityTier - 1];
    } else if (rarityTier >= 4) {
        MessageManager::displayMessage("Invalid rarity tier for item \"" + name + "\":" + std::to_string(rarityTier), 5, WARN);
    }
    Item::ITEM_SHOP_CHANCES[itemId] = shopChance;

    for (std::string tag : tags) {
        Item::ITEMS[itemId]->addTag(tag);
    }

    if (isAnimated) {
        Item::ANIMATION_CONFIGS[itemId] = WeaponAnimationConfig(itemId, ticksPerFrame, frameCount);
    }
}

void ModManager::loadProjectiles() {
    const std::string dirName = "data/projectiles";
    if (!std::filesystem::is_directory(dirName + "/")) {
        std::filesystem::create_directory(dirName);
        MessageManager::displayMessage("Created data/projectiles directory", 5, DEBUG);
    }

    std::vector<std::string> projFiles;
    for (const auto& entry : std::filesystem::directory_iterator(dirName)) {
        if (splitString(splitString(entry.path().string(), "\\")[1], ".").size() != 2) continue;
        else if (splitString(splitString(entry.path().string(), "\\")[1], ".")[1] != "projectile") continue;
        projFiles.push_back(splitString(entry.path().string(), "\\")[1]);
    }

    for (std::string file : projFiles) {
        std::ifstream in(dirName + "/" + file);

        if (in.good()) {
            try {
                loadProjectile(in);
            } catch (std::exception ex) {
                MessageManager::displayMessage("Error loading " + file + ": \n" + ex.what(), 5, ERR);
            }
        }
        in.close();
    }
}

void ModManager::loadProjectile(std::ifstream& in) {
    std::map<std::string, EXPLOSION_BEHAVIOR> explosionBehaviorNames = {
        {"NOT_EXPLOSIVE", EXPLOSION_BEHAVIOR::NOT_EXPLOSIVE},
        {"EXPLODE_ON_IMPACT", EXPLOSION_BEHAVIOR::EXPLODE_ON_IMPACT},
        {"EXPLODE_ON_DECAY_AND_IMPACT", EXPLOSION_BEHAVIOR::EXPLODE_ON_DECAY_AND_IMPACT}
    };

    std::string itemName = "";
    float baseVelocity = 0;
    sf::IntRect hitBox;
    bool rotateSprite = false;
    bool onlyHitEnemies = true;
    long long lifeTime = 500;
    bool isAnimated = false;
    int animationFrames = 0;
    int animationSpeed = 0;
    bool dropOnExpire = false;
    bool noCollide = false;
    bool useDamageMultiplier = true;
    bool allowPlayerProjectileEffects = true;
    EXPLOSION_BEHAVIOR explosionBehavior = EXPLOSION_BEHAVIOR::NOT_EXPLOSIVE;

    std::string line;
    while (getline(in, line)) {
        if (stringStartsWith(line, "#")) continue;

        const std::vector<std::string> tokens = tokenize(line);

        if (tokens.size() > 2 && tokens.at(1) == "=") {
            if (tokens.at(0) == "itemName") {
                std::string strName = tokens.at(2);
                replaceAll(strName, "\"", "");
                itemName = strName;
            } else if (tokens.at(0) == "hitBox") {
                if (tokens.size() != 9) {
                    MessageManager::displayMessage("Bad hitbox data for projectile: \"" + itemName + "\"", 5, ERR);
                    continue;
                }
                sf::Vector2i coords;
                coords.x = std::stoi(tokens.at(2));
                coords.y = std::stoi(tokens.at(4));
                int width = std::stoi(tokens.at(6));
                int height = std::stoi(tokens.at(8));

                hitBox.left = coords.x;
                hitBox.top = coords.y;
                hitBox.width = width;
                hitBox.height = height;
            } else if (tokens.at(0) == "velocity") {
                baseVelocity = std::stof(tokens.at(2));
            } else if (tokens.at(0) == "rotate") {
                rotateSprite = tokens.at(2) == "1";
            } else if (tokens.at(0) == "onlyHitEnemies") {
                onlyHitEnemies = tokens.at(2) == "1";
            } else if (tokens.at(0) == "lifeTime") {
                lifeTime = std::stoll(tokens.at(2));
            } else if (tokens.at(0) == "isAnimated") {
                isAnimated = tokens.at(2) == "1";
            } else if (tokens.at(0) == "frameCount") {
                animationFrames = std::stoi(tokens.at(2));
            } else if (tokens.at(0) == "animationSpeed") {
                animationSpeed = std::stoi(tokens.at(2));
            } else if (tokens.at(0) == "dropOnDespawn") {
                dropOnExpire = tokens.at(2) == "1";
            } else if (tokens.at(0) == "noCollide") {
                noCollide = tokens.at(2) == "1";
            } else if (tokens.at(0) == "useDamageMultiplier") {
                useDamageMultiplier = tokens.at(2) == "1";
            } else if (tokens.at(0) == "allowPlayerProjectileEffects") {
                allowPlayerProjectileEffects = tokens.at(2) == "1";
            } else if (tokens.at(0) == "explosionBehavior") {
                std::string strExplBehavior = tokens.at(2);
                if (explosionBehaviorNames.find(strExplBehavior) == explosionBehaviorNames.end()) {
                    MessageManager::displayMessage("Invalid explosion behavior type for projectile \"" + itemName + "\": \"" + strExplBehavior + "\"", 5, ERR);
                } else {
                    explosionBehavior = explosionBehaviorNames.at(strExplBehavior);
                }
            } else {
                MessageManager::displayMessage("Unrecognized projectile parameter: \"" + tokens.at(0) + "\"", 5, WARN);
            }
        }
    }

    unsigned int itemId = 0;

    bool foundItem = false;
    for (const auto& item : Item::ITEMS) {
        if (item->getName() == itemName) {
            itemId = item->getId();
            foundItem = true;
            break;
        }
    }

    if (!foundItem) {
        MessageManager::displayMessage("Did not find projectile item \"" + itemName + "\"", 5, ERR);
        return;
    }

    ProjectileData data(
        itemId, baseVelocity, hitBox, rotateSprite, onlyHitEnemies, lifeTime, isAnimated, animationFrames, animationSpeed, 
        dropOnExpire, noCollide, useDamageMultiplier, explosionBehavior, allowPlayerProjectileEffects
    );
}

void ModManager::loadPlayerVisualEffects() {
    const std::string dirName = "data/effects";
    if (!std::filesystem::is_directory(dirName + "/")) {
        std::filesystem::create_directory(dirName);
        MessageManager::displayMessage("Created data/effects directory", 5, DEBUG);
    }

    std::vector<std::string> projFiles;
    for (const auto& entry : std::filesystem::directory_iterator(dirName)) {
        if (splitString(splitString(entry.path().string(), "\\")[1], ".").size() != 2) continue;
        else if (splitString(splitString(entry.path().string(), "\\")[1], ".")[1] != "effect") continue;
        projFiles.push_back(splitString(entry.path().string(), "\\")[1]);
    }

    for (std::string file : projFiles) {
        std::ifstream in(dirName + "/" + file);

        if (in.good()) {
            try {
                loadPlayerVisualEffect(in);
            } catch (std::exception ex) {
                MessageManager::displayMessage("Error loading " + file + ": \n" + ex.what(), 5, ERR);
            }
        }
        in.close();
    }
}

void ModManager::loadPlayerVisualEffect(std::ifstream& in) {
    std::string name = "";
    unsigned int id = PlayerVisualEffectManager::getEffectCount();
    sf::Vector2i textPos(0, 0);

    std::string line;
    while (getline(in, line)) {
        const std::vector<std::string> tokens = tokenize(line);

        if (tokens.size() > 2 && tokens.at(1) == "=") {
            if (tokens.at(0) == "name") {
                std::string strName = tokens.at(2);
                replaceAll(strName, "\"", "");
                name = strName;
            } else if (tokens.at(0) == "textureCoords") {
                if (tokens.size() != 5) {
                    MessageManager::displayMessage("Bad texture coordinates for effect: \"" + name + "\"", 5, ERR);
                    continue;
                }
                textPos.x = std::stoi(tokens.at(2)) << SPRITE_SHEET_SHIFT;
                textPos.y = std::stoi(tokens.at(4)) << SPRITE_SHEET_SHIFT;
            } else {
                MessageManager::displayMessage("Unrecognized effect parameter: \"" + tokens.at(0) + "\"", 5, WARN);
            }
        }
    }

    PlayerVisualEffectManager::addEffectType({name, id, textPos});
}

std::vector<int> ModManager::getFunction(std::string functionName) {
    if (_functions.find(functionName) == _functions.end()) {
        MessageManager::displayMessage("No function named: " + functionName, 5, WARN);
        return { 0x01, 0xFF, 0x00, 0x00, 0x00, 0x1F };
    }
    return _functions.at(functionName);
}

std::vector<std::string> ModManager::tokenize(std::string inScript) {
    std::string script = "";
    bool replaceSpaces = false;
    for (auto symbol : inScript) {
        if (replaceSpaces && symbol == ' ') {
            script += "RPLSPC";
        } else if (replaceSpaces && symbol != '"') {
            for (int i = 4; i < _operators.size(); i++) {
                if (symbol == _operators.at(i).at(0)) {
                    script += "RPL" + std::to_string(i);
                    break;
                }
            }
        }

        if (symbol != ' ' || !replaceSpaces) {
            bool isOperator = false;
            for (int i = 4; i < _operators.size(); i++) {
                if (symbol == _operators.at(i).at(0)) {
                    isOperator = true;
                    break;
                }
            }
            if (!isOperator || !replaceSpaces) {
                script += std::string(1, symbol);
            }
        }
        if (symbol == '"') replaceSpaces = !replaceSpaces;
    }

    std::vector<std::string> tokens;

    std::vector<std::string> bareTokens = splitString(script, " ");
    for (std::string& bareToken : bareTokens) {
        replaceAll(bareToken, "RPLSPC", " ");
    }

    for (std::string bareToken : bareTokens) {
        std::vector<std::string> operatorTokens = splitOperators(bareToken);
        for (std::string token : operatorTokens) {
            if (token != "")
                tokens.push_back(token);
        }
    }

    for (std::string& token : tokens) {
        for (int i = 4; i < _operators.size(); i++) {
            replaceAll(token, "RPL" + std::to_string(i), _operators.at(i));
        }

        if (!stringStartsWith(token, "\"")) {
            replaceAll(token, "true", "1");
            replaceAll(token, "false", "0");
        }
    }

    return tokens;
}

std::vector<std::string> ModManager::splitOperators(std::string bareToken) {
    std::vector<std::string> operatorExpressionTokens;
    for (std::string operatorString : _operators) {
        if (bareToken.find(operatorString) != std::string::npos) {
            std::vector<std::string> tokens = splitString(bareToken, operatorString);
            std::vector<std::string> previousTokens = splitOperators(tokens.at(0));
            for (std::string previousToken : previousTokens) {
                operatorExpressionTokens.push_back(previousToken);
            }

            std::vector<std::string> additionalExpressions;
            for (int i = 1; i < tokens.size(); i++) {
                additionalExpressions.push_back(operatorString);
                for (std::string subToken : splitOperators(tokens.at(i))) {
                    additionalExpressions.push_back(subToken);
                }
            }

            for (std::string expression : additionalExpressions) {
                operatorExpressionTokens.push_back(expression);
            }

            return operatorExpressionTokens;
        }
    }

    operatorExpressionTokens.push_back(bareToken);
    return operatorExpressionTokens;
}