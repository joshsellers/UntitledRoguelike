#include "Game.h"
#include <iostream>

Game::Game(sf::View* camera) {
    _camera = camera;
}

void Game::update() {
    _player.update();
    _world.update();
    _camera->setCenter(_player.getPosition());
}

void Game::draw(sf::RenderTexture& surface) {
    _world.draw(surface);
    _player.draw(surface);
}

void Game::drawUI(sf::RenderTexture& surface) {
}

void Game::keyPressed(sf::Keyboard::Key& key) {
}

void Game::keyReleased(sf::Keyboard::Key& key) {
}
