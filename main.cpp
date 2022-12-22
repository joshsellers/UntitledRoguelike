#include "Game.h"
#include <iostream>


int main() {
    unsigned int windowWidth = WIDTH * SCALE;
    unsigned int windowHeight = HEIGHT * SCALE;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "");
    window.setFramerateLimit(60);

    sf::View camera(sf::Vector2f(0, 0), sf::Vector2f(WIDTH, HEIGHT));

    Game game(&camera);

    sf::Event event;

    sf::RenderTexture mainSurface;
    mainSurface.create(WIDTH, HEIGHT);
    const sf::Texture& mainSurfaceTexture = mainSurface.getTexture();

    sf::Sprite mainSurfaceSprite;
    mainSurfaceSprite.setTexture(mainSurfaceTexture);
    mainSurfaceSprite.setScale(SCALE, SCALE);

    sf::RenderTexture uiSurface;
    uiSurface.create(windowWidth, windowHeight);
    const sf::Texture& uiSurfaceTexture = uiSurface.getTexture();

    sf::Sprite uiSurfaceSprite;
    uiSurfaceSprite.setTexture(uiSurfaceTexture);

    sf::CircleShape shape(20);
    shape.setPosition(40, 0);
    shape.setFillColor(sf::Color(0x0000FFFF));

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
        mainSurface.draw(shape); // TEMP
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