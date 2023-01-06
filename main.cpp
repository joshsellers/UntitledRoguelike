#include "Game.h"
#include <iostream>
#include "Util.h"


int main() {
    float screenHeight = sf::VideoMode::getDesktopMode().height;
    float screenWidth = screenHeight * ((float)WIDTH / (float)HEIGHT);

    unsigned int windowWidth = screenWidth * RELATIVE_WINDOW_SIZE;
    unsigned int windowHeight = screenHeight * RELATIVE_WINDOW_SIZE;
    WINDOW_WIDTH = windowWidth;
    WINDOW_HEIGHT = windowHeight;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "");
    window.setFramerateLimit(60);

    sf::View camera(sf::Vector2f(0, 0), sf::Vector2f(WIDTH, HEIGHT));

    srand(currentTimeMillis());
    Game game(&camera, &window);

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
            case sf::Event::MouseButtonPressed:
                game.mouseButtonPressed(
                    event.mouseButton.x, event.mouseButton.y, event.mouseButton.button
                );
                break;
            case sf::Event::MouseButtonReleased:
                game.mouseButtonReleased(
                    event.mouseButton.x, event.mouseButton.y, event.mouseButton.button
                );
                break;
            case sf::Event::MouseMoved:
                game.mouseMoved(
                    event.mouseMove.x, event.mouseMove.y
                );
                break;
            case sf::Event::MouseWheelScrolled:
                game.mouseWheelScrolled(event.mouseWheelScroll);
                break;
            case sf::Event::TextEntered:
                game.textEntered(event.text.unicode);
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