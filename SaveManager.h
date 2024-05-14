#ifndef _SAVE_MANAGER_H
#define _SAVE_MANAGER_H

#include "World.h"
#include "Orbiter.h"

class SaveManager {
public:
    static void saveGame() {
        std::string fileName = "save/save";
        try {
            if (!std::filesystem::remove(fileName))
                MessageManager::displayMessage("Could not replace save file", 5, ERR);
        } catch (const std::filesystem::filesystem_error& err) {
            MessageManager::displayMessage("Could not replace save file: " + (std::string)err.what(), 5, ERR);
        }

        try {
            std::ofstream out(fileName);
            
            out << "TS:" << std::to_string(currentTimeMillis()) << std::endl;
            out << "SCORE:" << std::to_string(PLAYER_SCORE) << std::endl;
            saveWorldData(out);
            saveShopData(out);
            savePlayerData(out);
            saveEntityData(out);

            out.close();

            MessageManager::displayMessage("Game saved succesfully", 2);
        } catch (std::exception ex) {
            MessageManager::displayMessage("Error writing to save file: " + (std::string)ex.what(), 5, ERR);
        }
    }

    static bool loadGame() {
        bool loadedSuccessfully = true;

        std::ifstream in("save/save");
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
                    MessageManager::displayMessage("Error loading save file: " + (std::string)ex.what(), 5, ERR);
                    loadedSuccessfully = false;
                }
            }
        }

        in.close();

        return loadedSuccessfully;
    }

    static void init(World* world, ShopManager* shopManager) {
        _world = world;
        _shopManager = shopManager;
    }
private:
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
    }

    static void savePlayerData(std::ofstream& out) {
        auto& player = _world->getPlayer();
        out << "PLAYER:" << player->getUUID();
        out << ":" << std::to_string(player->getPosition().x) << "," << std::to_string(player->getPosition().y);
        out << ":" << std::to_string(player->getHitPoints());
        out << ":" << std::to_string(player->getMaxHitPoints());
        out << ":" << std::to_string(player->getMagazineAmmoType());
        out << ":" << std::to_string(player->getMagazineContents());
        out << ":" << std::to_string(player->getMagazineSize());
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
            if (entity->isActive() && entity->getSaveId() != NO_SAVE && entity->getSaveId() != PLAYER && entity->getSaveId() != PROJECTILE) {
                out << "ENTITY:"
                    << std::to_string((int)entity->getSaveId())
                    << ":" << entity->getUUID()
                    << ":" << std::to_string(entity->getPosition().x) << "," << std::to_string(entity->getPosition().y) 
                    << ":" << entity->getSaveData();
                out << std::endl;
            }
        }
    }
    
    inline static std::vector<std::vector<std::string>> _deferredOrbiters;
    inline static long long saveFileTimeStamp = 0;
    static void load(std::string header, std::vector<std::string> data) {
        if (header == "TS") {
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

            _world->init(seed);
        } else if (header == "SCORE") {
            PLAYER_SCORE = std::stof(data[0]);
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
        } else if (header == "SHOPLOCATION") {
            unsigned int seed = std::stoul(data[0]);
            for (int i = 1; i < data.size(); i++) {
                std::vector<std::string> parsedData = splitString(data[i], ",");
                unsigned int transactionNumber = std::stoi(parsedData[0]);
                unsigned int itemId = std::stoi(parsedData[1]);
                int amount = std::stoi(parsedData[2]);

                _shopManager->_shopLedger[seed][transactionNumber] = std::make_pair(itemId, amount);
            }
        } else if (header == "PLAYER") {
            auto& player = _world->getPlayer();
            player->setUUID(data[0]);
            player->_pos.x = std::stof(splitString(data[1], ",")[0]);
            player->_pos.y = std::stof(splitString(data[1], ",")[1]);
            player->_hitPoints = std::stoi(data[2]);
            player->_maxHitPoints = std::stoi(data[3]);
            player->_magazineAmmoType = std::stoi(data[4]);
            player->_magazineContents = std::stoi(data[5]);
            player->_magazineSize = std::stoi(data[6]);
        } else if (header == "PINVENTORY") {
            auto& player = _world->getPlayer();
            for (int i = 0; i < data.size(); i++) {
                std::vector<std::string> parsedData = splitString(data[i], ",");
                unsigned int itemId = std::stoi(parsedData[0]);
                unsigned int amount = std::stoi(parsedData[1]);
                bool isEquipped = parsedData[2] == "1";

                player->getInventory().addItem(itemId, amount);
                if (isEquipped) player->getInventory().equip(i, Item::ITEMS[itemId]->getEquipmentType());
            }
        } else if (header == "ENTITY") {
            ENTITY_SAVE_ID saveId = (ENTITY_SAVE_ID)std::stoi(data[0]);
            std::string uuid = data[1];
            std::vector<std::string> posData = splitString(data[2], ",");
            sf::Vector2f pos(std::stof(posData[0]), std::stof(posData[1]));

            std::shared_ptr<Entity> entity = nullptr;
            bool entityLoadedSuccessfully = true;

            switch (saveId) {
                case ORBITER:
                {
                    unsigned int orbiterTypeId = std::stoul(data[3]);
                    std::string parentUUID = data[4];
                    Entity* parent = nullptr;
                    bool foundParent = false;
                    for (auto& entity : _world->getEntities()) {
                        if (entity->getUUID() == parentUUID) {
                            parent = entity.get();
                            foundParent = true;
                            break;
                        }
                    }

                    if (foundParent) {
                        entity = std::shared_ptr<Orbiter>(new Orbiter(pos, orbiterTypeId, parent));
                    } else {
                        entityLoadedSuccessfully = false;
                        _deferredOrbiters.push_back(data);
                    }
                    break;
                }
                case CACTOID:
                {
                    bool isAggro = data[3] == "1";
                    std::shared_ptr<Cactoid> cactoid = std::shared_ptr<Cactoid>(new Cactoid(pos));
                    cactoid->_isAggro = isAggro;
                    entity = cactoid;
                    break;
                }
                case DROPPED_ITEM:
                {
                    unsigned int itemId = std::stoul(data[3]);
                    unsigned int amount = std::stoul(data[4]);
                    entity = std::shared_ptr<DroppedItem>(new DroppedItem(pos, 1.f, itemId, amount, Item::ITEMS[itemId]->getTextureRect()));
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
            }

            if (entityLoadedSuccessfully) {
                entity->setUUID(uuid);
                entity->loadSprite(_world->getSpriteSheet());
                entity->setWorld(_world);
                _world->addEntity(entity);
            }
        }
    }
};

#endif 
