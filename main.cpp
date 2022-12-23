#include "Game.h"
#include <iostream>
#include "Util.h"


int main() {
    float screenHeight = sf::VideoMode::getDesktopMode().height;
    float screenWidth = screenHeight * ((float)WIDTH / (float)HEIGHT);
    float relativeWindowSize = 0.75;

    unsigned int windowWidth = screenWidth * relativeWindowSize;
    unsigned int windowHeight = screenHeight * relativeWindowSize;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "");
    window.setFramerateLimit(60);

    sf::View camera(sf::Vector2f(0, 0), sf::Vector2f(WIDTH, HEIGHT));
    //camera.zoom(0.75);

    srand(currentTimeMillis());
    Game game(&camera);

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

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                game.keyPressed(event.key.code);
                break;
            case sf::Event::KeyReleased:
                game.keyReleased(event.key.code);
                break;
            }
        }

        game.update();

        mainSurface.setView(camera);

        mainSurface.clear();
        game.draw(mainSurface);

        mainSurface.display();

        uiSurface.clear(sf::Color::Transparent);
        game.drawUI(uiSurface);
        uiSurface.display();

        window.clear();
        window.draw(mainSurfaceSprite);
        window.draw(uiSurfaceSprite);
        window.display();
    }
}