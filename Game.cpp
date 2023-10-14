#include "Game.h"
#include <iostream>
#include "UICommandPrompt.h"
#include "UIAttributeMeter.h"

Game::Game(sf::View* camera, sf::RenderWindow* window) : 
    _player(std::shared_ptr<Player>(new Player(sf::Vector2f(0, 0), window, _isPaused))), _world(World(_player, _showDebug)) {
    _camera = camera;
    _window = window;

    if (!_font.loadFromFile("font.ttf")) {
        std::cout << "Failed to load font!" << std::endl;
    }
    _versionLabel.setFont(_font);
    _versionLabel.setCharacterSize(24);
    _versionLabel.setString("v" + VERSION);
    _versionLabel.setPosition(0, 0);

    _fpsLabel.setFont(_font);
    _fpsLabel.setCharacterSize(24);
    _fpsLabel.setString("0 fps");
    _fpsLabel.setPosition(0, 25);

    _activeChunksLabel.setFont(_font);
    _activeChunksLabel.setCharacterSize(24);
    _activeChunksLabel.setString("0 active chunks");
    _activeChunksLabel.setPosition(0, 50);

    _entityCountLabel.setFont(_font);
    _entityCountLabel.setCharacterSize(24);
    _entityCountLabel.setString("0 entities");
    _entityCountLabel.setPosition(0, 75);

    _seedLabel.setFont(_font);
    _seedLabel.setCharacterSize(24);
    _seedLabel.setString("seed: " + std::to_string(_world.getSeed()));
    _seedLabel.setPosition(0, 100);

    _playerPosLabel.setFont(_font);
    _playerPosLabel.setCharacterSize(24);
    _playerPosLabel.setString("x: 0, y: 0");
    _playerPosLabel.setPosition(0, 125);

    _spriteSheet->create(128, 208);
    if (!_spriteSheet->loadFromFile("res/sprite_sheet.png")) {
        std::cout << "failed to load sprite sheet" << std::endl;
    }

    _player->loadSprite(_spriteSheet);
    _world.loadSpriteSheet(_spriteSheet);

    initUI();
}

void Game::initUI() {
    // Pause menu
    std::shared_ptr<UIButton> exitButton = std::shared_ptr<UIButton>(new UIButton(
        1, 5, 9, 3, "quit game", _font, this, "exit"
    ));
    _pauseMenu->addElement(exitButton);
    _ui.addMenu(_pauseMenu);

    // Inventory menu
    std::shared_ptr<UIInventoryInterface> inventoryInterface = std::shared_ptr<UIInventoryInterface>(new UIInventoryInterface(
        _player->getInventory(), _font, _spriteSheet
    ));
    _inventoryMenu->addElement(inventoryInterface);
    _ui.addMenu(_inventoryMenu);

    // Command prompt menu
    std::shared_ptr<UICommandPrompt> cmdPrompt = std::shared_ptr<UICommandPrompt>(new UICommandPrompt(&_world, _font));
    _commandMenu->addElement(cmdPrompt);
    _ui.addMenu(_commandMenu);

    // HUD
    std::shared_ptr<UIAttributeMeter> playerHpMeter = std::shared_ptr<UIAttributeMeter>(new UIAttributeMeter(
        "HP", 50, 93, 24, 1.5f, _player->getHitPointsRef(), _player->getMaxHitPointsRef(), _font
    ));
    playerHpMeter->setColor(0xCC0000FF);
    playerHpMeter->setBackgroundColor(0xAA0000FF);
    _HUDMenu->addElement(playerHpMeter);
    _ui.addMenu(_HUDMenu);
    _HUDMenu->show();
}

void Game::update() {
    float leftStickXAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
    float leftStickYAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
    float rightStickXAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::U);
    float rightStickYAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::V);

    _ui.update();
    if (!_isPaused) _world.update();
    _camera->setCenter(_player->getPosition().x + (float)PLAYER_WIDTH / 2, _player->getPosition().y + (float)PLAYER_HEIGHT / 2);
}

void Game::draw(sf::RenderTexture& surface) {
    _world.draw(surface);
}

void Game::drawUI(sf::RenderTexture& surface) {
    _ui.draw(surface);

    if (_showDebug) {
        surface.draw(_versionLabel);

        float fps = 1.f / _clock.restart().asSeconds();
        if (_frameCounter >= 30) {
            _fpsLabel.setString(std::to_string((int)fps) + " fps");
            _frameCounter = 0;
        }

        surface.draw(_fpsLabel);

        int chunkCount = _world.getActiveChunkCount();
        _activeChunksLabel.setString(std::to_string(chunkCount) + " active chunk" + (chunkCount > 1 ? "s" : ""));
        surface.draw(_activeChunksLabel);
        
        int entityCount = _world.getEntities().size();
        _entityCountLabel.setString(std::to_string(entityCount) + " entit" + (entityCount > 1 ? "ies" : "y"));
        surface.draw(_entityCountLabel);

        _seedLabel.setString("seed: " + std::to_string(_world.getSeed()));
        surface.draw(_seedLabel);

        _playerPosLabel.setString("x: " + std::to_string(_player->getPosition().x) + ", y: " + std::to_string(_player->getPosition().y));
        surface.draw(_playerPosLabel);

        _frameCounter++;
    }
}

void Game::buttonPressed(std::string buttonCode) {
    if (buttonCode == "exit") {
        _window->close();
    }
}

void Game::keyPressed(sf::Keyboard::Key& key) {
    _ui.keyPressed(key);
}

void Game::keyReleased(sf::Keyboard::Key& key) {
    switch (key) {
    case sf::Keyboard::F3:
        _showDebug = !_showDebug;
        break;
    case sf::Keyboard::F10:
        if (_commandMenu->isActive()) {
            _commandMenu->hide();
            _isPaused = false;
            _pauseMenu->hide();
        } else {
            _commandMenu->show();
            _isPaused = true;
        }
        break;
    case sf::Keyboard::Enter:
        if (_commandMenu->isActive()) {
            _commandMenu->hide();
            _isPaused = false;
        }
        break;
    case sf::Keyboard::Hyphen:
        if (!_commandMenu->isActive()) _camera->zoom(2);
        break;
    case sf::Keyboard::Equal:
        if (!_commandMenu->isActive()) _camera->zoom(0.5);
        break;
    case sf::Keyboard::Escape:
        if (!_commandMenu->isActive()) {
            if (_pauseMenu->isActive()) _pauseMenu->hide();
            else _pauseMenu->show();
            _isPaused = !_isPaused;
        }
        break;
    case sf::Keyboard::I:
        if (!_commandMenu->isActive()) {
            if (_inventoryMenu->isActive()) _inventoryMenu->hide();
            else _inventoryMenu->show();
        }
        break;
    }

    _ui.keyReleased(key);
    _player->keyReleased(key);
}

void Game::mouseButtonPressed(const int mx, const int my, const int button) {
    _ui.mouseButtonPressed(mx, my, button);
}

void Game::mouseButtonReleased(const int mx, const int my, const int button) {
    _ui.mouseButtonReleased(mx, my, button);
    if (!_inventoryMenu->isActive()) _player->mouseButtonReleased(mx, my, button);
}

void Game::mouseMoved(const int mx, const int my) {
    _ui.mouseMoved(mx, my);
}

void Game::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
    _ui.mouseWheelScrolled(mouseWheelScroll);
}

void Game::textEntered(sf::Uint32 character) {
    _ui.textEntered(character);
}
