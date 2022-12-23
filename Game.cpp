#include "Game.h"
#include <iostream>

Game::Game(sf::View* camera) {
    _camera = camera;

    if (!_font.loadFromFile("font.ttf")) {
        std::cout << "Failed to load font!" << std::endl;
    }
    _fpsLabel.setFont(_font);
    _fpsLabel.setCharacterSize(24);
    _fpsLabel.setString("0 fps");
    _fpsLabel.setPosition(0, 0);

    _activeChunksLabel.setFont(_font);
    _activeChunksLabel.setCharacterSize(24);
    _activeChunksLabel.setString("0 active chunks");
    _activeChunksLabel.setPosition(0, 25);
}

void Game::update() {
    _player.update();
    _world.update();
    _camera->setCenter(_player.getPosition().x + 5, _player.getPosition().y + 5);
}

void Game::draw(sf::RenderTexture& surface) {
    _world.draw(surface);
    _player.draw(surface);
}

void Game::drawUI(sf::RenderTexture& surface) {
    float fps = 1.f / _clock.restart().asSeconds();
    if (_frameCounter >= 30) {
        _fpsLabel.setString(std::to_string((int)fps) + " fps");
        _frameCounter = 0;
    }

    surface.draw(_fpsLabel);

    int chunkCount = _world.getActiveChunkCount();
    _activeChunksLabel.setString(std::to_string(chunkCount) + " active chunk" + (chunkCount > 1 ? "s" : ""));
    surface.draw(_activeChunksLabel);

    _frameCounter++;
}

void Game::keyPressed(sf::Keyboard::Key& key) {
}

void Game::keyReleased(sf::Keyboard::Key& key) {
    if (key == sf::Keyboard::Hyphen) {
        _camera->zoom(2);
    } else if (key == sf::Keyboard::Equal) {
        _camera->zoom(0.5);
    }
}
