#ifndef _SAVE_MANAGER_H
#define _SAVE_MANAGER_H

#include "../world/World.h"
#include "../world/entities/orbiters/Orbiter.h"
#include "../world/entities/projectiles/Projectile.h"
#include "../world/entities/DroppedItem.h"
#include <filesystem>
#include "Versioning.h"
#include "../statistics/StatManager.h"
#include "../inventory/abilities/AbilityManager.h"
#include "../inventory/abilities/Ability.h"
#include "../world/entities/BeeFamiliar.h"
#include "../world/entities/Blinker.h"
#include "../world/MiniMapGenerator.h"

class SaveManager {
public:
    static void deleteSaveFile() {
        if (_currentSaveFileName == "NONE") return;

        std::string fileName = _saveDir + "/" + _currentSaveFileName;
        try {
            if (!std::filesystem::remove(fileName))
                MessageManager::displayMessage("Could not delete save file", 5, DEBUG);
        } catch (const std::filesystem::filesystem_error& err) {
            MessageManager::displayMessage("Could not delete save file: " + (std::string)err.what(), 5, ERR);
        }
    }

    static void saveGame(bool displaySuccessfulSaveMessage = true) {
        if (BACKUP_ENABLED) {
            std::string prevSaveFileName = _currentSaveFileName;
            _currentSaveFileName = "BACKUP_" + prevSaveFileName;
            BACKUP_ENABLED = false;
            saveGame(false);
            BACKUP_ENABLED = true;
            _currentSaveFileName = prevSaveFileName;
        }

        if (_currentSaveFileName == "NONE") {
            MessageManager::displayMessage("Saving game while _currentSaveFileName is \"NONE\"", 5, WARN);
            _currentSaveFileName += ".save";
        }

        std::string fileName = _saveDir + "/" + _currentSaveFileName;
        try {
            if (!std::filesystem::remove(fileName))
                MessageManager::displayMessage("Could not replace save file", 5, DEBUG);
        } catch (const std::filesystem::filesystem_error& err) {
            MessageManager::displayMessage("Could not replace save file: " + (std::string)err.what(), 5, ERR);
        }

        try {
            std::ofstream out(fileName);
            
            out << "GAMEVERSION:" << VERSION << std::endl;
            out << "TS:" << std::to_string(currentTimeMillis()) << std::endl;
            out << "SCORE:" << std::to_string(PLAYER_SCORE) << std::endl;
            if (HARD_MODE_ENABLED) out << "HARD:" + std::to_string(HARD_MODE_ENABLED) << std::endl;
            if (MID_GAME_PERF_BOOST) out << "PERF:" + std::to_string(MID_GAME_PERF_BOOST) << std::endl;
            saveStats(out);
            savePlayerData(out);
            saveWorldData(out);
            saveAbilities(out);
            saveEffects(out);
            saveShopData(out);
            saveEntityData(out);
            saveMiniMapData(out);

            out.close();

            if (displaySuccessfulSaveMessage) MessageManager::displayMessage("Game saved succesfully", 2);
        } catch (std::exception ex) {
            MessageManager::displayMessage("Error writing to save file: " + (std::string)ex.what(), 5, ERR);
        }

        StatManager::saveOverallStats();
    }

    static bool loadGame(std::string saveFileName) {
        bool loadedSuccessfully = true;

        std::ifstream in(_saveDir + "/" + saveFileName);
        if (!in.good()) {
            MessageManager::displayMessage("Could not find save file", 5, WARN);
            loadedSuccessfully = false;
            in.close();
            return loadedSuccessfully;
        } else {
            std::string line;
            while (getline(in, line)) {
                try {
                    std::vector<std::string> data = splitString(line, ":");
                    std::string fullHeader = data[0] + ":";
                    load(data[0], splitString(splitString(line, fullHeader)[1], ":"));
                } catch (std::exception ex) {
                    MessageManager::displayMessage("Error loading save file: " + (std::string)ex.what() + "\nLine was: \n" + line, 5, ERR);
                    loadedSuccessfully = false;
                }
            }

            if (loadedSuccessfully) {
                try {
                    if (_deferredOrbiters.size() != 0) loadDeferredOrbiters();
                    if (_deferredProjectiles.size() != 0) loadDeferredProjectiles();
                } catch (std::exception ex) {
                    MessageManager::displayMessage("Error loading deferred entities: " + (std::string)ex.what(), 5, ERR);
                    loadedSuccessfully = false;
                }
            }
        }

        in.close();

        return loadedSuccessfully;
    }

    static std::vector<std::string> getAvailableSaveFiles() {
        std::vector<std::string> saveFiles;
        for (const auto& entry : std::filesystem::directory_iterator(_saveDir)) {
            if (splitString(splitString(entry.path().string(), "\\")[1], ".").size() != 2) continue;
            else if (splitString(splitString(entry.path().string(), "\\")[1], ".")[1] != "save") continue;
            saveFiles.push_back(splitString(entry.path().string(), "\\")[1]);
        }

        return saveFiles;
    }

    static void init(World* world, ShopManager* shopManager) {
        _world = world;
        _shopManager = shopManager;

        if (!std::filesystem::is_directory("save/")) {
            std::filesystem::create_directory("save");
        }
    }

    static void setCurrentSaveFileName(std::string saveFileName) {
        _currentSaveFileName = saveFileName;
    }

    static std::string getCurrentSaveFileName() {
        return _currentSaveFileName;
    }

private:
    inline const static std::string _saveDir = "save";
    inline static std::string _currentSaveFileName = "NONE";

    inline static World* _world = nullptr;
    inline static ShopManager* _shopManager = nullptr;

    static void saveWorldData(std::ofstream& out) {
        out << "WORLD:" << std::to_string(_world->getSeed());
        out << ":" << std::to_string(_world->_cooldownStartTime);
        out << ":" << (_world->_maxEnemiesReached ? "1" : "0");
        out << ":" << (_world->_cooldownActive ? "1" : "0");
        out << ":" << std::to_string(_world->_enemySpawnCooldownTimeMilliseconds);
        out << ":" << std::to_string(_world->_maxActiveEnemies);
        out << ":" << std::to_string(_world->_enemiesSpawnedThisRound);
        out << ":" << std::to_string(_world->_waveCounter);
        out << ":" << std::to_string(_world->_currentWaveNumber);
        out << ":" << std::to_string(_world->_highestPlayerHp);
        out << std::endl;

        if (_world->_destroyedProps.size() != 0) {
            out << "DESTROYEDPROPS";
            for (auto& prop : _world->_destroyedProps) {
                out << ":" << std::to_string(prop.x) << "," << std::to_string(prop.y);
            }
            out << std::endl;
        }

        if (_world->_seenShops.size() != 0) {
            out << "SEENSHOPS";
            for (auto& shop : _world->_seenShops) {
                out << ":" << std::to_string(shop.x) << "," << std::to_string(shop.y);
            }
            out << std::endl;
        }

        if (_world->_visitedShops.size() != 0) {
            out << "VISITEDSHOPS";
            for (auto& shop : _world->_visitedShops) {
                out << ":" << (shop ? "1" : "0");
            }
            out << std::endl;
        }

        if (_world->_activatedAltars.size() != 0) {
            out << "ALTARS";
            for (auto& altar : _world->_activatedAltars) {
                out << ":" << std::to_string(altar.x) << "," << std::to_string(altar.y);
            }
            out << std::endl;
        }

        if (_world->_deadShopKeeps.size() != 0) {
            out << "DEADSHOPKEEPS";
            for (auto& seed : _world->_deadShopKeeps) {
                out << ":" << std::to_string(seed);
            }
            out << std::endl;
        }

        if (_world->_shopsWithDoorBlownOpen.size() != 0) {
            out << "BLOWNUPSHOPS";
            for (auto& shop : _world->_shopsWithDoorBlownOpen) {
                out << ":" << std::to_string(shop.x) << "," << std::to_string(shop.y);
            }
            out << std::endl;
        }
    }

    static void saveMiniMapData(std::ofstream& out) {
        if (_world->getPlayer()->getInventory().hasItem(Item::getIdFromName("Map"))) {
            out << "MAPDATA";
            const std::map<TERRAIN_TYPE, std::string> encoding =
            {
                {TERRAIN_TYPE::EMPTY, "0"},
                {TERRAIN_TYPE::MOUNTAIN_LOW, "1"},
                {TERRAIN_TYPE::MOUNTAIN_MID, "2"},
                {TERRAIN_TYPE::MOUNTAIN_HIGH, "3"},
                {TERRAIN_TYPE::SAND, "4"},
                {TERRAIN_TYPE::WATER, "5"},
                {TERRAIN_TYPE::GRASS, "6"},
                {TERRAIN_TYPE::TUNDRA, "8"},
                {TERRAIN_TYPE::DESERT, "9"},
                {TERRAIN_TYPE::SAVANNA, "A"},
                {TERRAIN_TYPE::FLESH, "B"},
                {TERRAIN_TYPE::GRASS_FOREST, "C"},
                {TERRAIN_TYPE::RIVER, "D"},
                {TERRAIN_TYPE::FUNGUS, "E"}
            };

            const int mapSize = MiniMapGenerator::CHUNK_SIZE_SCALED * MiniMapGenerator::MAP_SIZE_DEFAULT_CHUNKS;
            const int chunkSize = MiniMapGenerator::CHUNK_SIZE_SCALED;
            for (int y = 0; y < mapSize - chunkSize; y += chunkSize) {
                for (int x = 0; x < mapSize - chunkSize; x += chunkSize) {
                    const TERRAIN_TYPE firstSubChunk = MiniMapGenerator::getData(x + y * mapSize);
                    if (firstSubChunk != TERRAIN_TYPE::EMPTY) {
                        out << ":" << std::to_string(x) << "," << std::to_string(y) << ".";
                        bool sameTerrainType = true;
                        for (int ya = y; ya < y + chunkSize; ya++) {
                            for (int xa = x; xa < x + chunkSize; xa++) {
                                const TERRAIN_TYPE currentSubChunk = MiniMapGenerator::getData(xa + ya * mapSize);
                                if (currentSubChunk == firstSubChunk && sameTerrainType) {
                                    if (xa == x && ya == y) out << encoding.at(currentSubChunk);
                                    continue;
                                } else {
                                    if (sameTerrainType) {
                                        for (int i = 0; i < (xa - x) + (ya - y) * chunkSize - 1; i++) {
                                            out << encoding.at(firstSubChunk);
                                        }
                                    }

                                    out << encoding.at(currentSubChunk);
                                    sameTerrainType = false;
                                }
                            }
                        }
                    }
                }
            }

            if (!MiniMapGenerator::getPoiLocations().empty()) {
                out << std::endl << "MAPPOIS";
                for (const auto& location : MiniMapGenerator::getPoiLocations()) {
                    out << ":" << std::to_string(location.x) + "," << std::to_string(location.y);
                }
            }

            if (!MiniMapGenerator::getPinLocations().empty()) {
                out << std::endl << "MAPPINS";
                for (const auto& location : MiniMapGenerator::getPinLocations()) {
                    out << ":" << std::to_string(location.x) + "," << std::to_string(location.y);
                }
            }
        }
    }

    static void saveAbilities(std::ofstream& out) {
        for (const auto& ability : Ability::ABILITIES) {
            out << "ABILITY:"
                << std::to_string(ability->getId())
                << ":" << std::to_string(ability->playerHasAbility());

            for (auto& parameter : ability->getParameters()) {
                out << ":" << parameter.first << "," << std::to_string(parameter.second);
            }
            out << std::endl;
        }
    }

    static void saveEffects(std::ofstream& out) {
        if (PlayerVisualEffectManager::getPlayerEffects().size() > 0) {
            out << "EFFECTS";
            for (const unsigned int effectId : PlayerVisualEffectManager::getPlayerEffects()) {
                out << ":" << std::to_string(effectId);
            }
            out << std::endl;
        }
    }

    static void saveShopData(std::ofstream& out) {
        if (_shopManager->getShopLedger().size() != 0) {
            auto& ledger = _shopManager->getShopLedger();
            for (auto& shopLocation : ledger) {
                out << "SHOPLOCATION:" << std::to_string(shopLocation.first);
                for (auto& entry : shopLocation.second) {
                    out << ":" << std::to_string(entry.first) << "," << std::to_string(entry.second.first) << "," << std::to_string(entry.second.second);
                }
                out << std::endl;
            }
        }

        if (_shopManager->_discountHistory.size() != 0) {
            const auto& discountHist = _shopManager->_discountHistory;
            out << "DISCOUNTS";
            for (auto& discountEntry : discountHist) {
                out << ":" << discountEntry.first << "," << discountEntry.second.first << "," << discountEntry.second.second;
            }
            out << std::endl;
        }

        if (_shopManager->_initialShopInventories.size() != 0) {
            const auto& inventories = _shopManager->_initialShopInventories;
            out << "SHOPINITS";
            for (const auto& inventory : inventories) {
                out << ":" << inventory.first;
                for (const auto& item : inventory.second) {
                    out << "." << std::to_string(item.first) << "," << std::to_string(item.second);
                }
            }
            out << std::endl;
        }
    }

    static void savePlayerData(std::ofstream& out) {
        auto& player = _world->getPlayer();
        out << "PLAYER:" << player->getUID();
        out << ":" << std::to_string(player->getPosition().x) << "," << std::to_string(player->getPosition().y);
        out << ":" << std::to_string(player->getHitPoints());
        out << ":" << std::to_string(player->getMaxHitPoints());
        out << ":" << std::to_string(player->getMagazineAmmoType());
        out << ":" << std::to_string(player->getMagazineContents());
        out << ":" << std::to_string(player->getMagazineSize());
        out << ":" << std::to_string(player->getStamina());
        out << ":" << std::to_string(player->getMaxStamina());
        out << ":" << std::to_string(player->getStaminaRefreshRate());
        out << ":" << std::to_string(player->getDamageMultiplier());
        out << ":" << std::to_string(player->getCoinMagnetCount());
        out << ":" << std::to_string(player->getSpeedMultiplier());
        out << std::endl;
        
        if (player->getInventory().getCurrentSize() != 0) {
            out << "PINVENTORY";
            for (int i = 0; i < player->getInventory().getCurrentSize(); i++) {
                out << ":"
                    << std::to_string(player->getInventory().getItemIdAt(i))
                    << "," << std::to_string(player->getInventory().getItemAmountAt(i))
                    << "," << (player->getInventory().isEquipped(i) ? "1" : "0");
            }
            out << std::endl;
        }
    }

    static void saveEntityData(std::ofstream& out) {
        for (auto& entity : _world->getEntities()) {
            if (entity->isActive() && entity->getSaveId() != NO_SAVE && entity->getSaveId() != PLAYER) {
                if (entity->getSaveData() == "NOSAVE") continue;
                out << "ENTITY:"
                    << std::to_string((int)entity->getSaveId())
                    << ":" << entity->getUID()
                    << ":" << std::to_string(entity->getPosition().x) << "," << std::to_string(entity->getPosition().y)
                    << ":" << std::to_string(entity->getHitPoints())
                    << ":" << entity->getSaveData();
                out << std::endl;
            }
        }
    }

    static void saveStats(std::ofstream& out) {
        out << "STATS";
        for (int i = 0; i < StatManager::NUM_STATS; i++) {
            out << ":" << std::to_string(StatManager::getStatThisSave((STATISTIC)i));
        }
        out << std::endl;

        out << "CAT";
        for (int i = 0; i < 3; i++) {
            out << ":" << ConditionalUnlockManager::_catItems[i];
        }
        out << std::endl;
    }
    
    inline static std::vector<std::vector<std::string>> _deferredOrbiters;
    static void loadDeferredOrbiters() {
        for (auto& deferredData : _deferredOrbiters) {
            std::string uid = deferredData[1];
            std::vector<std::string> posData = splitString(deferredData[2], ",");
            sf::Vector2f pos(std::stof(posData[0]), std::stof(posData[1]));
            int hitPoints = std::stoi(deferredData[3]);

            std::shared_ptr<Entity> entity = nullptr;

            unsigned int orbiterTypeId = std::stoul(deferredData[4]);
            std::string parentUID = deferredData[5];
            float angle = std::stof(deferredData[6]); 
            float distance = 0;
            if (deferredData.size() > 7) distance = std::stof(deferredData[7]);
            Entity* parent = nullptr;
            bool foundParent = false;
            for (auto& entity : _world->getEntities()) {
                if (entity->getUID() == parentUID) {
                    parent = entity.get();
                    foundParent = true;
                    break;
                }
            }

            if (foundParent) {
                std::shared_ptr<Orbiter> orbiter = std::shared_ptr<Orbiter>(new Orbiter(angle, orbiterTypeId, parent));
                if (distance != 0) orbiter->setDistance(distance);
                entity = orbiter;
                entity->setUID(uid);
                entity->_hitPoints = hitPoints;
                entity->loadSprite(_world->getSpriteSheet());
                entity->setWorld(_world);
                _world->addEntity(entity);
            } else {
                MessageManager::displayMessage("Parent not found for deferred orbiter " + uid, 5, ERR);
            }
        }

        _deferredOrbiters.clear();
    }

    inline static std::vector<std::vector<std::string>> _deferredProjectiles;
    static void loadDeferredProjectiles() {
        for (auto& deferredData : _deferredProjectiles) {
            std::string uid = deferredData[1];
            std::vector<std::string> posData = splitString(deferredData[2], ",");
            sf::Vector2f pos(std::stof(posData[0]), std::stof(posData[1]));
            int hitPoints = std::stoi(deferredData[3]);

            std::shared_ptr<Entity> entity = nullptr;

            std::string parentUID = deferredData[4];
            float angle = std::stof(deferredData[5]);
            float velocity = std::stof(deferredData[6]);

            unsigned int itemId = std::stoul(deferredData[7]);
            float baseVelocity = std::stof(deferredData[8]);

            std::vector<std::string> hitBoxData = splitString(deferredData[9], ",");
            sf::IntRect hitBox(std::stoi(hitBoxData[0]), std::stoi(hitBoxData[1]), std::stoi(hitBoxData[2]), std::stoi(hitBoxData[3]));

            bool rotateSprite = deferredData[10] == "1";
            bool onlyHitEnemies = deferredData[11] == "1";
            long long lifeTime = std::stoll(deferredData[12]);
            bool isAnimated = deferredData[13] == "1";
            int animationFrames = std::stoi(deferredData[14]);
            int animationSpeed = std::stoi(deferredData[15]);
            bool onlyDamagePlayer = deferredData[16] == "1";

            ProjectileData projData = {
                itemId, baseVelocity, hitBox, rotateSprite, onlyHitEnemies, lifeTime, isAnimated, animationFrames, animationSpeed
            };

            Entity* parent = nullptr;
            bool foundParent = false;
            for (auto& entity : _world->getEntities()) {
                if (entity->getUID() == parentUID) {
                    parent = entity.get();
                    foundParent = true;
                    break;
                }
            }

            if (foundParent) {
                entity = std::shared_ptr<Projectile>(new Projectile(pos, parent, angle, velocity, projData, onlyDamagePlayer));
                entity->setUID(uid);
                entity->_hitPoints = hitPoints;
                entity->loadSprite(_world->getSpriteSheet());
                entity->setWorld(_world);
                _world->addEntity(entity);
            } else {
                MessageManager::displayMessage("Parent not found for deferred projectile " + uid, 5, ERR);
            }
        }

        _deferredProjectiles.clear();
    }

    inline static long long saveFileTimeStamp = 0;
    static void load(std::string header, std::vector<std::string> data) {
        if (header == "GAMEVERSION") {
            if (data[0] != VERSION) MessageManager::displayMessage("Outdated save file: v" + data[0] + "\nClient v" + VERSION, 5, WARN);
        } else if (header == "TS") {
            saveFileTimeStamp = std::stoll(data[0]);
        } else if (header == "WORLD") {
            unsigned int seed = std::stoul(data[0]);
            _world->_cooldownStartTime = currentTimeMillis() - (saveFileTimeStamp - std::stoll(data[1]));
            _world->_maxEnemiesReached = data[2] == "1";
            _world->_cooldownActive = data[3] == "1";
            _world->_enemySpawnCooldownTimeMilliseconds = std::stoll(data[4]);
            _world->_maxActiveEnemies = std::stoi(data[5]);
            _world->_enemiesSpawnedThisRound = std::stoi(data[6]);
            _world->_waveCounter = std::stoi(data[7]);
            _world->_currentWaveNumber = std::stoi(data[8]);
            if (data.size() > 9) {
                _world->_highestPlayerHp = std::stoi(data[9]);
            }

            _world->init(seed);
        } else if (header == "MAPDATA") {
            const std::map<char, TERRAIN_TYPE> encoding =
            {
                {'0', TERRAIN_TYPE::EMPTY},
                {'1', TERRAIN_TYPE::MOUNTAIN_LOW},
                {'2', TERRAIN_TYPE::MOUNTAIN_MID},
                {'3', TERRAIN_TYPE::MOUNTAIN_HIGH},
                {'4', TERRAIN_TYPE::SAND},
                {'5', TERRAIN_TYPE::WATER},
                {'6', TERRAIN_TYPE::GRASS},
                {'8', TERRAIN_TYPE::TUNDRA},
                {'9', TERRAIN_TYPE::DESERT},
                {'A', TERRAIN_TYPE::SAVANNA},
                {'B', TERRAIN_TYPE::FLESH},
                {'C', TERRAIN_TYPE::GRASS_FOREST},
                {'D', TERRAIN_TYPE::RIVER},
                {'E', TERRAIN_TYPE::FUNGUS}
            };

            for (const std::string& chunk : data) {
                const auto rawChunkData = splitString(chunk, ".");
                const auto rawCoordData = splitString(rawChunkData[0], ",");
                const std::string& rawSubChunkData = rawChunkData[1];

                const sf::Vector2i coords(std::stoi(rawCoordData[0]), std::stoi(rawCoordData[1]));
                const int chunkSize = MiniMapGenerator::CHUNK_SIZE_SCALED;
                for (int y = 0; y < chunkSize; y++) {
                    for (int x = 0; x < chunkSize; x++) {
                        const char& subChunk = rawSubChunkData.size() == 1 ? rawSubChunkData.at(0) : rawSubChunkData.at(x + y * chunkSize);
                        MiniMapGenerator::_data[(coords.x + x) + (coords.y + y) * chunkSize * MiniMapGenerator::MAP_SIZE_DEFAULT_CHUNKS] = encoding.at(subChunk);
                    }
                }
            }
        } else if (header == "MAPPOIS") {
            for (const auto& rawLocation : data) {
                const auto splitData = splitString(rawLocation, ",");
                MiniMapGenerator::_poiLocations.push_back(sf::Vector2i(std::stoi(splitData[0]), std::stoi(splitData[1])));
            }
        } else if (header == "MAPPINS") {
            for (const auto& rawLocation : data) {
                const auto splitData = splitString(rawLocation, ",");
                MiniMapGenerator::_markerLocations.push_back(sf::Vector2i(std::stoi(splitData[0]), std::stoi(splitData[1])));
            }
        } else if (header == "HARD") {
            HARD_MODE_ENABLED = true;
        } else if (header == "PERF") {
            MID_GAME_PERF_BOOST = true;
        } else if (header == "SCORE") {
            PLAYER_SCORE = std::stof(data[0]);
        } else if (header == "STATS") {
            for (int i = 0; i < data.size(); i++) {
                StatManager::setStatThisSave((STATISTIC)i, std::stof(data[i]));
            }
        } else if (header == "CAT") {
            for (int i = 0; i < data.size() && i < 3; i++) {
                ConditionalUnlockManager::_catItems[i] = std::stoul(data[i]);
            }
        } else if (header == "ABILITY") {
            const unsigned int id = std::stoul(data[0]);
            const bool playerHasAbility = data[1] == "1";
            if (playerHasAbility) AbilityManager::givePlayerAbility(id);
            if (data.size() > 2) {
                for (int i = 2; i < data.size(); i++) {
                    const std::vector<std::string> parameter = splitString(data[i], ",");
                    if (parameter.size() == 2) {
                        const std::string key = parameter[0];
                        const float value = std::stof(parameter[1]);
                        AbilityManager::setParameter(id, key, value);
                    } else {
                        MessageManager::displayMessage("Bad ability parameter data: " + std::to_string(parameter.size()), 5, WARN);
                    }
                }
            }
        } else if (header == "EFFECTS") {
            for (int i = 0; i < data.size(); i++) {
                const unsigned int effectId = std::stoul(data[i]);
                for (const auto& effect : PlayerVisualEffectManager::_effectTypes) {
                    if (effect.id == effectId) {
                        PlayerVisualEffectManager::addEffectToPlayer(effect.name);
                        break;
                    }
                }
            }
        } else if (header == "DESTROYEDPROPS") {
            for (auto& propPosData : data) {
                std::vector<std::string> parsedData = splitString(propPosData, ",");
                sf::Vector2f propPos(std::stof(parsedData[0]), std::stof(parsedData[1]));
                _world->propDestroyedAt(propPos);
            }
        } else if (header == "SEENSHOPS") {
            for (auto& shopPosData : data) {
                std::vector<std::string> parsedData = splitString(shopPosData, ",");
                sf::Vector2f shopPos(std::stof(parsedData[0]), std::stof(parsedData[1]));
                _world->shopSeenAt(shopPos);
            }
        } else if (header == "VISITEDSHOPS") {
            for (int i = 0; i < data.size(); i++) {
                if (i >= _world->_visitedShops.size()) {
                    MessageManager::displayMessage(
                        "Visited shop data discrepency\nWorld visited shops: " + std::to_string(_world->_visitedShops.size()) + "\nSave data visited shops: " + std::to_string(data.size()), 
                        5, WARN
                    );
                    break;
                }

                _world->_visitedShops.at(i) = data.at(i) == "1";
            }
        } else if (header == "ALTARS") {
            for (auto& altarPosData : data) {
                std::vector<std::string> parsedData = splitString(altarPosData, ",");
                sf::Vector2f altarPos(std::stof(parsedData[0]), std::stof(parsedData[1]));
                _world->altarActivatedAt(altarPos);
            }
        } else if (header == "BLOWNUPSHOPS") {
            for (auto& shopDoorData : data) {
                std::vector<std::string> parsedData = splitString(shopDoorData, ",");
                sf::Vector2f shopPos(std::stof(parsedData[0]), std::stof(parsedData[1]));
                _world->shopDoorBlownUpAt(shopPos);
            }
        } else if (header == "DEADSHOPKEEPS") {
            for (auto& seedData : data) {
                _world->shopKeepKilled(std::stoul(seedData));
            }
        } else if (header == "SHOPLOCATION") {
            unsigned int seed = std::stoul(data[0]);
            for (int i = 1; i < data.size(); i++) {
                std::vector<std::string> parsedData = splitString(data[i], ",");
                unsigned int transactionNumber = std::stoi(parsedData[0]);
                unsigned int itemId = std::stoi(parsedData[1]);
                int amount = std::stoi(parsedData[2]);

                _shopManager->_shopLedger[seed][transactionNumber] = std::make_pair(itemId, amount);
            }
        } else if (header == "DISCOUNTS") {
            for (const auto& discountEntryRaw : data) {
                const std::vector<std::string> parsedEntry = splitString(discountEntryRaw, ",");

                const unsigned int shopSeed = std::stoul(parsedEntry[0]);
                const unsigned int itemId = std::stoul(parsedEntry[1]);
                const float discount = std::stof(parsedEntry[2]);

                _shopManager->_discountHistory[shopSeed] = { itemId, discount };
            }
        } else if (header == "SHOPINITS") {
            for (const auto& initEntryRaw : data) {
                const std::vector<std::string> parsedEntry = splitString(initEntryRaw, ".");
                const unsigned int seed = std::stoul(parsedEntry.at(0));
                for (int i = 1; i < parsedEntry.size(); i++) {
                    const std::vector<std::string> parsedItem = splitString(parsedEntry.at(i), ",");
                    const unsigned int itemId = std::stoul(parsedItem.at(0));
                    const unsigned int amount = std::stoul(parsedItem.at(1));

                    _shopManager->addItemToInitialInventory(seed, itemId, amount);
                }
            }
        } else if (header == "PLAYER") {
            auto& player = _world->getPlayer();
            player->setUID(data[0]);
            player->_pos.x = std::stof(splitString(data[1], ",")[0]);
            player->_pos.y = std::stof(splitString(data[1], ",")[1]);
            player->_hitPoints = std::stoi(data[2]);
            player->_maxHitPoints = std::stoi(data[3]);
            player->_magazineAmmoType = std::stoul(data[4]);
            player->_magazineContents = std::stoi(data[5]);
            player->_magazineSize = std::stoi(data[6]);
            player->_magContentsPercentage = ((float)player->getMagazineContents() / (float)player->getMagazineSize()) * 100.f;
            player->_stamina = std::stoi(data[7]);
            player->_maxStamina = std::stoi(data[8]);
            player->_staminaRefreshRate = std::stoi(data[9]);
            player->_damageMultiplier = std::stof(data[10]);
            player->_coinMagnetCount = std::stoul(data[11]);

            if (data.size() < 13) {
                player->_speedMultiplier = 0.f;
                return;
            }
            player->_speedMultiplier = std::stof(data[12]);
        } else if (header == "PINVENTORY") {
            auto& player = _world->getPlayer();

            std::vector<unsigned int> equippedItemIds;
            for (int i = 0; i < data.size(); i++) {
                std::vector<std::string> parsedData = splitString(data[i], ",");
                unsigned int itemId = std::stoi(parsedData[0]);
                if (itemId >= Item::ITEMS.size()) continue;
                unsigned int amount = std::stoi(parsedData[1]);
                bool isEquipped = parsedData[2] == "1";
                if (isEquipped) equippedItemIds.push_back(itemId);

                player->getInventory().addItem(itemId, amount);
                //if (isEquipped) player->getInventory().equip(i, Item::ITEMS[itemId]->getEquipmentType());
            }

            for (int i = 0; i < player->getInventory().getCurrentSize(); i++) {
                for (unsigned int equippedItemId : equippedItemIds) {
                    if (player->getInventory().getItemIdAt(i) == equippedItemId) {
                        player->getInventory().equip(i, Item::ITEMS[equippedItemId]->getEquipmentType());
                    }
                }
            }

        } else if (header == "ENTITY") {
            ENTITY_SAVE_ID saveId = (ENTITY_SAVE_ID)std::stoi(data[0]);
            std::string uid = data[1];
            std::vector<std::string> posData = splitString(data[2], ",");
            sf::Vector2f pos(std::stof(posData[0]), std::stof(posData[1]));
            int hitPoints = std::stoi(data[3]);

            std::shared_ptr<Entity> entity = nullptr;
            bool entityLoadedSuccessfully = true;

            switch (saveId) {
                case ORBITER:
                {
                    unsigned int orbiterTypeId = std::stoul(data[4]);
                    std::string parentUID = data[5];
                    float angle = std::stof(data[6]);
                    float distance = 0;
                    if (data.size() > 7) distance = std::stof(data[7]);
                    Entity* parent = nullptr;
                    bool foundParent = false;
                    for (auto& entity : _world->getEntities()) {
                        if (entity->getUID() == parentUID) {
                            parent = entity.get();
                            foundParent = true;
                            break;
                        }
                    }

                    if (foundParent) {
                        std::shared_ptr<Orbiter> orbiter = std::shared_ptr<Orbiter>(new Orbiter(angle, orbiterTypeId, parent));
                        if (distance != 0) orbiter->setDistance(distance);
                        entity = orbiter;
                    } else {
                        entityLoadedSuccessfully = false;
                        _deferredOrbiters.push_back(data);
                    }
                    break;
                }
                case CACTOID:
                {
                    bool isAggro = data[4] == "1";
                    std::shared_ptr<Cactoid> cactoid = std::shared_ptr<Cactoid>(new Cactoid(pos));
                    cactoid->_isAggro = isAggro;
                    entity = cactoid;
                    break;
                }
                case DROPPED_ITEM:
                {
                    unsigned int itemId = std::stoul(data[4]);
                    unsigned int amount = std::stoul(data[5]);
                    entity = std::shared_ptr<DroppedItem>(new DroppedItem(pos, 1.f, itemId, amount, Item::ITEMS[itemId]->getTextureRect(), true));
                    break;
                }
                case PROJECTILE:
                {
                    std::string parentUID = data[4];
                    float angle = std::stof(data[5]);
                    float velocity = std::stof(data[6]);

                    unsigned int itemId = std::stoul(data[7]);
                    float baseVelocity = std::stof(data[8]);

                    std::vector<std::string> hitBoxData = splitString(data[9], ",");
                    sf::IntRect hitBox(std::stoi(hitBoxData[0]), std::stoi(hitBoxData[1]), std::stoi(hitBoxData[2]), std::stoi(hitBoxData[3]));

                    bool rotateSprite = data[10] == "1";
                    bool onlyHitEnemies = data[11] == "1";
                    long long lifeTime = std::stoll(data[12]);
                    bool isAnimated = data[13] == "1";
                    int animationFrames = std::stoi(data[14]);
                    int animationSpeed = std::stoi(data[15]);
                    bool onlyDamagePlayer = data[16] == "1";

                    ProjectileData projData = {
                        itemId, baseVelocity, hitBox, rotateSprite, onlyHitEnemies, lifeTime, isAnimated, animationFrames, animationSpeed
                    };

                    Entity* parent = nullptr;
                    bool foundParent = false;
                    for (auto& entity : _world->getEntities()) {
                        if (entity->getUID() == parentUID) {
                            parent = entity.get();
                            foundParent = true;
                            break;
                        }
                    }

                    if (foundParent) {
                        entity = std::shared_ptr<Projectile>(new Projectile(pos, parent, angle, velocity, projData, onlyDamagePlayer));
                    } else {
                        entityLoadedSuccessfully = false;
                        _deferredProjectiles.push_back(data);
                    }

                    break;
                }
                case DOG: 
                {
                    bool hasOwner = data[4] == "1";
                    std::string parentUID = data[5];

                    Entity* parent = nullptr;
                    bool foundParent = false;
                    if (hasOwner) {
                        for (auto& entity : _world->getEntities()) {
                            if (entity->getUID() == parentUID) {
                                parent = entity.get();
                                foundParent = true;
                                break;
                            }
                        }
                    }

                    if (foundParent || !hasOwner) {
                        std::shared_ptr<Dog> dog = std::shared_ptr<Dog>(new Dog(pos));
                        if (hasOwner) {
                            dog->setWorld(_world);
                            dog->setParent(parent);
                        }
                        entity = dog;
                    } else {
                        entityLoadedSuccessfully = false;
                        MessageManager::displayMessage("Did not find parent for Dog " + uid + "\nParent UID: " + parentUID, 5, WARN);

                        std::shared_ptr<Dog> dog = std::shared_ptr<Dog>(new Dog(pos));
                        if (hasOwner) {
                            dog->setWorld(_world);
                            dog->setParent(_world->getPlayer().get());
                        }
                        entity = dog;
                    }
                    break;
                }
                case PLANTMAN:
                    entity = std::shared_ptr<PlantMan>(new PlantMan(pos));
                    break;
                case SNOWMAN:
                    entity = std::shared_ptr<SnowMan>(new SnowMan(pos));
                    break;
                case FROG:
                    entity = std::shared_ptr<Frog>(new Frog(pos));
                    break;
                case TURTLE:
                    entity = std::shared_ptr<Turtle>(new Turtle(pos));
                    break;
                case PENGUIN:
                    entity = std::shared_ptr<Penguin>(new Penguin(pos));
                    break;
                case YETI:
                    entity = std::shared_ptr<Yeti>(new Yeti(pos));
                    break;
                case SKELETON:
                    entity = std::shared_ptr<Skeleton>(new Skeleton(pos));
                    break;
                case SHARK:
                    entity = std::shared_ptr<Shark>(new Shark(pos));
                    break;
                case CYCLOPS:
                    entity = std::shared_ptr<Cyclops>(new Cyclops(pos));
                    break;
                case CHEESE_BOSS:
                    entity = std::shared_ptr<CheeseBoss>(new CheeseBoss(pos));
                    if (data[4] == "1") {
                        Boss* boss = dynamic_cast<Boss*>(entity.get());
                        boss->deactivateBossMode();
                    }
                    break;
                case FLESH_CHICKEN:
                    entity = std::shared_ptr<FleshChicken>(new FleshChicken(pos));
                    break;
                case CANNON_BOSS:
                    entity = std::shared_ptr<CannonBoss>(new CannonBoss(pos));
                    break;
                case LOG_MONSTER:
                    entity = std::shared_ptr<LogMonster>(new LogMonster(pos));
                    break;
                case BOULDER_BEAST:
                    entity = std::shared_ptr<BoulderBeast>(new BoulderBeast(pos));
                    break;
                case TULIP_MONSTER:
                    entity = std::shared_ptr<TulipMonster>(new TulipMonster(pos));
                    break;
                case TREE_BOSS:
                    entity = std::shared_ptr<TreeBoss>(new TreeBoss(pos));
                    break;
                case CREAM_BOSS:
                    entity = std::shared_ptr<CreamBoss>(new CreamBoss(pos));
                    break;
                case CHEF_BOSS:
                    entity = std::shared_ptr<ChefBoss>(new ChefBoss(pos));
                    break;
                case BURGER_BEAST:
                    entity = std::shared_ptr<BurgerBeast>(new BurgerBeast(pos));
                    break;
                case BOMB_BOY:
                    entity = std::shared_ptr<BombBoy>(new BombBoy(pos));
                    break;
                case MEGA_BOMB_BOY:
                    entity = std::shared_ptr<MegaBombBoy>(new MegaBombBoy(pos));
                    break;
                case SOLDIER:
                    entity = std::shared_ptr<Soldier>(new Soldier(pos));
                    break;
                case BABY_BOSS:
                    entity = std::shared_ptr<BabyBoss>(new BabyBoss(pos));
                    break;
                case BIG_SNOWMAN:
                    entity = std::shared_ptr<BigSnowMan>(new BigSnowMan(pos));
                    break;
                case BEE_FAMILIAR:
                {
                    const float orbiterAngle = std::stof(data[4]);
                    const float orbiterDistance = std::stof(data[5]);
                    entity = std::shared_ptr<BeeFamiliar>(new BeeFamiliar(pos, orbiterAngle, orbiterDistance));
                    break;
                }
                case TEETH_BOSS:
                    entity = std::shared_ptr<TeethBoss>(new TeethBoss(pos));
                    break;
                case MUSHROOM_BOSS:
                    entity = std::shared_ptr<MushroomBoss>(new MushroomBoss(pos));
                    break;
                case BLINKER:
                {
                    entity = std::shared_ptr<Blinker>(new Blinker(pos));
                    const float angle = std::stof(data[4]);
                    Blinker* blinker = dynamic_cast<Blinker*>(entity.get());
                    blinker->_angle = angle;
                    break;
                }
                case MUSHROID:
                {
                    bool isAggro = data[4] == "1";
                    std::shared_ptr<Mushroid> mushroid = std::shared_ptr<Mushroid>(new Mushroid(pos));
                    mushroid->_isAggro = isAggro;
                    entity = mushroid;
                    break;
                }
                case FUNGUY:
                    entity = std::shared_ptr<Funguy>(new Funguy(pos));
                    break;
                case FUNGUS_MAN:
                    entity = std::shared_ptr<FungusMan>(new FungusMan(pos));
                    break;
            }

            if (entityLoadedSuccessfully) {
                entity->setUID(uid);
                entity->loadSprite(_world->getSpriteSheet());
                entity->setWorld(_world);
                // not deferring here solves a problem but also scares me a little check for bugs
                _world->addEntity(entity, false);
                // fixes bosses spawning with full hp when loading a save during a boss fight
                entity->_hitPoints = hitPoints;
            }
        }
    }
};

#endif 
