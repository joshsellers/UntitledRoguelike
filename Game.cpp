#include "Game.h"
#include <iostream>
#include "UITextField.h"
#include <regex>
#include "UIMessageDisplay.h"
#include "MessageManager.h"
#include "../SteamworksHeaders/steam_api.h"
#include "RemotePlayer.h"
#include <filesystem>
#include <fstream>

Game::Game(sf::View* camera, sf::RenderWindow* window) : 
    _player(std::shared_ptr<Player>(new Player(sf::Vector2f(0, 0), window, _isPaused))), _world(World(_player, _showDebug)) {
    _camera = camera;
    _window = window;

    if (!_font.loadFromFile("res/font.ttf")) {
        MessageManager::displayMessage("Failed to load font!", 10, WARN);
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

    _onEnemySpawnCooldownLabel.setFont(_font);
    _onEnemySpawnCooldownLabel.setCharacterSize(24);
    _onEnemySpawnCooldownLabel.setString("enemy spawn cooldown expired");
    _onEnemySpawnCooldownLabel.setPosition(0, 100);

    _seedLabel.setFont(_font);
    _seedLabel.setCharacterSize(24);
    _seedLabel.setString("seed: " + std::to_string(_world.getSeed()));
    _seedLabel.setPosition(0, 125);

    _playerPosLabel.setFont(_font);
    _playerPosLabel.setCharacterSize(24);
    _playerPosLabel.setString("x: 0, y: 0");
    _playerPosLabel.setPosition(0, 150);

    _currentBiomeLabel.setFont(_font);
    _currentBiomeLabel.setCharacterSize(24);
    _currentBiomeLabel.setString("biome: ");
    _currentBiomeLabel.setPosition(0, 175);


    _spriteSheet->create(128, 208);
    if (!_spriteSheet->loadFromFile("res/sprite_sheet.png")) {
        MessageManager::displayMessage("Failed to load sprite sheet!", 10, WARN);
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

    
    // Settings menu (from pause menu)
    std::shared_ptr<UIButton> backSettingsMenuButton = std::shared_ptr<UIButton>(new UIButton(
        1, 5, 9, 3, "back", _font, this, "back_pausesettings"
    ));
    backSettingsMenuButton->setSelectionId(0);
    _pauseMenu_settings->addElement(backSettingsMenuButton);

    std::shared_ptr<UIButton> togglefullscreenButton = std::shared_ptr<UIButton>(new UIButton(
        1, 11, 28, 3, "toggle fullscreen (requires restart)", _font, this, "togglefullscreen"
    ));
    togglefullscreenButton->setSelectionId(1);
    _pauseMenu_settings->addElement(togglefullscreenButton);

    _pauseMenu_settings->useGamepadConfiguration = true;
    _pauseMenu_settings->defineSelectionGrid(
        {
            {backSettingsMenuButton->getSelectionId()},
            {togglefullscreenButton->getSelectionId()}
        }
    );
    _ui->addMenu(_pauseMenu_settings);


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
        45, 39, 9, 3, "new game", _font, this, "newgame"
    ));
    newGameButton->setSelectionId(0);
    _startMenu->addElement(newGameButton);

    std::shared_ptr<UIButton> joinGameButton = std::shared_ptr<UIButton>(new UIButton(
        45, 46, 9, 3, "join game", _font, this, "joingame"
    ));
    joinGameButton->setSelectionId(1);
    _startMenu->addElement(joinGameButton);

    std::shared_ptr<UIButton> settingsButton_mainMenu = std::shared_ptr<UIButton>(new UIButton(
        45, 53, 9, 3, "settings", _font, this, "settings_mainmenu"
    ));
    settingsButton_mainMenu->setSelectionId(2);
    _startMenu->addElement(settingsButton_mainMenu);
    
    std::shared_ptr<UIButton> exitGameButton = std::shared_ptr<UIButton>(new UIButton(
        45, 60, 9, 3, "exit game", _font, this, "exit"
    ));
    exitGameButton->setSelectionId(3);
    _startMenu->addElement(exitGameButton);

    _startMenu->useGamepadConfiguration = true;
    _startMenu->defineSelectionGrid(
        {
            {newGameButton->getSelectionId()},
            {joinGameButton->getSelectionId()},
            {settingsButton_mainMenu->getSelectionId()},
            {exitGameButton->getSelectionId()}
        }
    );
    _ui->addMenu(_startMenu);
    _startMenu->show();

    
    // Settings menu (from start menu)
    std::shared_ptr<UIButton> backButton_startSettings = std::shared_ptr<UIButton>(new UIButton(
        45, 43, 9, 3, "back", _font, this, "back_startsettings"
    ));
    backButton_startSettings->setSelectionId(0);
    _startMenu_settings->addElement(backButton_startSettings);

    std::shared_ptr<UIButton> togglefullscreenButton_fromstart = std::shared_ptr<UIButton>(new UIButton(
        36, 50, 28, 3, "toggle fullscreen (requires restart)", _font, this, "togglefullscreen"
    ));
    togglefullscreenButton_fromstart->setSelectionId(1);
    _startMenu_settings->addElement(togglefullscreenButton_fromstart);

    _startMenu_settings->useGamepadConfiguration = true;
    _startMenu_settings->defineSelectionGrid(
        {
            {backButton_startSettings->getSelectionId()},
            {togglefullscreenButton_fromstart->getSelectionId()}
        }
    );
    _ui->addMenu(_startMenu_settings);


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


    // Join game menu
    _steamNameField = std::shared_ptr<UITextField>(new UITextField(
        "Enter a friend's Steam name:", 49.5, 48, _font
    ));
    _steamNameField->setSelectionId(0);
    _joinGameMenu->addElement(_steamNameField);

    std::shared_ptr<UIButton> joinButton = std::shared_ptr<UIButton>(new UIButton(
        45, 55, 9, 3, "join", _font, this, "join"
    ));
    joinButton->setSelectionId(1);
    _joinGameMenu->addElement(joinButton);

    std::shared_ptr<UIButton> joinGameBackButton = std::shared_ptr<UIButton>(new UIButton(
        45, 61, 9, 3, "back", _font, this, "back_joingame"
    ));
    joinGameBackButton->setSelectionId(2);
    _joinGameMenu->addElement(joinGameBackButton);
    
    _joinGameMenu->useGamepadConfiguration = true;
    _joinGameMenu->defineSelectionGrid(
        {
            {_steamNameField->getSelectionId()},
            {joinButton->getSelectionId()},
            {joinGameBackButton->getSelectionId()}
        }
    );
    _ui->addMenu(_joinGameMenu);


    // Message Display Menu
    std::shared_ptr<UIMessageDisplay> messageDisp = std::shared_ptr<UIMessageDisplay>(new UIMessageDisplay(
        _font
    ));
    _messageDispMenu->addElement(messageDisp);
    _ui->addMenu(_messageDispMenu);
    _messageDispMenu->show();
}

void Game::sendMultiplayerUpdates() {
    if (_connectedAsClient || IS_MULTIPLAYER_CONNECTED) {
        // this packets out limiting stuff is garbage, figure out a better way to do this
        if (currentTimeMillis() - _lastPacketsOutCountReset >= 1000) {
            _packetsOutThisTick = 0;
            _lastPacketsOutCountReset = currentTimeMillis();
        }

        if (_packetsOutThisTick >= MAX_PACKETS_OUT_PER_SECOND) return;

        for (auto& peer : Multiplayer::manager.getConnectedPeers()) {
            if (_player->_isActuallyMoving) {
                std::string playerPos = std::to_string(
                    _player->getPosition().x) + (std::string)","
                    + std::to_string(_player->getPosition().y) + (std::string)","
                    + std::to_string(_player->isDodging()) + (std::string)","
                    + std::to_string(_player->_multiplayerAimAngle);
                Multiplayer::manager.sendMessage(MultiplayerMessage(PayloadType::PLAYER_DATA, playerPos), peer);
            } else if (_player->_isHoldingWeapon) {
                Multiplayer::manager.sendMessage(MultiplayerMessage(PayloadType::PLAYER_AIM_ANGLE, std::to_string(_player->_multiplayerAimAngle)), peer);
            }
        }

        _packetsOutThisTick++;
    }
}

void Game::update() {
    if (STEAMAPI_INITIATED && SteamAPI_IsSteamRunning()) {
        SteamAPI_RunCallbacks();

        Multiplayer::manager.recieveMessages();
        sendMultiplayerUpdates();
    }

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

        bool spawnCooldown = _world.onEnemySpawnCooldown();
        _onEnemySpawnCooldownLabel.setString((spawnCooldown ? "enemy spawn cooldown active" : "enemy spawn cooldown expired"));
        surface.draw(_onEnemySpawnCooldownLabel);

        if (_textSeed == "NONE") _seedLabel.setString("seed: " + std::to_string(_world.getSeed()));
        else _seedLabel.setString("seed: " + std::to_string(_world.getSeed()) + " (" + _textSeed + ")");
        surface.draw(_seedLabel);

        _playerPosLabel.setString("x: " + std::to_string(_player->getPosition().x) + ", y: " + std::to_string(_player->getPosition().y));
        surface.draw(_playerPosLabel);

        TERRAIN_TYPE currentBiome = _world.getTerrainDataAt(_world.getCurrentChunk(), _player->getPosition());
        _currentBiomeLabel.setString("current biome: " + std::to_string((int)currentBiome));
        surface.draw(_currentBiomeLabel);

        _frameCounter++;
    }
}

void Game::buttonPressed(std::string buttonCode) {
    if (buttonCode == "exit") {
        if (_connectedAsClient || IS_MULTIPLAYER_CONNECTED) {
            for (auto& peer : Multiplayer::manager.getConnectedPeers()) {
                Multiplayer::manager.sendMessage(MultiplayerMessage(PayloadType::PEER_DISCONNECT, "DISCONNECT"), peer);
            }
            sf::sleep(sf::seconds(1));
            disconnectMultiplayer();
        }
        Multiplayer::manager.halt();
        SteamAPI_Shutdown();
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
            MessageManager::displayMessage(ex.what(), 10, ERR);
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
    } else if (buttonCode == "mainmenu" || buttonCode == "mainmenu_clientdisc") {
        if (buttonCode != "mainmenu_clientdisc" && (_connectedAsClient || IS_MULTIPLAYER_CONNECTED)) {
            for (auto& peer : Multiplayer::manager.getConnectedPeers()) {
                Multiplayer::manager.sendMessage(MultiplayerMessage(PayloadType::PEER_DISCONNECT, "DISCONNECT"), peer);
            }
            sf::sleep(sf::seconds(1));
            disconnectMultiplayer();
        }

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
    } else if (buttonCode == "joingame") {
        _startMenu->hide();
        _joinGameMenu->show();
    } else if (buttonCode == "join") {
        if (STEAMAPI_INITIATED) {
            std::string userName = _steamNameField->getText();

            SteamNetworkingIdentity sni;
            ISteamFriends* m_pFriends;

            bool bFoundFriend = false;
            m_pFriends = SteamFriends();

            memset(&sni, 0, sizeof(SteamNetworkingIdentity));
            sni.m_eType = k_ESteamNetworkingIdentityType_SteamID;

            int nFriendCount = m_pFriends->GetFriendCount(k_EFriendFlagAll);
            for (int nIndex = 0; nIndex < nFriendCount; ++nIndex) {
                CSteamID csFriendId = m_pFriends->GetFriendByIndex(nIndex, k_EFriendFlagAll);
                std::string sFriendName = m_pFriends->GetFriendPersonaName(csFriendId);
                if (sFriendName == userName) {
                    sni.SetSteamID(csFriendId);
                    bFoundFriend = true;
                    break;
                }
            }

            if (!bFoundFriend) {
                MessageManager::displayMessage("Could not find any Steam friends named \"" + userName + "\"", 5, NORMAL);
            } else {
                MultiplayerMessage message(PayloadType::JOIN_REQUEST, userName);
                Multiplayer::manager.sendMessage(message, sni);
            }
        } else {
            MessageManager::displayMessage("Steam is not connected", 5, NORMAL);
        }
    } else if (buttonCode == "back_joingame") {
        _joinGameMenu->hide();
        _startMenu->show();
    } else if (buttonCode == "settings") {
        _pauseMenu->hide();
        _pauseMenu_settings->show();
    } else if (buttonCode == "back_pausesettings") {
        _pauseMenu_settings->hide();
        _pauseMenu->show();
    } else if (buttonCode == "togglefullscreen") {
        std::string fileName = "settings.config";
        try {
            if (!std::filesystem::remove(fileName))
                MessageManager::displayMessage("Could not replace settings file", 5, ERR);
        } catch (const std::filesystem::filesystem_error& err) {
            MessageManager::displayMessage("Could not replace settings file: " + (std::string)err.what(), 5, ERR);
        }

        try {
            std::ofstream out(fileName);
            int fullscreenSetting = FULLSCREEN ? 0 : 1;
            out << "fullscreen=" << std::to_string(fullscreenSetting) << std::endl;
            out.close();

            MessageManager::displayMessage("The game will launch in " + (std::string)(fullscreenSetting == 1 ? "fullscreen" : "windowed") + " mode next time", 5);
        } catch (std::exception ex) {
            MessageManager::displayMessage("Error writing to settings file: " + (std::string)ex.what(), 5, ERR);
        }

    } else if (buttonCode == "settings_mainmenu") {
        _startMenu->hide();
        _startMenu_settings->show();
    } else if (buttonCode == "back_startsettings") {
        _startMenu_settings->hide();
        _startMenu->show();
    }
}

void Game::messageReceived(MultiplayerMessage message, SteamNetworkingIdentity identityPeer) {
    switch (message.payloadType) {
        case PayloadType::JOIN_REQUEST:
            if (_gameStarted) {
                Multiplayer::manager.sendMessage(MultiplayerMessage(PayloadType::WORLD_SEED, std::to_string(_world.getSeed())), identityPeer);
                IS_MULTIPLAYER_CONNECTED = true;
            } else {
                std::string reason = "Game not started yet";
                Multiplayer::manager.sendMessage(MultiplayerMessage(PayloadType::JOIN_REJECT, reason), identityPeer);
            }
            break;
        case PayloadType::JOIN_REJECT:
            MessageManager::displayMessage("Join request rejected. Reason: " + message.data, 5, NORMAL);
            break;
        case PayloadType::PEER_DISCONNECT:
            MessageManager::displayMessage(Multiplayer::getPeerSteamName(identityPeer) + " has disconnected", 5, NORMAL);
            if (_connectedAsClient) {
                buttonPressed("mainmenu_clientdisc");
            }
            disconnectMultiplayer();
            break;
        case PayloadType::WORLD_SEED:
        {
            unsigned int seed = std::stoul(message.data);
            _world.init(seed);
            _joinGameMenu->hide();
            _HUDMenu->show();
            _magazineMeter->hide();
            _gameStarted = true;
            _connectedAsClient = true;
            IS_MULTIPLAYER_CONNECTED = true;

            std::string playerPos = std::to_string(_player->getPosition().x) + "," + std::to_string(_player->getPosition().y);
            Multiplayer::manager.sendMessage(MultiplayerMessage(PayloadType::PLAYER_DATA, playerPos), identityPeer);
        }
            break;
        case PayloadType::PLAYER_DATA:
            for (auto& connectedPeer : Multiplayer::manager.getConnectedPeers()) {
                if (connectedPeer == identityPeer) return;
            }
            Multiplayer::manager.addConnectedPeer(identityPeer);

            std::vector<std::string> parsedData = splitString(message.data, ",");
            float x = std::stof(parsedData[0]);
            float y = std::stof(parsedData[1]);
            std::shared_ptr<RemotePlayer> remotePlayer = std::shared_ptr<RemotePlayer>(new RemotePlayer(identityPeer, sf::Vector2f(x, y), _window, _isPaused));
            Multiplayer::manager.addListener(remotePlayer);
            remotePlayer->loadSprite(_spriteSheet);
            remotePlayer->setWorld(&_world);
            _world.addEntity(remotePlayer);
            break;
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

void Game::disconnectMultiplayer() {
    for (auto& peer : Multiplayer::manager.getConnectedPeers()) {
        SteamNetworkingMessages()->CloseSessionWithUser(peer);
    }
    Multiplayer::manager.clearPeers();
    Multiplayer::manager.clearListeners();
    _connectedAsClient = false;
    IS_MULTIPLAYER_CONNECTED = false;
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

        _player->_inventoryMenuIsOpen = _inventoryMenu->isActive();

        // this causes non-critical visual problems that i would like to not have happen
        //_isPaused = _inventoryMenu->isActive();
    }
}

void Game::textEntered(sf::Uint32 character) {
    _ui->textEntered(character);
}
