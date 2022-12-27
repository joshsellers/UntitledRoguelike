#include "Game.h"
#include <iostream>

Game::Game(sf::View* camera, sf::RenderWindow* window) {
    _camera = camera;
    _window = window;

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

    _spriteSheet->create(128, 208);
    if (!_spriteSheet->loadFromFile("res/url_guy_walking-Sheet.png")) {
        std::cout << "failed to load sprite sheet" << std::endl;
    }

    _player->loadSprite(_spriteSheet);
    _world.loadSpriteSheet(_spriteSheet);

    initUI();
}

void Game::initUI() {
    std::shared_ptr<UIButton> exitButton = std::shared_ptr<UIButton>(new UIButton(
        1, 5, 9, 3, "quit game", _font, this, "exit"
    ));
    _pauseMenu->addElement(exitButton);
    _ui.addMenu(_pauseMenu);
}

void Game::update() {
    _ui.update();
    if (!_isPaused) _world.update();
    _camera->setCenter(_player->getPosition().x + (float)PLAYER_WIDTH / 2, _player->getPosition().y + (float)PLAYER_HEIGHT / 2);
}

void Game::draw(sf::RenderTexture& surface) {
    _world.draw(surface);
}

void Game::drawUI(sf::RenderTexture& surface) {
    _ui.draw(surface);

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

void Game::buttonPressed(std::string buttonCode) {
    if (buttonCode == "exit") {
        _window->close();
    }
}

void Game::keyPressed(sf::Keyboard::Key& key) {
}

void Game::keyReleased(sf::Keyboard::Key& key) {
    switch (key) {
    case sf::Keyboard::Hyphen:
        _camera->zoom(2);
        break;
    case sf::Keyboard::Equal:
        _camera->zoom(0.5);
        break;
    case sf::Keyboard::Escape:
        if (_pauseMenu->isActive()) _pauseMenu->hide();
        else _pauseMenu->show();
        _isPaused = !_isPaused;
        break;
    }
}

void Game::mouseButtonPressed(const int mx, const int my) {
    _ui.mouseButtonPressed(mx, my);
}

void Game::mouseButtonReleased(const int mx, const int my) {
    _ui.mouseButtonReleased(mx, my);
}

void Game::mouseMoved(const int mx, const int my) {
    _ui.mouseMoved(mx, my);
}

void Game::textEntered(sf::Uint32 character) {
    _ui.textEntered(character);
}
