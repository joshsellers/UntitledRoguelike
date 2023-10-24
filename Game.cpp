#include "Game.h"
#include <iostream>
#include "UITextField.h"
#include <regex>

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

    GameController::addListener(_player);
    GameController::addListener(_ui);

    initUI();
}

void Game::initUI() {
    // Pause menu
    std::shared_ptr<UIButton> mainMenuButton = std::shared_ptr<UIButton>(new UIButton(
        1, 5, 9, 3, "main menu", _font, this, "mainmenu"
    ));
    mainMenuButton->setSelectionId(0);
    _pauseMenu->addElement(mainMenuButton);

    std::shared_ptr<UIButton> exitButton = std::shared_ptr<UIButton>(new UIButton(
        1, 11, 9, 3, "exit game", _font, this, "exit"
    ));
    exitButton->setSelectionId(1);
    _pauseMenu->addElement(exitButton);

    std::shared_ptr<UIButton> saveButton = std::shared_ptr<UIButton>(new UIButton(
        1, 17, 9, 3, "save game", _font, this, "save"
    ));
    saveButton->setSelectionId(2);
    _pauseMenu->addElement(saveButton);

    std::shared_ptr<UIButton> settingsButton = std::shared_ptr<UIButton>(new UIButton(
        1, 23, 9, 3, "settings", _font, this, "settings"
    ));
    settingsButton->setSelectionId(3);
    _pauseMenu->addElement(settingsButton);

    _pauseMenu->useGamepadConfiguration = true;
    _pauseMenu->defineSelectionGrid(
        {
            {mainMenuButton->getSelectionId()},
            {exitButton->getSelectionId()},
            {saveButton->getSelectionId()},
            {settingsButton->getSelectionId()}
        }
    );
    _ui->addMenu(_pauseMenu);

    // Command prompt menu
    _cmdPrompt = std::shared_ptr<UICommandPrompt>(new UICommandPrompt(&_world, _font));
    _commandMenu->addElement(_cmdPrompt);
    _ui->addMenu(_commandMenu);

    // HUD
    std::shared_ptr<UIAttributeMeter> playerHpMeter = std::shared_ptr<UIAttributeMeter>(new UIAttributeMeter(
        "HP", 50, 93, 24, 1.5f, _player->getHitPointsRef(), _player->getMaxHitPointsRef(), _font
    ));
    playerHpMeter->setColor(0xCC0000FF);
    playerHpMeter->setBackgroundColor(0xAA0000FF);
    _HUDMenu->addElement(playerHpMeter);

    _magazineMeter = std::shared_ptr<UIAttributeMeter>(new UIAttributeMeter(
        "", 90, 92, 6, 3.f, _player->getMagazineContents(), _player->getMagazineSize(), _font
    ));
    _magazineMeter->setBackgroundColor(0x55555599);
    _magazineMeter->setColor(0x99999999);
    _magazineMeter->useDefaultLabel(false);
    _magazineMeter->setCharacterSize(2);
    _HUDMenu->addElement(_magazineMeter);
    _ui->addMenu(_HUDMenu);

    // Inventory menu
    std::shared_ptr<UIInventoryInterface> inventoryInterface = std::shared_ptr<UIInventoryInterface>(new UIInventoryInterface(
        _player->getInventory(), _font, _spriteSheet
    ));
    _inventoryMenu->addElement(inventoryInterface);
    _ui->addMenu(_inventoryMenu);

    // Start menu
    std::shared_ptr<UIButton> newGameButton = std::shared_ptr<UIButton>(new UIButton(
        45, 50, 9, 3, "new game", _font, this, "newgame"
    ));
    newGameButton->setSelectionId(0);
    _startMenu->addElement(newGameButton);
    
    std::shared_ptr<UIButton> exitGameButton = std::shared_ptr<UIButton>(new UIButton(
        45, 57, 9, 3, "exit game", _font, this, "exit"
    ));
    exitGameButton->setSelectionId(1);
    _startMenu->addElement(exitGameButton);

    _startMenu->useGamepadConfiguration = true;
    _startMenu->defineSelectionGrid(
        {
            {newGameButton->getSelectionId()},
            {exitGameButton->getSelectionId()}
        }
    );
    _ui->addMenu(_startMenu);
    _startMenu->show();

    // New game menu
    _worldNameField = std::shared_ptr<UITextField>(new UITextField(
        "world name:", 49.5, 37, _font
    ));
    _worldNameField->setDefaultText("My World");
    _worldNameField->setSelectionId(0);
    _newGameMenu->addElement(_worldNameField);

    _seedField = std::shared_ptr<UITextField>(new UITextField(
        "seed:", 49.5, 48, _font
    ));
    _seedField->setDefaultText(std::to_string((unsigned int)currentTimeMillis()));
    _seedField->setSelectionId(1);
    _newGameMenu->addElement(_seedField);

    std::shared_ptr<UIButton> startGameButton = std::shared_ptr<UIButton>(new UIButton(
        45, 55, 9, 3, "start", _font, this, "startnewgame"
    ));
    startGameButton->setSelectionId(2);
    _newGameMenu->addElement(startGameButton);

    std::shared_ptr<UIButton> newGameBackButton = std::shared_ptr<UIButton>(new UIButton(
        45, 61, 9, 3, "back", _font, this, "back_newgame"
    ));
    newGameBackButton->setSelectionId(3);
    _newGameMenu->addElement(newGameBackButton);
    
    _newGameMenu->useGamepadConfiguration = true;
    _newGameMenu->defineSelectionGrid(
        {
            {_worldNameField->getSelectionId()},
            {_seedField->getSelectionId()},
            {startGameButton->getSelectionId()},
            {newGameBackButton->getSelectionId()}
        }
    );
    _ui->addMenu(_newGameMenu);
}

void Game::update() {
    float leftStickXAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
    float leftStickYAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
    float rightStickXAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::U);
    float rightStickYAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::V);

    _ui->update();
    if (!_isPaused && _gameStarted) {
        _world.update();

        int equippedTool = _player->getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL);
        if (equippedTool != NOTHING_EQUIPPED && Item::ITEMS[equippedTool]->isGun()) {
            int ammoIndex = _player->getInventory().getEquippedIndex(EQUIPMENT_TYPE::AMMO);
            int ammoCount = ammoIndex == NOTHING_EQUIPPED ? 0 : _player->getInventory().getItemAmountAt(ammoIndex);

            _magazineMeter->show();
            if (ammoIndex != NOTHING_EQUIPPED && _player->getInventory().getItemIdAt(ammoIndex) == Item::ITEMS[equippedTool]->getAmmoId()) {
                _magazineMeter->setText(sf::String(std::to_string(_player->getMagazineContents()) + ":" + std::to_string(ammoCount)));
            } else {
                _magazineMeter->setText(sf::String(std::to_string(_player->getMagazineContents()) + ":0"));
            }
        } else {
            _magazineMeter->hide();
        }
    }
    _camera->setCenter(_player->getPosition().x + (float)PLAYER_WIDTH / 2, _player->getPosition().y + (float)PLAYER_HEIGHT / 2);
}

void Game::draw(sf::RenderTexture& surface) {
    if (_gameStarted) _world.draw(surface);
}

void Game::drawUI(sf::RenderTexture& surface) {
    if (!_gameStarted) {
        sf::RectangleShape startMenuBg;
        startMenuBg.setFillColor(sf::Color(0x444448FF));
        //startMenuBg.setOutlineColor(sf::Color(0xFFFFFFFF));
        //startMenuBg.setOutlineThickness(-10);
        startMenuBg.setPosition(0, 0);
        startMenuBg.setSize(sf::Vector2f(surface.getSize().x, surface.getSize().y));
        surface.draw(startMenuBg);
    }

    _ui->draw(surface);

    if (_showDebug) {
        surface.draw(_versionLabel);

        float fps = 1.f / _clock.restart().asSeconds();
        if (_frameCounter >= 30) {
            _fpsLabel.setString(std::to_string((int)fps) + " fps");
            _frameCounter = 0;
        }

        surface.draw(_fpsLabel);

        int chunkCount = _world.getActiveChunkCount();
        _activeChunksLabel.setString(std::to_string(chunkCount) + " active chunk" + (chunkCount != 1 ? "s" : ""));
        surface.draw(_activeChunksLabel);
        
        int entityCount = _world.getEntities().size();
        _entityCountLabel.setString(std::to_string(entityCount) + " entit" + (entityCount > 1 ? "ies" : "y"));
        surface.draw(_entityCountLabel);

        if (_textSeed == "NONE") _seedLabel.setString("seed: " + std::to_string(_world.getSeed()));
        else _seedLabel.setString("seed: " + std::to_string(_world.getSeed()) + " (" + _textSeed + ")");
        surface.draw(_seedLabel);

        _playerPosLabel.setString("x: " + std::to_string(_player->getPosition().x) + ", y: " + std::to_string(_player->getPosition().y));
        surface.draw(_playerPosLabel);

        _frameCounter++;
    }
}

void Game::buttonPressed(std::string buttonCode) {
    if (buttonCode == "exit") {
        _window->close();
    } else if (buttonCode == "newgame") {
        _newGameMenu->show();
        _startMenu->hide();
    } else if (buttonCode == "startnewgame") {
        std::string seedText = _seedField->getText();
        unsigned int seed = 0;
        try {
            if (!std::regex_match(seedText, std::regex("^[0-9]+$"))) {
                _textSeed = seedText;
                for (int i = 0; i < seedText.length(); i++) {
                    seed = ((seed << 5) + seed) + (int)seedText.at(i);
                }
            } else seed = std::stoul(seedText);
        } catch (std::exception ex) {
            std::cout << ex.what() << std::endl;
            seed = currentTimeMillis();
        }

        _world.init(seed);
        _newGameMenu->hide();
        _HUDMenu->show();
        _magazineMeter->hide();
        _gameStarted = true;
    } else if (buttonCode == "back_newgame") {
        _newGameMenu->hide();
        _startMenu->show();
    } else if (buttonCode == "mainmenu") {
        _gameStarted = false;
        _isPaused = false;
        _pauseMenu->hide();
        _HUDMenu->hide();

        _cmdPrompt->unlock();
        _cmdPrompt->processCommand("killall");
        _cmdPrompt->processCommand("clear inventory");
        _cmdPrompt->processCommand("respawn");
        _cmdPrompt->processCommand("setmaxhp:100");
        _cmdPrompt->processCommand("addhp:100");
        if (LOCK_CMD_PROMPT) _cmdPrompt->lock();

        _player->_pos = sf::Vector2f(0, 0);
        _player->_movingDir = DOWN;
        _player->_facingDir = DOWN;
        _player->_isDodging = false;
        _player->_dodgeTimer = 0;
        _player->_maxDodgeTime = 10;
        _player->_dodgeSpeedMultiplier = 1.f;
        _player->_dodgeKeyReleased = true;
        _player->_magazineSize = 0;
        _player->_magazineContents = 0;
        _player->_baseSpeed = BASE_PLAYER_SPEED;

        _world.resetChunks();

        _worldNameField->setDefaultText("My World");
        _seedField->setDefaultText(std::to_string((unsigned int)currentTimeMillis()));
        _textSeed = "NONE";
        _startMenu->show();
    }
}

void Game::keyPressed(sf::Keyboard::Key& key) {
    _ui->keyPressed(key);
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
        togglePauseMenu();
        break;
    case sf::Keyboard::I:
        toggleInventoryMenu();
        break;
    }

    _ui->keyReleased(key);
    _player->keyReleased(key);
}

void Game::mouseButtonPressed(const int mx, const int my, const int button) {
    _ui->mouseButtonPressed(mx, my, button);
}

void Game::mouseButtonReleased(const int mx, const int my, const int button) {
    _ui->mouseButtonReleased(mx, my, button);
    if (!_inventoryMenu->isActive()) _player->mouseButtonReleased(mx, my, button);
}

void Game::mouseMoved(const int mx, const int my) {
    _ui->mouseMoved(mx, my);
}

void Game::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
    _ui->mouseWheelScrolled(mouseWheelScroll);
}

void Game::controllerButtonPressed(CONTROLLER_BUTTON button) {
}

void Game::controllerButtonReleased(CONTROLLER_BUTTON button) {
    switch (button) {
        case CONTROLLER_BUTTON::START:
            togglePauseMenu();
            break;
        case CONTROLLER_BUTTON::SELECT:
            toggleInventoryMenu();
            break;
    }
}

void Game::togglePauseMenu() {
    if (_gameStarted && !_commandMenu->isActive() && !_inventoryMenu->isActive()) {
        if (_pauseMenu->isActive()) _pauseMenu->hide();
        else _pauseMenu->show();
        _isPaused = !_isPaused;
    }
}

void Game::toggleInventoryMenu() {
    if (_gameStarted && !_commandMenu->isActive()) {
        if (_inventoryMenu->isActive()) _inventoryMenu->hide();
        else _inventoryMenu->show();

        // this causes non-critical visual problems that i would like to not have happen
        //_isPaused = _inventoryMenu->isActive();
    }
}

void Game::textEntered(sf::Uint32 character) {
    _ui->textEntered(character);
}
