#include "Game.h"
#include <iostream>
#include <regex>
#include "MessageManager.h"
#include <filesystem>
#include <fstream>
#include "SaveManager.h"
#include "../ui/UIMessageDisplay.h"
#include "../world/entities/DroppedItem.h"
#include "../ui/UILabel.h"
#include "Tutorial.h"

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

    _enemyCountLabel.setFont(_font);
    _enemyCountLabel.setCharacterSize(24);
    _enemyCountLabel.setString("0 enemies");
    _enemyCountLabel.setPosition(0, 100);

    _onEnemySpawnCooldownLabel.setFont(_font);
    _onEnemySpawnCooldownLabel.setCharacterSize(24);
    _onEnemySpawnCooldownLabel.setString("enemy spawn cooldown expired");
    _onEnemySpawnCooldownLabel.setPosition(0, 125);

    _seedLabel.setFont(_font);
    _seedLabel.setCharacterSize(24);
    _seedLabel.setString("seed: " + std::to_string(_world.getSeed()));
    _seedLabel.setPosition(0, 150);

    _playerPosLabel.setFont(_font);
    _playerPosLabel.setCharacterSize(24);
    _playerPosLabel.setString("x: 0, y: 0");
    _playerPosLabel.setPosition(0, 175);

    _currentBiomeLabel.setFont(_font);
    _currentBiomeLabel.setCharacterSize(24);
    _currentBiomeLabel.setString("biome: ");
    _currentBiomeLabel.setPosition(0, 200);

    _coinMagnetCountLabel.setFont(_font);
    _coinMagnetCountLabel.setCharacterSize(24);
    _coinMagnetCountLabel.setString("magnets: ");
    _coinMagnetCountLabel.setPosition(0, 225);


    _spriteSheet->create(128, 208);
    if (!_spriteSheet->loadFromFile("res/sprite_sheet.png")) {
        MessageManager::displayMessage("Failed to load sprite sheet!", 10, WARN);
    }

    _player->loadSprite(_spriteSheet);
    _world.loadSpriteSheet(_spriteSheet);

    _shopKeep = std::shared_ptr<ShopKeep>(new ShopKeep(sf::Vector2f(0, 0), &_shopManager, _spriteSheet));
    _world.setShopKeep(_shopKeep);

    _shopArrow.setWorld(&_world);
    _shopArrow.loadSprite(_spriteSheet);

    GamePad::addListener(_player);
    GamePad::addListener(_ui);

    SaveManager::init(&_world, &_shopManager);

    initUI();
}

void Game::initUI() {
    // Load game menu
    _ui->addMenu(_loadGameMenu);


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


    // HUD
    std::shared_ptr<UIAttributeMeter> playerHpMeter = std::shared_ptr<UIAttributeMeter>(new UIAttributeMeter(
        "HP", 50, 92, 24, 1.5f, _player->getHitPointsRef(), _player->getMaxHitPointsRef(), _font
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
    _magazineMeter->fitWidthToText(true);
    _magazineMeter->setCharacterSize(2);
    _HUDMenu->addElement(_magazineMeter);

    _staminaMeter = std::shared_ptr<UIAttributeMeter>(new UIAttributeMeter(
        "STAMINA", 50.25f, 87, 18, 1.3f, _player->getStaminaRef(), _player->getMaxStaminaRef(), _font
    ));
    _staminaMeter->setColor(0x00CC00FF);
    _staminaMeter->setBackgroundColor(0x00AA00FF);
    _HUDMenu->addElement(_staminaMeter);

    int placeholder = 0;
    _waveCounterMeter = std::shared_ptr<UIAttributeMeter>(new UIAttributeMeter(
        "", 50.f, 97.f, 14.f, 0.75f, placeholder, placeholder, _font
    ));
    _waveCounterMeter->setBackgroundColor(0x555555BB);
    _waveCounterMeter->setColor(0x999999BB);
    _waveCounterMeter->useDefaultLabel(false);
    _waveCounterMeter->useAttributes(false);
    _waveCounterMeter->fitWidthToText(true);
    _HUDMenu->addElement(_waveCounterMeter);

    _ui->addMenu(_HUDMenu);


    // Command prompt menu
    _cmdPrompt = std::shared_ptr<UICommandPrompt>(new UICommandPrompt(&_world, _font));
    _commandMenu->addElement(_cmdPrompt);
    _ui->addMenu(_commandMenu);


    // Inventory menu
    std::shared_ptr<UIInventoryInterface> inventoryInterface = std::shared_ptr<UIInventoryInterface>(new UIInventoryInterface(
        _player->getInventory(), _font, _spriteSheet
    ));

    std::shared_ptr<UIButton> filterNoneButton = std::shared_ptr<UIButton>(new UIButton(
        26, 10, 5, 3, "all", _font, inventoryInterface.get(), "filter_none"
    ));
    filterNoneButton->pressWhenSelected = true;
    filterNoneButton->setSelectionId(0);
    _inventoryMenu->addElement(filterNoneButton);

    std::shared_ptr<UIButton> filterApparelButton = std::shared_ptr<UIButton>(new UIButton(
        26, 16, 6, 3, "apparel", _font, inventoryInterface.get(), "filter_apparel"
    ));
    filterApparelButton->pressWhenSelected = true;
    filterApparelButton->setSelectionId(1);
    _inventoryMenu->addElement(filterApparelButton);

    std::shared_ptr<UIButton> filterWeaponsButton = std::shared_ptr<UIButton>(new UIButton(
        26, 22, 7, 3, "weapons", _font, inventoryInterface.get(), "filter_weapons"
    ));
    filterWeaponsButton->pressWhenSelected = true;
    filterWeaponsButton->setSelectionId(2);
    _inventoryMenu->addElement(filterWeaponsButton);

    std::shared_ptr<UIButton> filterAmmoButton = std::shared_ptr<UIButton>(new UIButton(
        26, 28, 5, 3, "ammo", _font, inventoryInterface.get(), "filter_ammo"
    ));
    filterAmmoButton->pressWhenSelected = true;
    filterAmmoButton->setSelectionId(3);
    _inventoryMenu->addElement(filterAmmoButton);

    std::shared_ptr<UIButton> filterMiscButton = std::shared_ptr <UIButton>(new UIButton(
        26, 34, 5, 3, "misc", _font, inventoryInterface.get(), "filter_misc"
    ));
    filterMiscButton->pressWhenSelected = true;
    filterMiscButton->setSelectionId(4);
    _inventoryMenu->addElement(filterMiscButton);
    
    _inventoryMenu->useGamepadConfiguration = true;
    _inventoryMenu->defineSelectionGrid(
        {
            {0, 1, 2, 3, 4}
        }
    );

    _inventoryMenu->addElement(inventoryInterface);
    _ui->addMenu(_inventoryMenu);


    // Shop menu
    // Shopkeep's UI
    std::shared_ptr<UIShopInterface>buyInterface = std::shared_ptr<UIShopInterface>(new UIShopInterface(
        _shopManager, true, _shopKeep->getInventory(), _font, _spriteSheet
    ));

    std::shared_ptr<UIButton> shop_filterNoneButton = std::shared_ptr<UIButton>(new UIButton(
        26, 30, 5, 3, "all", _font, buyInterface.get(), "filter_none"
    ));
    shop_filterNoneButton->pressWhenSelected = true;
    shop_filterNoneButton->setSelectionId(0);
    _shopMenu->addElement(shop_filterNoneButton);

    std::shared_ptr<UIButton> shop_filterApparelButton = std::shared_ptr<UIButton>(new UIButton(
        26, 36, 6, 3, "apparel", _font, buyInterface.get(), "filter_apparel"
    ));
    shop_filterApparelButton->pressWhenSelected = true;
    shop_filterApparelButton->setSelectionId(1);
    _shopMenu->addElement(shop_filterApparelButton);

    std::shared_ptr<UIButton> shop_filterWeaponsButton = std::shared_ptr<UIButton>(new UIButton(
        26, 42, 7, 3, "weapons", _font, buyInterface.get(), "filter_weapons"
    ));
    shop_filterWeaponsButton->pressWhenSelected = true;
    shop_filterWeaponsButton->setSelectionId(2);
    _shopMenu->addElement(shop_filterWeaponsButton);

    std::shared_ptr<UIButton> shop_filterAmmoButton = std::shared_ptr<UIButton>(new UIButton(
        26, 48, 5, 3, "ammo", _font, buyInterface.get(), "filter_ammo"
    ));
    shop_filterAmmoButton->pressWhenSelected = true;
    shop_filterAmmoButton->setSelectionId(3);
    _shopMenu->addElement(shop_filterAmmoButton);

    std::shared_ptr<UIButton> shop_filterMiscButton = std::shared_ptr <UIButton>(new UIButton(
        26, 54, 5, 3, "misc", _font, buyInterface.get(), "filter_misc"
    ));
    shop_filterMiscButton->pressWhenSelected = true;
    shop_filterMiscButton->setSelectionId(4);
    _shopMenu->addElement(shop_filterMiscButton);

    _shopMenu->addElement(buyInterface);

    // Player's UI
    std::shared_ptr<UIShopInterface>sellInterface = std::shared_ptr<UIShopInterface>(new UIShopInterface(
        _shopManager, false, _player->getInventory(), _font, _spriteSheet
    ));

    std::shared_ptr<UIButton> sellshop_filterNoneButton = std::shared_ptr<UIButton>(new UIButton(
        71 - 2.25, 30, 5, 3, "all", _font, sellInterface.get(), "filter_none"
    ));
    sellshop_filterNoneButton->pressWhenSelected = true;
    sellshop_filterNoneButton->setSelectionId(0);
    _shopMenu->addElement(sellshop_filterNoneButton);

    std::shared_ptr<UIButton> sellshop_filterApparelButton = std::shared_ptr<UIButton>(new UIButton(
        70 - 2.25, 36, 6, 3, "apparel", _font, sellInterface.get(), "filter_apparel"
    ));
    sellshop_filterApparelButton->pressWhenSelected = true;
    sellshop_filterApparelButton->setSelectionId(1);
    _shopMenu->addElement(sellshop_filterApparelButton);

    std::shared_ptr<UIButton> sellshop_filterWeaponsButton = std::shared_ptr<UIButton>(new UIButton(
        69 - 2.25, 42, 7, 3, "weapons", _font, sellInterface.get(), "filter_weapons"
    ));
    sellshop_filterWeaponsButton->pressWhenSelected = true;
    sellshop_filterWeaponsButton->setSelectionId(2);
    _shopMenu->addElement(sellshop_filterWeaponsButton);

    std::shared_ptr<UIButton> sellshop_filterAmmoButton = std::shared_ptr<UIButton>(new UIButton(
        71 - 2.25, 48, 5, 3, "ammo", _font, sellInterface.get(), "filter_ammo"
    ));
    sellshop_filterAmmoButton->pressWhenSelected = true;
    sellshop_filterAmmoButton->setSelectionId(3);
    _shopMenu->addElement(sellshop_filterAmmoButton);

    std::shared_ptr<UIButton> sellshop_filterMiscButton = std::shared_ptr <UIButton>(new UIButton(
        71 - 2.25, 54, 5, 3, "misc", _font, sellInterface.get(), "filter_misc"
    ));
    sellshop_filterMiscButton->pressWhenSelected = true;
    sellshop_filterMiscButton->setSelectionId(4);
    _shopMenu->addElement(sellshop_filterMiscButton);

    _shopMenu->useGamepadConfiguration = true;
    _shopMenu->defineSelectionGrid(
        {
            {0, 1, 2, 3, 4}
        }
    );

    sellInterface->blockControllerInput = true;
    _shopMenu->addElement(sellInterface);

    _shopManager.setBuyInterface(buyInterface);
    _shopManager.setSellInterface(sellInterface);

    _ui->addMenu(_shopMenu);


    // Start menu
    std::shared_ptr<UILabel> titleLabel = std::shared_ptr<UILabel>(new UILabel(
        GAME_NAME, 49.5f, 10.f, 6.f, _font
    ));
    _startMenu->addElement(titleLabel);

    std::shared_ptr<UILabel> versionLabel = std::shared_ptr<UILabel>(new UILabel(
        "v" + VERSION, 43.5f, 21.f, 1.f, _font
    ));
    _startMenu->addElement(versionLabel);

    std::shared_ptr<UIButton> newGameButton = std::shared_ptr<UIButton>(new UIButton(
        45, 39, 9, 3, "new game", _font, this, "newgame"
    ));
    newGameButton->setSelectionId(0);
    _startMenu->addElement(newGameButton);

    std::shared_ptr<UIButton> loadGameButton = std::shared_ptr<UIButton>(new UIButton(
        45, 46, 9, 3, "load game", _font, this, "loadgame"
    ));
    loadGameButton->setSelectionId(1);
    _startMenu->addElement(loadGameButton);

    std::shared_ptr<UIButton> settingsButton_mainMenu = std::shared_ptr<UIButton>(new UIButton(
        45, 53, 9, 3, "settings", _font, this, "settings_mainmenu"
    ));
    settingsButton_mainMenu->setSelectionId(2);
    _startMenu->addElement(settingsButton_mainMenu);

    std::shared_ptr<UIButton> controlsButton = std::shared_ptr<UIButton>(new UIButton(
        45, 60, 9, 3, "controls", _font, this, "controls"
    ));
    controlsButton->setSelectionId(3);
    _startMenu->addElement(controlsButton);
    
    std::shared_ptr<UIButton> exitGameButton = std::shared_ptr<UIButton>(new UIButton(
        45, 67, 9, 3, "exit game", _font, this, "exit"
    ));
    exitGameButton->setSelectionId(4);
    _startMenu->addElement(exitGameButton);

    _startMenu->useGamepadConfiguration = true;
    _startMenu->defineSelectionGrid(
        {
            {newGameButton->getSelectionId()},
            {loadGameButton->getSelectionId()},
            {settingsButton_mainMenu->getSelectionId()},
            {controlsButton->getSelectionId()},
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

    std::shared_ptr<UIButton> completeTutorialButton_startSettings = std::shared_ptr<UIButton>(new UIButton(
        43.5, 57, 12, 3, "disable tutorial", _font, this, "skiptutorial"
    ));
    completeTutorialButton_startSettings->setSelectionId(2);
    _startMenu_settings->addElement(completeTutorialButton_startSettings);

    _startMenu_settings->useGamepadConfiguration = true;
    _startMenu_settings->defineSelectionGrid(
        {
            {backButton_startSettings->getSelectionId()},
            {togglefullscreenButton_fromstart->getSelectionId()},
            {completeTutorialButton_startSettings->getSelectionId()}
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


    // Message Display menu
    std::shared_ptr<UIMessageDisplay> messageDisp = std::shared_ptr<UIMessageDisplay>(new UIMessageDisplay(
        _font
    ));
    _messageDispMenu->addElement(messageDisp);
    _ui->addMenu(_messageDispMenu);
    _messageDispMenu->show();


    // Controls menu
    std::shared_ptr<UIButton> backControlsMenuButton = std::shared_ptr<UIButton>(new UIButton(
        5.f, 5.f, 9.f, 3.f, "back", _font, this, "back_controls"
    ));
    backControlsMenuButton->setSelectionId(0);
    _controlsMenu->addElement(backControlsMenuButton);

    std::shared_ptr<UILabel> controlsLabel = std::shared_ptr<UILabel>(new UILabel(
        "", 5.f, 11.f, 1.f, _font
    ));
    _controlsMenu->addElement(controlsLabel);
    controlsLabel->setText((std::string)
        "keyboard/mouse\n" +
        "_______________________________\n"
        "movement: WASD\n" +
        "aim: mouse\n" +
        "sprint: shift\n" +
        "dodge: spacebar\n" +
        "shoot: left click\n" +
        "reload: R\n" +
        "pause: escape\n" +
        "open/close inventory: tab\n" +
        "talk to shopkeep: E\n\n" +
        "INVENTORY:\n" +
        "equip/use item: left click\n" +
        "drop one item: right click\n" +
        "drop stack: middle click\n\n" +
        "SHOP:\n" +
        "buy/sell one item: left click\n" +
        "buy/sell stack: right click\n" +
        "buy/sell 25: middle click"
    );

    std::shared_ptr<UILabel> controlsImage = std::shared_ptr<UILabel>(new UILabel(
        "IMAGE:res/controls.png", 34.f, 0.f, 1.f, _font, 50.f, 50.f
    ));
    _controlsMenu->addElement(controlsImage);

    _controlsMenu->useGamepadConfiguration = true;
    _controlsMenu->defineSelectionGrid(
        {
            {backControlsMenuButton->getSelectionId()}
        }
    );
    _ui->addMenu(_controlsMenu);
}

void Game::update() {
    if (!_world.playerIsInShop()) _shopMenu->hide();

    _ui->update();
    if (!_isPaused && _gameStarted) {
        if (!Tutorial::isCompleted()) Tutorial::completeStep(TUTORIAL_STEP::START);

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

        if (_showWaveMeter) {
            long long cooldownTimeAtStart = _world.getEnemySpawnCooldownTimeMilliseconds() / 1000;
            long long cooldownTimeRemaining = _world.getTimeUntilNextEnemyWave() / 1000;

            int timeRemainingMinutes = cooldownTimeRemaining / 60;
            int timeRemainingSeconds = cooldownTimeRemaining % 60;

            std::string timerString = "";
            if (_world.onEnemySpawnCooldown()) {
                timerString = " in " + std::to_string(timeRemainingMinutes) + ":"
                    + (timeRemainingSeconds < 10 ? "0" : "") + std::to_string(timeRemainingSeconds);

                _waveCounterMeter->setPercentFull(((float)cooldownTimeRemaining / (float)cooldownTimeAtStart));
            } else {
                _waveCounterMeter->setPercentFull(0.f);
            }

            _waveCounterMeter->setText("WAVE " + std::to_string(_world._currentWaveNumber) 
                + timerString);
        }

        displayEnemyWaveCountdownUpdates();
        _shopArrow.update();

        if (_player->getStamina() < _player->getMaxStamina()) _staminaMeter->show();
        else _staminaMeter->hide();

        if (!_player->isActive()) {
            MessageManager::displayMessage("You died :(\nYou made it to wave " + std::to_string(_world._currentWaveNumber), 5);
            SaveManager::deleteSaveFile();
            buttonPressed("mainmenu");
        }
    } else if (_isPaused && _gameStarted) {
        _world.incrementEnemySpawnCooldownTimeWhilePaused();
    }
    _camera->setCenter(_player->getPosition().x + (float)PLAYER_WIDTH / 2, _player->getPosition().y + (float)PLAYER_HEIGHT / 2);
}

void Game::displayEnemyWaveCountdownUpdates() {
    if (currentTimeMillis() - _lastCooldownUpdateTime > 1000) {
        int secondsUntilNextWave = _world.getTimeUntilNextEnemyWave() / 1000;

        std::string timeRemainingString = "NONE";
        if (secondsUntilNextWave == 60) timeRemainingString = "1 minute";
        else if (secondsUntilNextWave == 45) timeRemainingString = "45 seconds";
        else if (secondsUntilNextWave == 30) timeRemainingString = "30 seconds";
        else if (secondsUntilNextWave == 10) timeRemainingString = "10 seconds";

        if (timeRemainingString != "NONE") {
            MessageManager::displayMessage(timeRemainingString + " until next wave", 5);
            _lastCooldownUpdateTime = currentTimeMillis();
        }
    }
}

void Game::draw(sf::RenderTexture& surface) {
    if (_gameStarted) {
        _world.draw(surface);
        _shopArrow.draw(surface);
    }
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

        int enemyCount = _world.getEnemies().size();
        _enemyCountLabel.setString(std::to_string(enemyCount) + " enem" + (enemyCount != 1 ? "ies" : "y"));
        surface.draw(_enemyCountLabel);

        bool spawnCooldown = _world.onEnemySpawnCooldown();
        _onEnemySpawnCooldownLabel.setString((spawnCooldown ? "enemy spawn cooldown active - " : "enemy spawn cooldown expired - ") 
            + std::to_string(_world.getMaxActiveEnemies()) + " - " + std::to_string((int)PLAYER_SCORE) 
            + " (" + std::to_string(_world.getTimeUntilNextEnemyWave() / 1000) + "s)");
        surface.draw(_onEnemySpawnCooldownLabel);

        if (_textSeed == "NONE") _seedLabel.setString("seed: " + std::to_string(_world.getSeed()));
        else _seedLabel.setString("seed: " + std::to_string(_world.getSeed()) + " (" + _textSeed + ")");
        surface.draw(_seedLabel);

        _playerPosLabel.setString("x: " + std::to_string(_player->getPosition().x) + ", y: " + std::to_string(_player->getPosition().y));
        surface.draw(_playerPosLabel);

        TERRAIN_TYPE currentBiome = _world.getTerrainDataAt(_world.getCurrentChunk(), _player->getPosition());
        _currentBiomeLabel.setString("current biome: " + std::to_string((int)currentBiome));
        surface.draw(_currentBiomeLabel);

        _coinMagnetCountLabel.setString("magnets: " + std::to_string(_player->getCoinMagnetCount()));
        surface.draw(_coinMagnetCountLabel);

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
        SaveManager::setCurrentSaveFileName(_worldNameField->getText() + ".save");

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

        std::shared_ptr<DroppedItem> droppedSlimeBall 
            = std::shared_ptr<DroppedItem>(new DroppedItem(
                sf::Vector2f(_player->getPosition().x, _player->getPosition().y - 48), 2, Item::SLIME_BALL.getId(), 1, Item::SLIME_BALL.getTextureRect())
              );
        droppedSlimeBall->setWorld(&_world);
        droppedSlimeBall->loadSprite(_world.getSpriteSheet());
        _world.addEntity(droppedSlimeBall);

        _gameStarted = true;
    } else if (buttonCode == "back_newgame") {
        _newGameMenu->hide();
        _startMenu->show();
    } else if (buttonCode == "mainmenu") {
        if (_inventoryMenu->isActive()) toggleInventoryMenu();
        if (_shopMenu->isActive()) toggleShopMenu();
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
        if (!DEBUG_MODE) _cmdPrompt->lock();
        
        PLAYER_SCORE = 1.f;
        _world.setMaxActiveEnemies(INITIAL_MAX_ACTIVE_ENEMIES);
        _world._enemiesSpawnedThisRound = 0;
        _world._waveCounter = 0;
        _world._currentWaveNumber = 1;
        _world._maxEnemiesReached = false;
        _world._destroyedProps.clear();
        _world._seenShops.clear();

        _world._isPlayerInShop = false;
        _shopManager.clearLedger();

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
        _player->_isReloading = false;
        _player->_damageMultiplier = 1.f;
        _player->_maxStamina = INITIAL_MAX_STAMINA;
        _player->_staminaRefreshRate = INITIAL_STAMINA_REFRESH_RATE;
        _player->_coinMagnetCount = 0;

        _world.resetChunks();
        _world.resetEnemySpawnCooldown();

        _worldNameField->setDefaultText("My World");
        _seedField->setDefaultText(std::to_string((unsigned int)currentTimeMillis()));
        _textSeed = "NONE";
        _startMenu->show();
    } else if (buttonCode == "joingame") {
        _startMenu->hide();
        _joinGameMenu->show();
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
            int tutorialCompleted = Tutorial::isCompleted() ? 1 : 0;
            out << "fullscreen=" << std::to_string(fullscreenSetting) << std::endl;
            out << "tutorial=" << std::to_string(tutorialCompleted) << std::endl;
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
    } else if (buttonCode == "save") {
        if (!_world.playerIsInShop()) {
            SaveManager::saveGame();
            if (_player->isInBoat()) MessageManager::displayMessage(
                "Heads up:\nYou might not be in your boat when you load this save.\nI am not going to fix this bug.\nSorry\n\nIt'll still be in your inventory though so you can\njust get back in it."
                , 10);
        } else MessageManager::displayMessage("You can't save the game while you're in the shop :(", 5);
    } else if (buttonCode == "loadgame") {
        if (SaveManager::getAvailableSaveFiles().empty()) MessageManager::displayMessage("There are no saved games", 5);
        else {
            _startMenu->hide();

            std::shared_ptr<UIButton> backButton = std::shared_ptr<UIButton>(new UIButton(
                5.f, 5.f, 5.f, 3.f, "back", _font, this, "back_loadgame"
            ));
            backButton->setSelectionId(0);
            _loadGameMenu->addElement(backButton);

            float x = 0.f, y = 0.f;
            const float xOffset = 12.f, yOffset = 12.f;
            const float buttonWidth = 10.f, buttonHeight = 3.f;
            const float menuWidth = 6.f;
            const float menuHeight = 8.f;
            const float spacingX = 1.5f, spacingY = 3.5f;
            for (auto& saveFile : SaveManager::getAvailableSaveFiles()) {
                std::shared_ptr<UIButton> saveFileButton = std::shared_ptr<UIButton>(new UIButton(
                    xOffset + x * (buttonWidth + spacingX), yOffset + y * (buttonHeight + spacingY), buttonWidth, buttonHeight, 
                    splitString(saveFile, ".")[0], _font, this, "load:" + saveFile
                ));

                saveFileButton->setSelectionId(x + (y+1) * menuWidth);
                _loadGameMenu->addElement(saveFileButton);

                x++;
                if (x >= menuWidth) {
                    y++;
                    x = 0.f;
                }
            }

            if (x == 0.f) y--;
            const float menuGridRows = y+2;
            
            _loadGameMenu->useGamepadConfiguration = true;
            std::vector<std::vector<int>> grid;
            grid.push_back({ backButton->getSelectionId() });
            int xa = 0;
            for (int ya = 1; ya < menuGridRows; ya++) {
                grid.push_back({});
                xa = 0;
                for (auto& button : _loadGameMenu->getElements()) {
                    if (button->getSelectionId() == xa + ya * menuWidth) {
                        grid[ya].push_back(button->getSelectionId());
                        xa++;
                        if (xa == menuWidth) break;
                    }
                }
            }
            _loadGameMenu->defineSelectionGrid(grid);
            _loadGameMenu->show();
        }
    } else if (buttonCode == "back_loadgame") {
        _loadGameMenu->hide();
        _loadGameMenu->clearElements();

        _startMenu->show();
    } else if (stringStartsWith(buttonCode, "load:")) {
        if (SaveManager::loadGame(splitString(buttonCode, ":")[1])) {
            SaveManager::setCurrentSaveFileName(splitString(buttonCode, ":")[1]);

            _loadGameMenu->hide();
            _loadGameMenu->clearElements();

            _HUDMenu->show();
            _magazineMeter->hide();

            _gameStarted = true;
        } else {
            _loadGameMenu->hide();
            _loadGameMenu->clearElements();
            buttonPressed("mainmenu");
        }
    } else if (buttonCode == "controls") {
        _startMenu->hide();
        _controlsMenu->show();
    } else if (buttonCode == "back_controls") {
        _controlsMenu->hide();
        _startMenu->show();
    } else if (buttonCode == "skiptutorial") {
        Tutorial::completeStep(TUTORIAL_STEP::END);

        std::string fileName = "settings.config";
        try {
            if (!std::filesystem::remove(fileName))
                MessageManager::displayMessage("Could not replace settings file", 5, DEBUG);
        } catch (const std::filesystem::filesystem_error& err) {
            MessageManager::displayMessage("Could not replace settings file: " + (std::string)err.what(), 5, ERR);
        }

        try {
            std::ofstream out(fileName);
            int fullscreenSetting = FULLSCREEN ? 1 : 0;
            int tutorialCompleted = 1;
            out << "fullscreen=" << std::to_string(fullscreenSetting) << std::endl;
            out << "tutorial=" << std::to_string(tutorialCompleted) << std::endl;
            out.close();

            MessageManager::displayMessage("Disabled the tutorial", 5);
        } catch (std::exception ex) {
            MessageManager::displayMessage("Error writing to settings file: " + (std::string)ex.what(), 5, ERR);
        }
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
        if (!_commandMenu->isActive() && DEBUG_MODE) _camera->zoom(2);
        break;
    case sf::Keyboard::Equal:
        if (!_commandMenu->isActive() && DEBUG_MODE) _camera->zoom(0.5);
        break;
    case sf::Keyboard::Escape:
        togglePauseMenu();
        break;
    case sf::Keyboard::Tab:
        toggleInventoryMenu();
        break;
    case sf::Keyboard::E:
        toggleShopMenu();
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
    if (!_inventoryMenu->isActive() && !_shopMenu->isActive()) _player->mouseButtonReleased(mx, my, button);
}

void Game::mouseMoved(const int mx, const int my) {
    _ui->mouseMoved(mx, my);
}

void Game::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
    _ui->mouseWheelScrolled(mouseWheelScroll);
}

void Game::controllerButtonPressed(GAMEPAD_BUTTON button) {
}

void Game::controllerButtonReleased(GAMEPAD_BUTTON button) {
    switch (button) {
        case GAMEPAD_BUTTON::START:
            togglePauseMenu();
            break;
        case GAMEPAD_BUTTON::SELECT:
            toggleInventoryMenu();
            break;
        case GAMEPAD_BUTTON::X:
            toggleShopMenu();
            break;
    }

    if (_shopMenu->isActive()) _shopManager.controllerButtonReleased(button);
}

void Game::togglePauseMenu() {
    if (_gameStarted && !_commandMenu->isActive() && !_inventoryMenu->isActive() && !_shopMenu->isActive()) {
        if (_pauseMenu->isActive()) _pauseMenu->hide();
        else _pauseMenu->show();
        _isPaused = !_isPaused;
    } else if (_gameStarted && _inventoryMenu->isActive()) _inventoryMenu->hide();
    else if (_gameStarted && _shopMenu->isActive()) _shopMenu->hide();
}

void Game::toggleInventoryMenu() {
    if (_gameStarted && !_commandMenu->isActive() && !_shopMenu->isActive()) {
        if (_inventoryMenu->isActive()) _inventoryMenu->hide();
        else _inventoryMenu->show();

        _player->_inventoryMenuIsOpen = _inventoryMenu->isActive();

        // this causes non-critical visual problems that i would like to not have happen
        //_isPaused = _inventoryMenu->isActive();
    }
}

void Game::toggleShopMenu() {
    if (_world.playerIsInShop() && !_inventoryMenu->isActive() && !_shopMenu->isActive() && !_isPaused) {
        for (auto& entity : _world.getEntities()) {
            if (entity->isActive() && entity->getEntityType() == "shopkeep") {
                if (_player->getHitBox().intersects(entity->getHitBox())) {
                    _shopMenu->show();
                    MessageManager::displayMessage(
                        "Left click to buy/sell 1 item\nRight click to buy/sell a stack\nMiddle click to buy/sell 25",
                        10
                    );
                    break;
                }
            }
        }
    } else if (_shopMenu->isActive()) _shopMenu->hide();

    _player->_inventoryMenuIsOpen = _shopMenu->isActive();
}

void Game::textEntered(sf::Uint32 character) {
    _ui->textEntered(character);
}
