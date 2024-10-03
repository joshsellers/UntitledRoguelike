#include "Game.h"
#include <iostream>
#include "Util.h"
#include "gamepad/GamePad.h"
#include "SoundManager.h"
#include <fstream>
#include "Tutorial.h"
#include "ShaderManager.h"
#include "InputBindings.h"
#include "Globals.h"
#include "../statistics/StatManager.h"
#include "../../SteamworksHeaders/steam_api.h"
#include "Versioning.h"
#include "../statistics/AchievementManager.h"
#include "CrashDetector.h"
#include "music/MusicManager.h"
#include "../world/entities/projectiles/ProjectilePoolManager.h"
#include "../world/TerrainGenParameters.h"
#include "../mod/ModManager.h"

#ifndef DBGBLD
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

void readSettings(std::ifstream& in) {
    std::string line;
    while (getline(in, line)) {
        if (line.rfind("fullscreen", 0) == 0) {
            try {
                std::vector<std::string> parsedLine = splitString(line, "=");
                FULLSCREEN = (bool)std::stoi(parsedLine[1]);
                RELATIVE_WINDOW_SIZE = FULLSCREEN ? 1 : 0.75;
            } catch (std::exception ex) {
                MessageManager::displayMessage(ex.what(), 5, ERR);
            }
        } else if (line.rfind("tutorial", 0) == 0) {
            try {
                std::vector<std::string> parsedLine = splitString(line, "=");
                bool tutorialCompleted = (bool)std::stoi(parsedLine[1]);
                if (tutorialCompleted) Tutorial::completeStep(TUTORIAL_STEP::END);
            } catch (std::exception ex) {
                MessageManager::displayMessage(ex.what(), 5, ERR);
            }
        } else if (line.rfind("vsync", 0) == 0) {
            try {
                std::vector<std::string> parsedLine = splitString(line, "=");
                bool vsyncEnabled = (bool)std::stoi(parsedLine[1]);
                VSYNC_ENABLED = vsyncEnabled;
            } catch (std::exception ex) {
                MessageManager::displayMessage(ex.what(), 5, ERR);
            }
        }
    }
}

void loadSettings() {
    std::string localLowPath = getLocalLowPath() + "\\settings.config";
    std::ifstream in(localLowPath);
    if (!in.good()) {
        MessageManager::displayMessage("Could not find LocalLow settings file", 5, DEBUG);
        in.close();

        std::ifstream steamDirIn("settings.config");
        if (!steamDirIn.good()) {
            MessageManager::displayMessage("Could not find steam directory settings file", 5, WARN);
            steamDirIn.close();
            return;
        } else {
            readSettings(steamDirIn);
        }
        steamDirIn.close();
        return;
    } else {
        readSettings(in);
    }

    in.close();
}

void checkLocalLowExists() {
    std::string localLowPath = getLocalLowPath();
    if (localLowPath == "err") {
        MessageManager::displayMessage("getLocalLowPath returned \"err\"", 5, WARN);
        return;
    }
    if (!std::filesystem::is_directory(localLowPath + "\\")) {
        std::filesystem::create_directories(localLowPath);
        MessageManager::displayMessage("Created LocalLow directory", 5, DEBUG);
    }
}

void shutdown() {
    SteamAPI_Shutdown();
    ProjectilePoolManager::shutdown();
    MusicManager::shutdown();
    SoundManager::shutdown();
    MessageManager::stop();
    Logger::log("SHUTDOWN");
    while (!Logger::queuesHaveFlushed()) {
        sf::sleep(sf::milliseconds(500));
    }
    Logger::stop();
}

void steamworksSetup() {
    /*if (SteamAPI_RestartAppIfNecessary(480)) {
        MessageManager::displayMessage("Steam did not connect", 5, DEBUG);
        shutdown();
        exit(0);
    }*/

    STEAMAPI_INITIATED = SteamAPI_Init();

    MessageManager::displayMessage("STEAMAPI_INITIATED: " + (std::string)(STEAMAPI_INITIATED ? "true" : "false"), 5, DEBUG);
}

void checkCrash() {
    CrashData crashData = CrashDetector::checkForCrash();

    if (crashData.probableCrash) {
        MessageManager::displayMessage("Crash detected", 5, DEBUG);
        if (crashData.autoSaveTimeString != "NONE" && crashData.saveFileName != "NONE") {
            MessageManager::displayMessage("It looks like pennylooter did not shut down\ncorrectly last time.", 15);
            MessageManager::displayMessage(
                "Your world named \"" + crashData.saveFileName + "\" was last autosaved on " + crashData.autoSaveTimeString, 15
            );
        } else {
            MessageManager::displayMessage(
                "It looks like pennylooter did not shut down\ncorrectly last time.\n\nYou can report bugs on instagram (dm @pennylooter)\nor email rolmigame@gmail.com"
                , 15);
        }
    }
}

int main() {
    checkCrash();
    Logger::start();
    Logger::log("v" + VERSION + " (" + BUILD_NUMBER + ")");
    MessageManager::start();
    ModManager::loadAll();
    InputBindingManager::init();
    SoundManager::loadSounds();
    ShaderManager::compileShaders();
    ShaderManager::configureShaders();
    StatManager::loadOverallStats();
    MusicManager::start();
    MusicManager::setSituation(MUSIC_SITUTAION::MAIN_MENU);
    ProjectilePoolManager::init();

    if (!TerrainGenInitializer::loadParameters()) {
        MessageManager::displayMessage("Terrain generation parameters did not load", 5, WARN);
    }

    steamworksSetup();
    AchievementManager::start();

    Item::checkForIncompleteItemConfigs();

    checkLocalLowExists();

    if (WIDTH % 16 != 0) MessageManager::displayMessage("WIDTH % 16 != 0", 5, DEBUG);
    if (HEIGHT % 16 != 0) MessageManager::displayMessage("HEIGHT % 16 != 0", 5, DEBUG);

    loadSettings();

    if (FULLSCREEN) {
        HEIGHT = (float)WIDTH / ((float)sf::VideoMode::getDesktopMode().width / (float)sf::VideoMode::getDesktopMode().height);
    }

    float screenHeight = sf::VideoMode::getDesktopMode().height; 
    float screenWidth = FULLSCREEN ? sf::VideoMode::getDesktopMode().width : screenHeight * ((float)WIDTH / (float)HEIGHT);

    unsigned int windowWidth = screenWidth * RELATIVE_WINDOW_SIZE;
    unsigned int windowHeight = screenHeight * RELATIVE_WINDOW_SIZE;
    WINDOW_WIDTH = windowWidth;
    WINDOW_HEIGHT = windowHeight;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), GAME_NAME, FULLSCREEN ? sf::Style::Fullscreen : sf::Style::Default);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(VSYNC_ENABLED);

    // Set icon
    sf::Image icon;
    if (!icon.loadFromFile("res/icon0.png")) MessageManager::displayMessage("Could not load icon", 5, WARN);
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    //

    sf::View camera(sf::Vector2f(0, 0), sf::Vector2f(WIDTH, HEIGHT));

    srand(currentTimeMillis());
    
    std::shared_ptr<Game> game = std::shared_ptr<Game>(new Game(&camera, &window));
    GamePad::addListener(game);

    sf::Event event;

    sf::RenderTexture mainSurface;
    mainSurface.create(WIDTH, HEIGHT);
    const sf::Texture& mainSurfaceTexture = mainSurface.getTexture();

    sf::Sprite mainSurfaceSprite;
    mainSurfaceSprite.setTexture(mainSurfaceTexture);
    mainSurfaceSprite.setScale((float)windowWidth / (float)WIDTH, (float)windowHeight / (float)HEIGHT);

    sf::RenderTexture uiSurface;
    uiSurface.create(windowWidth, windowHeight);
    const sf::Texture& uiSurfaceTexture = uiSurface.getTexture();

    sf::Sprite uiSurfaceSprite;
    uiSurfaceSprite.setTexture(uiSurfaceTexture);

    // Controller
    bool controllerConnected = false;
    int controllerId = -1;
    for (int i = 0; i < 7; i++) {
        if (sf::Joystick::isConnected(i)) {
            controllerConnected = true;
            controllerId = i;
            break;
        }
    }
    if (controllerId != - 1) GamePad::setControllerId(controllerId);
    MessageManager::displayMessage("Controller is " + (std::string)(controllerConnected ? "" : "not ") + "connected", 0);
    MessageManager::displayMessage("Controller id: " + std::to_string(controllerId), 0);

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                game->keyPressed(event.key.code);
                break;
            case sf::Event::KeyReleased:
                game->keyReleased(event.key.code);
                break;
            case sf::Event::MouseButtonPressed:
                game->mouseButtonPressed(
                    event.mouseButton.x, event.mouseButton.y, event.mouseButton.button
                );
                break;
            case sf::Event::MouseButtonReleased:
                game->mouseButtonReleased(
                    event.mouseButton.x, event.mouseButton.y, event.mouseButton.button
                );
                break;
            case sf::Event::MouseMoved:
                window.setMouseCursorVisible(true);
                USING_MOUSE = true;
                game->mouseMoved(
                    event.mouseMove.x, event.mouseMove.y
                );
                break;
            case sf::Event::MouseWheelScrolled:
                game->mouseWheelScrolled(event.mouseWheelScroll);
                break;
            case sf::Event::TextEntered:
                game->textEntered(event.text.unicode);
                break;
            case sf::Event::JoystickConnected:
                GamePad::receiveControllerEvent(event);
                break;
            case sf::Event::JoystickDisconnected:
                GamePad::receiveControllerEvent(event);
                break;
            case sf::Event::JoystickMoved:
                window.setMouseCursorVisible(false);
                USING_MOUSE = false;
                GamePad::receiveControllerEvent(event);
                break;
            case sf::Event::JoystickButtonReleased:
                window.setMouseCursorVisible(false);
                USING_MOUSE = false;
                GamePad::receiveControllerEvent(event);
                break;
            case sf::Event::JoystickButtonPressed:
                window.setMouseCursorVisible(false);
                USING_MOUSE = false;
                GamePad::receiveControllerEvent(event);
                break;
            }
        }

        game->update();

        mainSurface.setView(camera);

        mainSurface.clear();
        game->draw(mainSurface);

        mainSurface.display();

        uiSurface.clear(sf::Color::Transparent);
        game->drawUI(uiSurface);
        uiSurface.display();

        window.clear();
        window.draw(mainSurfaceSprite);
        window.draw(uiSurfaceSprite);
        window.display();
    }

    shutdown();
}