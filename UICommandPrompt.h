#ifndef _UI_COMMAND_PROMPT_H
#define _UI_COMMAND_PROMPT_H

#include "UIElement.h"
#include "World.h"
#include "Turtle.h"
#include "Penguin.h"
#include "PlantMan.h"
#include "Cactoid.h"
#include <regex>
#include <boost/algorithm/string.hpp>

constexpr bool LOCK_CMD_PROMPT = false;
constexpr const char UNLOCK_HASH[11] = "3491115221";

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
                    } else if (!std::regex_match(parsedCommand.at(1), std::regex("^[0-9]+$"))) {
                        const Item* item = nullptr;
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

                std::cout << processCommand("addhp:" + std::to_string(_world->getPlayer()->getMaxHitPoints())) << std::endl;
                _world->getPlayer()->activate();
                _world->addEntity(_world->getPlayer());
                return "Player respawned at full health";
            })
        },
        
        {
            "vibrate",
            Command("Cause the gamepad to vibrate",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                if (GameController::isConnected()) {
                    if (parsedCommand.size() > 2) {
                        try {
                            int vibrationAmount = std::stoi(parsedCommand[1]);
                            long long time = std::stoll(parsedCommand[2]);

                            GameController::vibrate(vibrationAmount, time);
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
            "hash",
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
            "help",
            Command("List all commands with descriptions",
            [this](std::vector<std::string>& parsedCommand)->std::string {
                std::string output = "";
                for (auto& command : _commands) {
                    output += command.first + " - " + command.second.description + "\n";
                }
                return output;
            })
        }
    };
};

#endif