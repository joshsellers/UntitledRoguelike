#include "UICommandPrompt.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "PlantMan.h"
#include "Util.h"
#include "Turtle.h"
#include "Penguin.h"

UICommandPrompt::UICommandPrompt(World* world, sf::Font font) : _world(world),
    UIElement(30, 30, 4, 4, false, false, font) {
    _disableAutomaticTextAlignment = true;

    _bg.setSize(sf::Vector2f(_width, _height));
    _bg.setPosition(sf::Vector2f(_x, _y));
    _bg.setFillColor(sf::Color(0x2D2D30FF));
    _bg.setOutlineColor(sf::Color(0x3E3E41FF));
    _bg.setOutlineThickness(getRelativeWidth(0.25f));

    float fontSize = 1;
    int relativeFontSize = getRelativeWidth(fontSize);
    _text.setFont(_font);
    _text.setCharacterSize(relativeFontSize);
    _text.setFillColor(sf::Color(0x00AA22FF));
}

void UICommandPrompt::update() {

}

void UICommandPrompt::draw(sf::RenderTexture& surface) {
    float width = _text.getGlobalBounds().width;
    float height = _text.getGlobalBounds().height;

    float x = getRelativeWidth(50.f) - width / 2;
    float y = getRelativeWidth(50.f);
    _text.setPosition(sf::Vector2f(x, y));

    float padding = getRelativeWidth(1.f);
    _bg.setPosition(sf::Vector2f(x - padding, y - padding));
    _bg.setSize(sf::Vector2f(width + padding * 2, height + padding * 2));

    surface.draw(_bg);
    surface.draw(_text);
}

void UICommandPrompt::mouseButtonPressed(const int mx, const int my, const int button) {
}

void UICommandPrompt::mouseButtonReleased(const int mx, const int my, const int button) {
}

void UICommandPrompt::mouseMoved(const int mx, const int my) {
}

void UICommandPrompt::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
}

void UICommandPrompt::keyReleased(sf::Keyboard::Key& key) {
    switch (key) {
        case sf::Keyboard::Up:
            if (_historyIndex > 0) {
                _historyIndex--;
            }
            break;
        case sf::Keyboard::Down:
            if (_historyIndex < _history.size() - 1) {
                _historyIndex++;
            }
            break;
        default:
            return;
    }

    if (_history.size() && _historyIndex < _history.size()) _text.setString(_history.at(_historyIndex));
}

void UICommandPrompt::textEntered(const sf::Uint32 character) {
    sf::String userInput = _text.getString();
    if (character != 13) {
        if (character == '\b' && userInput.getSize()) {
            userInput.erase(userInput.getSize() - 1, 1);
        } else if (character != '\b') {
            userInput += character;
        }
        _text.setString(userInput);
    } else {
        for (int i = 0; i < _history.size(); i++) {
            if (_history.at(i) == userInput) {
                _history.erase(_history.begin() + i);
                break;
            }
        }
        if (userInput != "") _history.push_back(userInput);
        _historyIndex = _history.size();
        std::cout << processCommand(userInput) << std::endl;
        _text.setString("");
    }
}

std::string UICommandPrompt::processCommand(sf::String commandInput) {
    std::string command = (std::string)commandInput;
    boost::to_lower(command);

    size_t pos = 0;
    std::vector<std::string> parsedCommand;
    std::string delimiter = ":";
    while ((pos = command.find(delimiter)) != std::string::npos) {
        parsedCommand.push_back(command.substr(0, pos));
        command.erase(0, pos + delimiter.length());
    }
    parsedCommand.push_back(command);

    std::string commandHeader = parsedCommand.at(0);
    if (commandHeader == "give") {
        if (parsedCommand.size() > 1) {
            if (parsedCommand.at(1) == "tux") {
                for (int id = Item::TOP_HAT.getId(); id < Item::DRESS_SHOES.getId() + 1; id++)
                    _world->getPlayer()->getInventory().addItem(id, 1);
                return "Player given tuxedo";
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
            return "Not enough parameters for command: " + (std::string)("\"") + commandHeader + "\"";
        }
    } else if (commandHeader == "tgm") {
        _world->getPlayer()->freeMove = !_world->getPlayer()->freeMove;
        if (_world->getPlayer()->freeMove) _world->getPlayer()->setBaseSpeed(8);
        else _world->getPlayer()->setBaseSpeed(BASE_PLAYER_SPEED);
        return "Godmode set to " + (std::string)(_world->getPlayer()->freeMove ? "true" : "false");
    } else if (commandHeader == "addhp") {
        if (parsedCommand.size() > 1) {
            try {
                int dHp = stoi(parsedCommand.at(1));
                _world->getPlayer()->heal(dHp);
                return "Player's HP set to " + std::to_string(_world->getPlayer()->getHitPoints());
            } catch (std::exception ex) {
                return ex.what();
            }
        } else {
            return "Not enough parameters for command: " + (std::string)("\"") + commandHeader + "\"";
        }
    } else if (commandHeader == "setmaxhp") {
        if (parsedCommand.size() > 1) {
            try {
                int mhp = stoi(parsedCommand.at(1));
                _world->getPlayer()->setMaxHitPoints(mhp);
                return "Player's max HP set to " + std::to_string(_world->getPlayer()->getMaxHitPoints());
            } catch (std::exception ex) {
                return ex.what();
            }
        } else {
            return "Not enough parameters for command: " + (std::string)("\"") + commandHeader + "\"";
        }
    } else if (commandHeader == "killall") {
        for (auto& entity : _world->getEntities()) {
            entity->deactivate();
        }
        _world->getPlayer()->activate();
        return "Set all entities to inactive";
    } else if (commandHeader == "reseed") {
        if (parsedCommand.size() > 1) {
            try {
                unsigned int seed = stoul(parsedCommand.at(1));
                _world->reseed(seed);
                return "World seed set to " + std::to_string(_world->getSeed());
            } catch (std::exception ex) {
                return ex.what();
            }
        } else {
            return "Not enough parameters for command: " + (std::string)("\"") + commandHeader + "\"";
        }
    } else if (commandHeader == "summon") {
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
                } else {
                    return entityName + " is not a valid entity name";
                }

                entity->loadSprite(_world->getSpriteSheet());
                entity->setWorld(_world);
                _world->addEntity(entity);
            }
            return "Spawned " + std::to_string(amt) + " " + entityName + (amt > 1 ? "s" : "");
        } else {
            return "Not enough parameters for command: " + (std::string)("\"") + commandHeader + "\"";
        }
    } else {
        return "Command not recognized: " + (std::string)("\"") + commandHeader + "\"";
    }

    return "If you're reading this, josh made a mistake";
}
