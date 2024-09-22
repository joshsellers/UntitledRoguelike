#ifndef _UI_COMMAND_PROMPT_H
#define _UI_COMMAND_PROMPT_H

#include "UIElement.h"
#include "../world/World.h"
#include "../world/Entities/Turtle.h"
#include "../world/Entities/Penguin.h"
#include "../world/Entities/PlantMan.h"
#include "../world/Entities/Cactoid.h"
#include <regex>
#include <boost/algorithm/string.hpp>
#include "../world/Entities/ShopInterior.h"
#include "../world/Entities/ShopExterior.h"
#include "../world/Entities/ShopCounter.h"
#include "../world/Entities/Frog.h"
#include "../world/Entities/SnowMan.h"
#include "../world/Entities/Yeti.h"
#include "../world/entities/Dog.h"
#include "../world/entities/Skeleton.h"
#include "../world/entities/Shark.h"
#include "../world/entities/BarberExterior.h"
#include "../world/entities/Cyclops.h"
#include "../world/entities/CheeseBoss.h"
#include "../world/entities/orbiters/Orbiter.h"
#include "../world/entities/FleshChicken.h"
#include "../world/entities/CannonBoss.h"
#include "../statistics/AchievementManager.h"
#include "../inventory/abilities/Ability.h"
#include "../inventory/abilities/AbilityManager.h"
#include "../world/entities/LogMonster.h"
#include "../core/Viewport.h"
#include "../world/entities/BoulderBeast.h"
#include "../world/entities/TulipMonster.h"
#include "../inventory/effect/PlayerVisualEffectManager.h"
#include "../world/entities/TreeBoss.h"

const bool LOCK_CMD_PROMPT = !DEBUG_MODE;
constexpr const char UNLOCK_HASH[11] = "2636727673";

struct Command {
    Command(const std::string description, const std::function<std::string(std::vector<std::string>&)> command)
        : description(description), command(command)
    {}
    const std::function<std::string(std::vector<std::string>&)> command;
    const std::string description;
};

class UICommandPrompt : public UIElement {
public:
    UICommandPrompt(World* world, sf::Font font);

    void update();
    void draw(sf::RenderTexture& surface);

    void mouseButtonPressed(const int mx, const int my, const int button);
    void mouseButtonReleased(const int mx, const int my, const int button);
    void mouseMoved(const int mx, const int my);
    void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    void keyReleased(sf::Keyboard::Key& key);
    void textEntered(const sf::Uint32 character);

    std::string processCommand(sf::String command);

    void unlock();
    void lock();
private:

    World* _world;

    sf::RectangleShape _bg;

    sf::Uint32 _historyIndex = 0;
    std::vector<sf::String> _history;
    
    sf::Text _hiddenText;
    bool _unlocked = !LOCK_CMD_PROMPT;

    std::map<std::string, Command> _commands = {
        {
            "give",
            Command("Give the player an item or items",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                if (parsedCommand.size() > 1) {
                    if (parsedCommand.at(1) == "tux") {
                        for (int id = Item::TOP_HAT.getId(); id < Item::DRESS_SHOES.getId() + 1; id++)
                            _world->getPlayer()->getInventory().addItem(id, 1);
                        return "Player given tuxedo";
                    } else if (parsedCommand.at(1) == "nottux") {
                        processCommand("give:" + std::to_string(Item::SOMBRERO.getId()));
                        processCommand("give:" + std::to_string(Item::WIFE_BEATER.getId()));
                        processCommand("give:" + std::to_string(Item::JORTS.getId()));
                        processCommand("give:" + std::to_string(Item::WHITE_TENNIS_SHOES.getId()));
                        return "Player given Spencer's outfit";
                    } else if (parsedCommand.at(1) == "matmura") {
                        processCommand("give:" + std::to_string(Item::MATMURA_HELMET.getId()));
                        processCommand("give:" + std::to_string(Item::MATMURA_CHESTPLATE.getId()));
                        processCommand("give:" + std::to_string(Item::MATMURA_LEGGINGS.getId()));
                        processCommand("give:" + std::to_string(Item::MATMURA_BOOTS.getId()));
                        return "Player given Matmura armor";
                    } else if (!std::regex_match(parsedCommand.at(1), std::regex("^[0-9]+$"))) {
                        std::shared_ptr<const Item> item = nullptr;
                        for (int i = 0; i < Item::ITEMS.size(); i++) {
                            std::string itemName = Item::ITEMS[i]->getName();
                            boost::to_lower(itemName);
                            if (parsedCommand.at(1) == itemName) {
                                try {
                                    item = Item::ITEMS[i];
                                    int amount = 1;
                                    if (parsedCommand.size() == 3)
                                        amount = stoi(parsedCommand.at(2));

                                    _world->getPlayer()->getInventory().addItem(i, amount);
                                    return "Player given " + std::to_string(amount) + " " + Item::ITEMS[i]->getName() + (amount > 1 ? "s" : "");
                                } catch (std::exception ex) {
                                    return ex.what();
                                }
                            }
                        }
                        return parsedCommand.at(1) + " is not a valid item";
                    } else {
                        try {
                            int itemId = stoi(parsedCommand.at(1));
                            if (itemId >= Item::ITEMS.size()) return "Invalid item ID: " + std::to_string(itemId);

                            int amount = 1;
                            if (parsedCommand.size() == 3)
                                amount = stoi(parsedCommand.at(2));

                            _world->getPlayer()->getInventory().addItem(itemId, amount);
                            return "Player given " + std::to_string(amount) + " " + Item::ITEMS[itemId]->getName() + (amount > 1 ? "s" : "");
                        } catch (std::exception ex) {
                            return ex.what();
                        }
                    }
                } else {
                    return "Not enough parameters for command: " + (std::string)("\"") + parsedCommand[0] + "\"";
                }
            })
        },

        {
            "tgm",
            Command("Toggle god mode",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                _world->getPlayer()->freeMove = !_world->getPlayer()->freeMove;
                if (_world->getPlayer()->freeMove) _world->getPlayer()->setBaseSpeed(8);
                else _world->getPlayer()->setBaseSpeed(BASE_PLAYER_SPEED);
                return "Godmode set to " + (std::string)(_world->getPlayer()->freeMove ? "true" : "false");
            })
        },

        {
            "addhp",
            Command("Add or remove hp from the player",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                if (parsedCommand.size() > 1) {
                    try {
                        int dHp = stoi(parsedCommand.at(1));
                        _world->getPlayer()->heal(dHp);
                        return "Player's HP set to " + std::to_string(_world->getPlayer()->getHitPoints());
                    } catch (std::exception ex) {
                        return ex.what();
                    }
                } else {
                    return "Not enough parameters for command: " + (std::string)("\"") + parsedCommand[0] + "\"";
                }
            })
        },

        {
            "setmaxhp",
            Command("Set the player's max hp",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                if (parsedCommand.size() > 1) {
                    try {
                        int mhp = stoi(parsedCommand.at(1));
                        _world->getPlayer()->setMaxHitPoints(mhp);
                        return "Player's max HP set to " + std::to_string(_world->getPlayer()->getMaxHitPoints());
                    } catch (std::exception ex) {
                        return ex.what();
                    }
                } else {
                    return "Not enough parameters for command: " + (std::string)("\"") + parsedCommand[0] + "\"";
                }
            })
        },

        {
            "killall",
            Command("Remove all entities except the player",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                for (auto& entity : _world->getEntities()) {
                    entity->deactivate();
                }
                _world->getPlayer()->activate();
                return "Set all entities to inactive";
            })
        },

        {
            "reseed",
            Command("Change the world seed",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                if (parsedCommand.size() > 1) {
                    try {
                        unsigned int seed = stoul(parsedCommand.at(1));
                        _world->reseed(seed);
                        return "World seed set to " + std::to_string(_world->getSeed());
                    } catch (std::exception ex) {
                    return ex.what();
                    }
                } else {
                    return "Not enough parameters for command: " + (std::string)("\"") + parsedCommand[0] + "\"";
                }
            })
        },

        {
            "summon",
            Command("Spawn an entity",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                if (parsedCommand.size() > 1) {
                    const std::string entityName = parsedCommand.at(1);
                    const sf::Vector2f playerPos = _world->getPlayer()->getPosition();

                    int amt = 1;
                    if (parsedCommand.size() > 2) {
                        try {
                            amt = stoi(parsedCommand.at(2));
                        } catch (std::exception ex) {
                            return ex.what();
                        }
                    }

                    for (int i = 0; i < amt; i++) {
                        const int offsetX = randomInt(16, 32);
                        const int offsetY = randomInt(16, 32);
                        const int signX = randomInt(-1, 0);
                        const int signY = randomInt(-1, 0);
                        const sf::Vector2f pos(playerPos.x + (offsetX * (signX == 0 ? 1 : signX)), playerPos.y + (offsetY * (signY == 0 ? 1 : signY)));

                        std::shared_ptr<Entity> entity = nullptr;
                        if (entityName == "plantman") {
                            entity = std::shared_ptr<PlantMan>(new PlantMan(pos));
                        } else if (entityName == "turtle") {
                            entity = std::shared_ptr<Turtle>(new Turtle(pos));
                        } else if (entityName == "penguin") {
                            entity = std::shared_ptr<Penguin>(new Penguin(pos));
                        } else if (entityName == "cactoid") {
                            entity = std::shared_ptr<Cactoid>(new Cactoid(pos));
                        } else if (entityName == "shopext") {
                            entity = std::shared_ptr<ShopExterior>(new ShopExterior(pos, _world->getSpriteSheet()));
                        } else if (entityName == "shopcounter") {
                            entity = std::shared_ptr<ShopCounter>(new ShopCounter(pos, _world->getSpriteSheet()));
                        } else if (entityName == "frog") {
                            entity = std::shared_ptr<Frog>(new Frog(pos));
                        } else if (entityName == "snowman") {
                            entity = std::shared_ptr<SnowMan>(new SnowMan(pos));
                        } else if (entityName == "yeti") {
                            entity = std::shared_ptr<Yeti>(new Yeti(pos));
                        } else if (entityName == "dog") {
                            entity = std::shared_ptr<Dog>(new Dog(pos));
                        } else if (entityName == "skeleton") {
                            entity = std::shared_ptr<Skeleton>(new Skeleton(pos));
                        } else if (entityName == "shark") {
                            entity = std::shared_ptr<Shark>(new Shark(pos));
                        } else if (entityName == "barberext") {
                            entity = std::shared_ptr<BarberExterior>(new BarberExterior(pos, _world->getSpriteSheet()));
                        } else if (entityName == "cyclops") {
                            entity = std::shared_ptr<Cyclops>(new Cyclops(pos));
                        } else if (entityName == "cheeseboss") {
                            entity = std::shared_ptr<CheeseBoss>(new CheeseBoss(pos));
                        } else if (entityName == "fleshchicken") {
                            entity = std::shared_ptr<FleshChicken>(new FleshChicken(pos));
                        } else if (entityName == "cannonboss") {
                            entity = std::shared_ptr<CannonBoss>(new CannonBoss(pos));
                        } else if (entityName == "logmonster") {
                            entity = std::shared_ptr<LogMonster>(new LogMonster(pos));
                        } else if (entityName == "boulderbeast") {
                            entity = std::shared_ptr<BoulderBeast>(new BoulderBeast(pos));
                        } else if (entityName == "tulipmonster") {
                            entity = std::shared_ptr<TulipMonster>(new TulipMonster(pos));
                        } else if (entityName == "treeboss") {
                            entity = std::shared_ptr<TreeBoss>(new TreeBoss(pos));
                        } else {
                            return entityName + " is not a valid entity name";
                        }

                        entity->loadSprite(_world->getSpriteSheet());
                        entity->setWorld(_world);
                        _world->addEntity(entity);
                    }
                    return "Spawned " + std::to_string(amt) + " " + entityName + (amt > 1 ? "s" : "");
                } else {
                    return "Not enough parameters for command: " + (std::string)("\"") + parsedCommand[0] + "\"";
                }
            })
        },

        {
            "respawn",
            Command("Respawn the player",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                for (auto& entity : _world->getEntities()) {
                    if (entity->compare(_world->getPlayer().get())) {
                        return "Did not respawn because player has not been despawned";
                    }
                }

                processCommand("addhp:" + std::to_string(_world->getPlayer()->getMaxHitPoints()));
                _world->getPlayer()->activate();
                _world->addEntity(_world->getPlayer());
                return "Player respawned at full health";
            })
        },

        {
            "vibrate",
            Command("Cause the gamepad to vibrate",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                if (GamePad::isConnected()) {
                    if (parsedCommand.size() > 2) {
                        try {
                            int vibrationAmount = std::stoi(parsedCommand[1]);
                            long long time = std::stoll(parsedCommand[2]);

                            GamePad::vibrate(vibrationAmount, time);
                            return "Controller vibrating at strength " + std::to_string(vibrationAmount) + " for " + std::to_string(time) + " milliseconds";
                        } catch (std::exception ex) {
                            return ex.what();
                        }
                    } else return "Not enough parameters for command: " + (std::string)("\"") + parsedCommand[0] + "\"";
                } else {
                    return "No gamepad connected";
                }
            })
        },

        {
            "clear inventory",
            Command("Clear the player's inventory",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                while (_world->getPlayer()->getInventory().getCurrentSize() != 0) {
                    _world->getPlayer()->getInventory().removeItemAt(0, _world->getPlayer()->getInventory().getItemAmountAt(0));
                }
                return "Removed all items from player's inventory";
            })
        },

        {
            "tco",
            Command("Toggle chunk outlines",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                _world->drawChunkOutline = !_world->drawChunkOutline;
                return "drawChunkOutline set to " + (_world->drawChunkOutline ? (std::string)"true" : "false");
            })
        },

        {
            "tp",
            Command("Teleport the player",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                if (parsedCommand.size() == 3) {
                    try {
                        float x = std::stof(parsedCommand[1]);
                        float y = std::stof(parsedCommand[2]);
                        sf::Vector2f currentPos = _world->getPlayer()->getPosition();
                        _world->getPlayer()->move(x - currentPos.x, y - currentPos.y);

                        return "Player teleported to " + std::to_string(x) + ", " + std::to_string(y);
                    } catch (std::exception ex) {
                        return ex.what();
                    }
                } else return "Not enough parameters for command: " + (std::string)("\"") + parsedCommand[0] + "\"";
            })
        },

        {
            "hash-djb2",
            Command("Create a djb2 hash of a string",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                if (parsedCommand.size() == 2) {
                    std::string text = parsedCommand[1];
                    unsigned int hash = 0;
                    for (int i = 0; i < text.length(); i++) {
                        hash = ((hash << 5) + hash) + (unsigned int)text.at(i);
                    }
                    return std::to_string(hash);
                } else return "Not enough parameters for command: " + (std::string)("\"") + parsedCommand[0] + "\"";
            })
        },

        {
            "hash",
            Command("Create a FNV-1a hash of a string",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                if (parsedCommand.size() == 2) {
                    std::string text = parsedCommand[1];
                    unsigned int hash = 0x811c9dc5u;
                    for (int i = 0; i < text.length(); i++) {
                        hash = (hash ^ (unsigned int)text.at(i)) * 0x1000193u;
                    }
                    hash = (hash ^ (hash >> 16)) * 0x7feb352du;
                    hash = (hash ^ (hash >> 15)) * 0x846ca68bu;

                    return std::to_string(hash ^ (hash >> 16));
                } else return "Not enough parameters for command: " + (std::string)("\"") + parsedCommand[0] + "\"";
            })
        },

        {
            "lock",
            Command("Lock the command prompt",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                lock();
                return "Command prompt has been locked";
            })
        },

        {
            "tcbm",
            Command("Toggle terrain gen benchmark",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                BENCHMARK_TERRAIN_AND_BIOME_GEN = !BENCHMARK_TERRAIN_AND_BIOME_GEN;
                return (BENCHMARK_TERRAIN_AND_BIOME_GEN ? "Activated" : "Deactivated") + (std::string)" terrain generation benchmark";
            })
        },

        {
            "tms",
            Command("Toggle mob spawns",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                _world->disableMobSpawning = !_world->disableMobSpawning;
                return (_world->disableMobSpawning ? "Disabled" : "Enabled") + (std::string)" mob spawns";
            })
        },

        {
            "tes",
            Command("Toggle enemy spawns",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                _world->disableEnemySpawning = !_world->disableEnemySpawning;
                return (_world->disableEnemySpawning ? "Disabled" : "Enabled") + (std::string)" enemy spawns";
            })
        },

        {
            "help",
            Command("List all commands with descriptions",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                std::string output = "";
                for (auto& command : _commands) {
                    output += command.first + " - " + command.second.description + "\n";
                }
                return output;
            })
        },

        {
            "msg",
            Command("Display a message on the screen",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                int timeout = 5;
                int messageType = NORMAL;
                if (parsedCommand.size() >= 3) {
                    try {
                        timeout = stoi(parsedCommand[2]);
                    } catch (std::exception ex) {
                        return ex.what();
                    }
                }

                if (parsedCommand.size() == 4) {
                    auto messageTypeStr = parsedCommand[3];
                    if (messageTypeStr == "warn") messageType = WARN;
                    else if (messageTypeStr == "err") messageType = ERR;
                    else if (messageTypeStr == "debug") messageType = DEBUG;
                    else if (messageTypeStr != "normal") return "\"" + messageTypeStr + "\" is not a valid message type";
                }

                MessageManager::displayMessage(parsedCommand[1], timeout, messageType);
                return "";
            })
        },

        {
            "timestamp",
            Command("Display the current UNIX timestamp in milliseconds",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                return std::to_string(currentTimeMillis());
            })
        },

        {
            "loadchunksaroundplayer",
            Command("Force loadChunksAroundPlayer()",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                _world->loadChunksAroundPlayer();
                return "Called loadChunksAroundPlayer()";
            })
        },

        {
            "tpg",
            Command("Toggle prop generation",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                _world->disablePropGeneration = !_world->disablePropGeneration;
                return "disablePropGeneration set to " + (std::string)(_world->disablePropGeneration ? "true" : "false");
            })
        },

        {
            "toggledebug",
            Command("Toggle debug mode",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                DISPLAY_DEBUG_MESSAGES = !DISPLAY_DEBUG_MESSAGES;
                DEBUG_MODE = !DEBUG_MODE;
                return "DEBUG_MODE set to " + (std::string)(DEBUG_MODE ? "true" : "false");
            })
        },

        {
            "endenemycooldown",
            Command("End enemy spawn cooldown",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                _world->resetEnemySpawnCooldown();
                return "Ended enemy spawn cooldown";
            })
        },

        {
            "setscore",
            Command("Set the player's score",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                try {
                    if (parsedCommand.size() < 2) return "Not enough parameters for command: " + (std::string)("\"") + parsedCommand[0] + "\"";
                    PLAYER_SCORE = std::stof(parsedCommand[1]);
                    return "Score set to " + parsedCommand[1];
                } catch (std::exception ex) {
                    return ex.what();
                }
            })
        },

        {
            "removedroppeditems",
            Command("Remove all dropped items",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                int count = 0;
                for (const auto& entity : _world->getEntities()) {
                    if (entity->getEntityType() == "droppeditem") {
                        entity->deactivate();
                        count++;
                    }
                }

                return "Deactivated " + std::to_string(count) + " entities";
            })
        },

        {
            "setwavenumber",
            Command("Set the displayed wave number",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                try {
                    if (parsedCommand.size() < 2) return "Not enough parameters for command: " + (std::string)("\"") + parsedCommand[0] + "\"";
                    _world->setDisplayedWaveNumber(std::stoi(parsedCommand[1]));
                    return "Wave number set to " + parsedCommand[1];
                } catch (std::exception ex) {
                    return ex.what();
                }
            })
        },

        {
            "killenemies",
            Command("Remove all enemies",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                int count = 0;
                for (const auto& enemy : _world->getEnemies()) {
                    enemy->deactivate();
                    count++;
                }

                return "Deactivated " + std::to_string(count) + " entities";
            })
        },

        {
            "tdm",
            Command("Toggle diagonal movement",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                DIAGONAL_MOVEMENT_ENABLED = !DIAGONAL_MOVEMENT_ENABLED;
                return (DIAGONAL_MOVEMENT_ENABLED ? "Enabled" : "Disabled") + (std::string)" diagonal movement";
            })
        },

        {
            "tas",
            Command("Toggle autosave",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                AUTOSAVE_ENABLED = !AUTOSAVE_ENABLED;
                return (AUTOSAVE_ENABLED ? "Enabled" : "Disabled" + (std::string)" autosave");
            })
        },

        {
            "giveorbiter",
            Command("Give the player an orbiter",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                if (parsedCommand.size() > 1) {
                    unsigned int orbiterId;
                    bool foundOrbiter = false;
                    std::string inputName = parsedCommand[1];
                    boost::to_lower(inputName);
                    for (auto& orbiterType : OrbiterType::ORBITER_TYPES) {
                        std::string orbiterName = orbiterType->getName();
                        boost::to_lower(orbiterName);
                        if (orbiterName == inputName) {
                            orbiterId = orbiterType->getId();
                            foundOrbiter = true;
                            break;
                        }
                    }

                    if (foundOrbiter) {
                        Entity* player = _world->getPlayer().get();
                        std::shared_ptr<Orbiter> orbiter = std::shared_ptr<Orbiter>(new Orbiter(90.f, orbiterId, player));
                        orbiter->loadSprite(_world->getSpriteSheet());
                        orbiter->setWorld(_world);
                        _world->addEntity(orbiter);
                        return "Player given orbiter \"" + parsedCommand[1] + "\"";
                    } else {
                        return "No orbiter named " + parsedCommand[1];
                    }
                } else {
                    return "Not enough parameters for commmand: " + (std::string)("\"") + parsedCommand[0] + "\"";
                }
            })
        },

        {
            "bighp",
            Command("Give the player a bunch of HP",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                processCommand("setmaxhp:100000000");
                processCommand("addhp:100000000");
                return "Player given 100000000 hit points";
            })
        },

        {
            "toggleplayer",
            Command("Toggle player visibility",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                _world->getPlayer()->toggleVisible();
                return "Toggled player visibility";
            })
        },

        {
            "resetach",
            Command("Reset all achivements",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                AchievementManager::resetAchievements();
                return "Attempted achievement reset";
            })
        },

        {
            "giveability",
            Command("Give the player an ability",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                if (parsedCommand.size() == 2) {
                    for (const auto& ability : Ability::ABILITIES) {
                        std::string abilityName = ability->getName();
                        boost::to_lower(abilityName);
                        if (abilityName == parsedCommand[1]) {
                            AbilityManager::givePlayerAbility(ability->getId());
                            return "Player given ability \"" + parsedCommand[1] + "\"";
                        }
                    }

                    return "No ability named " + parsedCommand[1];
                } else {
                    return "Not enough parameters for commmand: " + (std::string)("\"") + parsedCommand[0] + "\"";
                }
            })
        },

        {
            "sap",
            Command("Set a parameter for an ability",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                if (parsedCommand.size() == 4) {
                    for (const auto& ability : Ability::ABILITIES) {
                        std::string abilityName = ability->getName();
                        boost::to_lower(abilityName);
                        if (abilityName == parsedCommand[1]) {
                            for (const auto& parameter : ability->getParameters()) {
                                std::string parameterName = parameter.first;
                                boost::to_lower(parameterName);
                                if (parameterName == parsedCommand[2]) {
                                    float value = 0.f;
                                    try {
                                        value = std::stof(parsedCommand[3]);
                                        if (AbilityManager::setParameter(ability->getId(), parameterName, value)) {
                                            return "Parameter \"" + parameterName + "\" set to " + std::to_string(value) + " for ability \"" + ability->getName() + "\"";
                                        } else {
                                            return "Player does not have ability \"" + ability->getName() + "\"";
                                        }
                                    } catch (std::exception ex) {
                                        return ex.what();
                                    }
                                }
                            }

                            return "No parameter named " + parsedCommand[2] + " for ability " + parsedCommand[1];
                        }
                    }

                    return "No ability named " + parsedCommand[1];
                } else {
                    return "Not enough parameters for commmand: " + (std::string)("\"") + parsedCommand[0] + "\"";
                }
            })
        },

        {
            "toggleculling",
            Command("Toggle frustum culling",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                Viewport::setCullingEnabled(!Viewport::isCullingEnabled());
                return (Viewport::isCullingEnabled() ? "Enabled" : "Disabled") + (std::string)" culling";
            })
        },

        {
            "addeffect",
            Command("Add an effect to the player",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                if (parsedCommand.size() == 2) {
                    const std::string effectName = parsedCommand[1];
                    PlayerVisualEffectManager::addEffectToPlayer(effectName);
                    return "Player given effect \"" + effectName + "\"";
                } else {
                    return "Not enough parameters for commmand: " + (std::string)("\"") + parsedCommand[0] + "\"";
                }
            })
        }
    };
};

#endif