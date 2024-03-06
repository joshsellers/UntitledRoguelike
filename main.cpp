#include "Game.h"
#include <iostream>
#include "Util.h"
#include "GameController.h"
#include "SoundManager.h"
#include "../SteamworksHeaders/steam_api.h"

int main() {
    MessageManager::start();
    SoundManager::loadSounds();

    // Steam
    SteamAPI_Init();

    if (SteamAPI_RestartAppIfNecessary(480)) {
        MessageManager::displayMessage("Steam did not connect", 10, WARN);
    }
    //

    if (FULLSCREEN) {
        HEIGHT = (float)WIDTH / ((float)sf::VideoMode::getDesktopMode().width / (float)sf::VideoMode::getDesktopMode().height);
    }

    float screenHeight = sf::VideoMode::getDesktopMode().height; 
    float screenWidth = FULLSCREEN ? sf::VideoMode::getDesktopMode().width : screenHeight * ((float)WIDTH / (float)HEIGHT);

    unsigned int windowWidth = screenWidth * RELATIVE_WINDOW_SIZE;
    unsigned int windowHeight = screenHeight * RELATIVE_WINDOW_SIZE;
    WINDOW_WIDTH = windowWidth;
    WINDOW_HEIGHT = windowHeight;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "", FULLSCREEN ? sf::Style::Fullscreen : sf::Style::Default);
    window.setFramerateLimit(60);
    //window.setVerticalSyncEnabled();

    sf::View camera(sf::Vector2f(0, 0), sf::Vector2f(WIDTH, HEIGHT));

    srand(currentTimeMillis());
    
    std::shared_ptr<Game> game = std::shared_ptr<Game>(new Game(&camera, &window));
    GameController::addListener(game);

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
    if (controllerId != - 1) GameController::setControllerId(controllerId);
    MessageManager::displayMessage("Controller is " + (std::string)(controllerConnected ? "" : "not ") + "connected", 0);
    MessageManager::displayMessage("Controller id: " + std::to_string(controllerId), 0);

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                SteamAPI_Shutdown();
                window.close();
                break;
            case sf::Event::KeyPressed:
                game->keyPressed(event.key.code);
                break;
            case sf::Event::KeyReleased:
                if (event.key.code == sf::Keyboard::F11) {
                    window.close();

                    FULLSCREEN = true;
                    RELATIVE_WINDOW_SIZE = FULLSCREEN ? 1 : 0.75;
                    if (FULLSCREEN) {
                        HEIGHT = (float)WIDTH / ((float)sf::VideoMode::getDesktopMode().width / (float)sf::VideoMode::getDesktopMode().height);
                    }
                    float screenHeight1 = sf::VideoMode::getDesktopMode().height;
                    float screenWidth1 = FULLSCREEN ? sf::VideoMode::getDesktopMode().width : screenHeight1 * ((float)WIDTH / (float)HEIGHT);

                    unsigned int windowWidth1 = screenWidth1 * RELATIVE_WINDOW_SIZE;
                    unsigned int windowHeight1 = screenHeight1 * RELATIVE_WINDOW_SIZE;
                    WINDOW_WIDTH = windowWidth1;
                    WINDOW_HEIGHT = windowHeight1;

                    window.create(sf::VideoMode(windowWidth1, windowHeight1), "", FULLSCREEN ? sf::Style::Fullscreen : sf::Style::Default);
                    window.setFramerateLimit(60);

                    camera.setSize(sf::Vector2f(WIDTH, HEIGHT));

                    mainSurface.create(WIDTH, HEIGHT);
                    const sf::Texture& mainSurfaceTexture1 = mainSurface.getTexture();
                    mainSurfaceSprite.setTexture(mainSurfaceTexture1);
                    mainSurfaceSprite.setScale((float)windowWidth1 / (float)WIDTH, (float)windowHeight1 / (float)HEIGHT);
                    uiSurface.create(windowWidth1, windowHeight1);
                    const sf::Texture& uiSurfaceTexture1 = uiSurface.getTexture();
                    uiSurfaceSprite.setTexture(uiSurfaceTexture1);
                    //Might have to make a way to like reinitialize each UIElement
                    break;
                }

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
                GameController::receiveControllerEvent(event);
                break;
            case sf::Event::JoystickDisconnected:
                GameController::receiveControllerEvent(event);
                break;
            case sf::Event::JoystickMoved:
                window.setMouseCursorVisible(false);
                GameController::receiveControllerEvent(event);
                break;
            case sf::Event::JoystickButtonReleased:
                window.setMouseCursorVisible(false);
                GameController::receiveControllerEvent(event);
                break;
            case sf::Event::JoystickButtonPressed:
                window.setMouseCursorVisible(false);
                GameController::receiveControllerEvent(event);
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
}