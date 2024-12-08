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
#include "ShaderManager.h"
#include "Versioning.h"
#include "InputBindings.h"
#include "../ui/UIKeyboardBindingButton.h"
#include "../ui/UIGamepadBindingButton.h"
#include <boost/random/uniform_int_distribution.hpp>
#include "music/MusicManager.h"
#include "Viewport.h"
#include "../world/entities/projectiles/ProjectilePoolManager.h"
#include "../world/entities/AltarArrow.h"
#include "../ui/UISlider.h"
#include "SoundManager.h"

Game::Game(sf::View* camera, sf::RenderWindow* window) : 
    _player(std::shared_ptr<Player>(new Player(sf::Vector2f(0, 0), window, _isPaused))), _world(World(_player, _showDebug)) {
    _camera = camera;
    _window = window;

    Viewport::setCamera(camera);
    Viewport::setWindow(window);

    if (!_font.loadFromFile("res/font.ttf")) {
        MessageManager::displayMessage("Failed to load font!", 10, WARN);
    }
    _versionLabel.setFont(_font);
    _versionLabel.setCharacterSize(24);
    _versionLabel.setString("v" + VERSION + " (" + BUILD_NUMBER + ")");
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
    
    _hardModeEnabledLabel.setFont(_font);
    _hardModeEnabledLabel.setCharacterSize(24);
    _hardModeEnabledLabel.setString("hardmode: ");
    _hardModeEnabledLabel.setPosition(0, 250);

    _loadingStatusLabel.setFont(_font);
    _loadingStatusLabel.setCharacterSize(UIElement::getRelativeWidth(3.f));
    _loadingStatusLabel.setString("loading...");
    _loadingStatusLabel.setPosition(UIElement::getRelativePos(44, 50));


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
    AltarArrow::setWorld(&_world);
    AltarArrow::loadSprite(_spriteSheet);

    GamePad::addListener(_player);
    GamePad::addListener(_ui);

    SaveManager::init(&_world, &_shopManager);

    initUI();

    loadLoadingScreenMessages();
    loadTips();

    displayStartupMessages();
}

void Game::initUI() {
    // Title screen background
    std::vector<std::string> bgPaths;
    if (std::filesystem::is_directory("res/waterbg")) {
        for (const auto& entry : std::filesystem::directory_iterator("res/waterbg")) {
            std::string pathStr = entry.path().string();
            replaceAll(pathStr, "\\", "/");
            if (stringEndsWith(pathStr, ".png")) {
                bgPaths.push_back(pathStr);
            }
        }
        if (bgPaths.size() > 0) {
            int tsBgIndex = randomInt(0, bgPaths.size() - 1);
            _titleScreenBackground = std::shared_ptr<UILabel>(new UILabel("IMAGE:" + bgPaths.at(tsBgIndex), 0, 0, 0, _font, 100.f, 100.f, false));
            _titleScreenBackground->show();
        } else {
            MessageManager::displayMessage("No background images found", 5, DEBUG);
            _titleScreenBackground = std::shared_ptr<UILabel>(new UILabel("", 0, 0, 0, _font, 100.f, 100.f, false));
        }
    } else {
        MessageManager::displayMessage("Could not find res/waterbg", 5, WARN);
        _titleScreenBackground = std::shared_ptr<UILabel>(new UILabel("", 0, 0, 0, _font, 100.f, 100.f, false));
    }


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

    std::shared_ptr<UIButton> pauseControlsButton = std::shared_ptr<UIButton>(new UIButton(
        1, 29, 9, 3, "controls", _font, this, "controls"
    ));
    pauseControlsButton->setSelectionId(4);
    _pauseMenu->addElement(pauseControlsButton);

    std::shared_ptr<UIButton> statsMenuButton_pause = std::shared_ptr<UIButton>(new UIButton(
        1, 35, 9, 3, "stats", _font, this, "stats_pause"
    ));
    statsMenuButton_pause->setSelectionId(5);
    _pauseMenu->addElement(statsMenuButton_pause);

    _pauseMenu->useGamepadConfiguration = true;
    _pauseMenu->defineSelectionGrid(
        {
            {mainMenuButton->getSelectionId()},
            {exitButton->getSelectionId()},
            {saveButton->getSelectionId()},
            {settingsButton->getSelectionId()},
            {pauseControlsButton->getSelectionId()},
            {statsMenuButton_pause->getSelectionId()}
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

    _vsyncToggleButton_pauseMenu = std::shared_ptr<UIButton>(new UIButton(
        1, 17, 12, 3, (VSYNC_ENABLED ? "disable" : "enable") + (std::string)" vsync", _font, this, "togglevsync"
    ));
    _vsyncToggleButton_pauseMenu->setSelectionId(2);
    _pauseMenu_settings->addElement(_vsyncToggleButton_pauseMenu);

    std::shared_ptr<UIButton> audioSettingsButton_pauseMenu = std::shared_ptr<UIButton>(new UIButton(
        1, 23, 11, 3, "audio settings", _font, this, "audiosettings_pause"
    ));
    audioSettingsButton_pauseMenu->setSelectionId(3);
    _pauseMenu_settings->addElement(audioSettingsButton_pauseMenu);

    _pauseMenu_settings->useGamepadConfiguration = true;
    _pauseMenu_settings->defineSelectionGrid(
        {
            {backSettingsMenuButton->getSelectionId()},
            {togglefullscreenButton->getSelectionId()},
            {_vsyncToggleButton_pauseMenu->getSelectionId()},
            {audioSettingsButton_pauseMenu->getSelectionId()}
        }
    );
    _ui->addMenu(_pauseMenu_settings);


    // Audio menu
    std::shared_ptr<UIButton> backAudioSettingsButton = std::shared_ptr<UIButton>(new UIButton(
        1, 5, 9, 3, "back", _font, this, "back_audio"
    ));
    backAudioSettingsButton->setSelectionId(0);
    _audioMenu->addElement(backAudioSettingsButton);

    _sfxSlider = std::shared_ptr<UISlider>(new UISlider(
        "sound effects:", 45, 11, 10, _font, "sfxslider"
    ));
    _sfxSlider->setSelectionId(1);
    _sfxSlider->setListener(this);
    _sfxSlider->setValue(SFX_VOLUME);
    _audioMenu->addElement(_sfxSlider);

    _musicSlider = std::shared_ptr<UISlider>(new UISlider(
        "music:", 45, 19, 10, _font, "musicslider"
    ));
    _musicSlider->setSelectionId(2);
    _musicSlider->setListener(this);
    _musicSlider->setValue(MUSIC_VOLUME);
    _audioMenu->addElement(_musicSlider);

    _audioMenu->useGamepadConfiguration = true;
    _audioMenu->defineSelectionGrid(
        {
            {backAudioSettingsButton->getSelectionId()},
            {_sfxSlider->getSelectionId()},
            {_musicSlider->getSelectionId()}
        }
    );
    _ui->addMenu(_audioMenu);


    // HUD
    std::shared_ptr<UIAttributeMeter> playerHpMeter = std::shared_ptr<UIAttributeMeter>(new UIAttributeMeter(
        "HP", 50, 92, 24, 1.5f, _player->getHitPointsRef(), _player->getMaxHitPointsRef(), _font
    ));
    playerHpMeter->setColor(0xCC0000FF);
    playerHpMeter->setBackgroundColor(0x9A0000FF);
    _HUDMenu->addElement(playerHpMeter);

    _magazineMeter = std::shared_ptr<UIAttributeMeter>(new UIAttributeMeter(
        "", 90, 92, 6, 3.f, _player->getMagazineContentsPercentage(), _maxMagPercentage, _font
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
    //_inventoryMenu->addElement(filterAmmoButton);

    std::shared_ptr<UIButton> filterMiscButton = std::shared_ptr <UIButton>(new UIButton(
        26, 28, 5, 3, "misc", _font, inventoryInterface.get(), "filter_misc"
    ));
    filterMiscButton->pressWhenSelected = true;
    filterMiscButton->setSelectionId(3);
    _inventoryMenu->addElement(filterMiscButton);
    
    _inventoryMenu->useGamepadConfiguration = true;
    _inventoryMenu->defineSelectionGrid(
        {
            {0, 1, 2, 3}
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
    //_shopMenu->addElement(shop_filterAmmoButton);

    std::shared_ptr<UIButton> shop_filterMiscButton = std::shared_ptr <UIButton>(new UIButton(
        26, 48, 5, 3, "misc", _font, buyInterface.get(), "filter_misc"
    ));
    shop_filterMiscButton->pressWhenSelected = true;
    shop_filterMiscButton->setSelectionId(3);
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
    //_shopMenu->addElement(sellshop_filterAmmoButton);

    std::shared_ptr<UIButton> sellshop_filterMiscButton = std::shared_ptr <UIButton>(new UIButton(
        71 - 2.25, 48, 5, 3, "misc", _font, sellInterface.get(), "filter_misc"
    ));
    sellshop_filterMiscButton->pressWhenSelected = true;
    sellshop_filterMiscButton->setSelectionId(3);
    _shopMenu->addElement(sellshop_filterMiscButton);

    _shopMenu->useGamepadConfiguration = true;
    _shopMenu->defineSelectionGrid(
        {
            {0, 1, 2, 3}
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
    //_startMenu->addElement(titleLabel);

    std::shared_ptr<UILabel> logoImage = std::shared_ptr<UILabel>(new UILabel(
        "IMAGE:res/logo.png", 25.0f, -18.f, 1.f, _font, 50.f, 50.f
    ));
    _startMenu->addElement(logoImage);

    std::shared_ptr<UILabel> versionLabel = std::shared_ptr<UILabel>(new UILabel(
        "v" + VERSION, 2.f, 96.f, 1.f, _font
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

    std::shared_ptr<UIButton> statsMenuButton_main = std::shared_ptr<UIButton>(new UIButton(
        45, 67, 9, 3, "stats", _font, this, "stats_main"
    ));
    statsMenuButton_main->setSelectionId(4);
    _startMenu->addElement(statsMenuButton_main);
    
    std::shared_ptr<UIButton> exitGameButton = std::shared_ptr<UIButton>(new UIButton(
        45, 74, 9, 3, "exit game", _font, this, "exit"
    ));
    exitGameButton->setSelectionId(5);
    _startMenu->addElement(exitGameButton);

    _startMenu->useGamepadConfiguration = true;
    _startMenu->defineSelectionGrid(
        {
            {newGameButton->getSelectionId()},
            {loadGameButton->getSelectionId()},
            {settingsButton_mainMenu->getSelectionId()},
            {controlsButton->getSelectionId()},
            {statsMenuButton_main->getSelectionId()},
            {exitGameButton->getSelectionId()}
        }
    );
    _ui->addMenu(_startMenu);
    _startMenu->show();

    
    // Settings menu (from start menu)
    std::shared_ptr<UIButton> backButton_startSettings = std::shared_ptr<UIButton>(new UIButton(
        45, 10, 9, 3, "back", _font, this, "back_startsettings"
    ));
    backButton_startSettings->setSelectionId(0);
    _startMenu_settings->addElement(backButton_startSettings);

    std::shared_ptr<UIButton> togglefullscreenButton_fromstart = std::shared_ptr<UIButton>(new UIButton(
        36, 17, 28, 3, "toggle fullscreen (requires restart)", _font, this, "togglefullscreen"
    ));
    togglefullscreenButton_fromstart->setSelectionId(1);
    _startMenu_settings->addElement(togglefullscreenButton_fromstart);

    _vsyncToggleButton_mainMenu = std::shared_ptr<UIButton>(new UIButton(
        43.5, 24, 12, 3, (VSYNC_ENABLED ? "disable" : "enable") + (std::string)" vsync", _font, this, "togglevsync"
    ));
    _vsyncToggleButton_mainMenu->setSelectionId(2);
    _startMenu_settings->addElement(_vsyncToggleButton_mainMenu);

    _completeTutorialButton_startSettings = std::shared_ptr<UIButton>(new UIButton(
        43.5, 31, 12, 3, (!Tutorial::isCompleted() ? "disable" : "enable") + (std::string)" tutorial", _font, this, "skiptutorial"
    ));
    _completeTutorialButton_startSettings->setSelectionId(3);
    _startMenu_settings->addElement(_completeTutorialButton_startSettings);

    std::shared_ptr<UIButton> audioSettingsButton_mainMenu = std::shared_ptr<UIButton>(new UIButton(
        43.5, 38, 12, 3, "audio settings", _font, this, "audiosettings_main"
    ));
    audioSettingsButton_mainMenu->setSelectionId(4);
    _startMenu_settings->addElement(audioSettingsButton_mainMenu);

    _startMenu_settings->useGamepadConfiguration = true;
    _startMenu_settings->defineSelectionGrid(
        {
            {backButton_startSettings->getSelectionId()},
            {togglefullscreenButton_fromstart->getSelectionId()},
            {_vsyncToggleButton_mainMenu->getSelectionId()},
            {_completeTutorialButton_startSettings->getSelectionId()},
            {audioSettingsButton_mainMenu->getSelectionId()}
        }
    );
    _ui->addMenu(_startMenu_settings);


    // New game menu
    _tipLabel = std::shared_ptr<UILabel>(new UILabel(
        "", 50.f, 20.f, 2, _font
    ));
    _newGameMenu->addElement(_tipLabel);

    _worldNameField = std::shared_ptr<UITextField>(new UITextField(
        "world name:", 49.5, 37, _font
    ));
    _worldNameField->setDefaultText("New World");
    _worldNameField->setId("newworld");
    _worldNameField->setListener(this);
    _worldNameField->setSelectionId(0);
    _newGameMenu->addElement(_worldNameField);

    _seedField = std::shared_ptr<UITextField>(new UITextField(
        "seed:", 49.5, 48, _font
    ));
    _seedField->setDefaultText(std::to_string((unsigned int)currentTimeMillis()));
    _seedField->setId("seed");
    _seedField->setListener(this);
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

    _hardModeToggleButton = std::shared_ptr<UIButton>(new UIButton(
        44, 72, 11, 3, "hard mode: off", _font, this, "hardmode"
    ));
    _hardModeToggleButton->setSelectionId(4);
    if (Tutorial::isCompleted()) _newGameMenu->addElement(_hardModeToggleButton);
    
    _newGameMenu->useGamepadConfiguration = true;
    _newGameMenu->defineSelectionGrid(
        {
            {_worldNameField->getSelectionId()},
            {_seedField->getSelectionId()},
            {startGameButton->getSelectionId()},
            {newGameBackButton->getSelectionId()},
            {_hardModeToggleButton->getSelectionId()}
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


    // Boss HUD menu
    // probably should not be passing this as a reference but it should be okay
    _bossHPMeter = std::shared_ptr<UIAttributeMeter>(new UIAttributeMeter(
        "", 50, 8, 32, 1.5f, placeholder, placeholder, _font
    ));
    _bossHPMeter->setColor(0xCC0000FF);
    _bossHPMeter->setBackgroundColor(0x9A0000FF);
    _bossHPMeter->useDefaultLabel(false);
    _bossHPMeter->useAttributes(false);
    _bossHUDMenu->addElement(_bossHPMeter);
    _ui->addMenu(_bossHUDMenu);


    // Controls menu
    std::shared_ptr<UIButton> backControlsMenuButton = std::shared_ptr<UIButton>(new UIButton(
        5.f, 5.f, 9.f, 3.f, "back", _font, this, "back_controls"
    ));
    backControlsMenuButton->setSelectionId(0);
    _controlsMenu->addElement(backControlsMenuButton);

    std::shared_ptr<UIButton> bindingsMenuButton = std::shared_ptr<UIButton>(new UIButton(
        16.f, 5.f, 14.f, 3.f, "change bindings", _font, this, "bindings"
    ));
    bindingsMenuButton->setSelectionId(1);
    _controlsMenu->addElement(bindingsMenuButton);

    std::shared_ptr<UILabel> controlsLabel = std::shared_ptr<UILabel>(new UILabel(
        "", 5.f, 11.f, 1.f, _font
    ));
    _controlsMenu->addElement(controlsLabel);
    controlsLabel->setText((std::string)
        "keyboard/mouse\n" +
        "_______________________________\n"
        "movement: WASD\n" +
        "aim: mouse\n" +
        "slow-walk: shift\n" +
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
            {backControlsMenuButton->getSelectionId(), bindingsMenuButton->getSelectionId()}
        }
    );
    _ui->addMenu(_controlsMenu);


    // Input bindings menu
    std::shared_ptr<UIButton> backBindingsMenuButton = std::shared_ptr<UIButton>(new UIButton(
        5.f, 5.f, 9.f, 3.f, "back", _font, this, "back_bindings"
    ));
    backBindingsMenuButton->setSelectionId(0);
    _inputBindingsMenu->addElement(backBindingsMenuButton);

    std::shared_ptr<UILabel> keyboardLabel = std::shared_ptr<UILabel>(new UILabel(
        "keyboard controls", 25.f, 6.f, 1.5f, _font
    ));
    _inputBindingsMenu->addElement(keyboardLabel);

    std::shared_ptr<UILabel> gamepadLabel = std::shared_ptr<UILabel>(new UILabel(
        "gamepad controls", 57.f, 6.f, 1.5f, _font
    ));
    _inputBindingsMenu->addElement(gamepadLabel);

    // keyboard
    std::shared_ptr<UIKeyboardBindingButton> sprintButtonKeyboard = std::shared_ptr<UIKeyboardBindingButton>(new UIKeyboardBindingButton(
        18.f, 12.f, 14.f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::WALK
    ));
    sprintButtonKeyboard->setSelectionId(-1);
    _inputBindingsMenu->addElement(sprintButtonKeyboard);

    std::shared_ptr<UIKeyboardBindingButton> dodgeButtonKeyboard = std::shared_ptr<UIKeyboardBindingButton>(new UIKeyboardBindingButton(
        18.f, 18.f, 14.f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::DODGE
    ));
    dodgeButtonKeyboard->setSelectionId(-1);
    _inputBindingsMenu->addElement(dodgeButtonKeyboard);

    std::shared_ptr<UIKeyboardBindingButton> reloadButtonKeyboard = std::shared_ptr<UIKeyboardBindingButton>(new UIKeyboardBindingButton(
        18.f, 24.f, 14.f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::RELOAD
    ));
    reloadButtonKeyboard->setSelectionId(-1);
    _inputBindingsMenu->addElement(reloadButtonKeyboard);

    std::shared_ptr<UIKeyboardBindingButton> interactButtonKeyboard = std::shared_ptr<UIKeyboardBindingButton>(new UIKeyboardBindingButton(
        18.f, 30.f, 14.f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::INTERACT
    ));
    interactButtonKeyboard->setSelectionId(-1);
    _inputBindingsMenu->addElement(interactButtonKeyboard);

    std::shared_ptr<UIKeyboardBindingButton> inventoryButtonKeyboard = std::shared_ptr<UIKeyboardBindingButton>(new UIKeyboardBindingButton(
        18.f, 36.f, 20.f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::TOGGLE_INVENTORY
    ));
    inventoryButtonKeyboard->setSelectionId(-1);
    _inputBindingsMenu->addElement(inventoryButtonKeyboard);

    std::shared_ptr<UIKeyboardBindingButton> pauseButtonKeyboard = std::shared_ptr<UIKeyboardBindingButton>(new UIKeyboardBindingButton(
        18.f, 42.f, 14.f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::TOGGLE_PAUSE
    ));
    pauseButtonKeyboard->setSelectionId(-1);
    _inputBindingsMenu->addElement(pauseButtonKeyboard);

    std::shared_ptr<UIKeyboardBindingButton> skipCooldownButtonKeyboard = std::shared_ptr<UIKeyboardBindingButton>(new UIKeyboardBindingButton(
        18.f, 48.f, 14.f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::SKIP_COOLDOWN
    ));
    skipCooldownButtonKeyboard->setSelectionId(-1);
    _inputBindingsMenu->addElement(skipCooldownButtonKeyboard);

    // gamepad
    std::shared_ptr<UIGamepadBindingButton> sprintButtonGamepad = std::shared_ptr<UIGamepadBindingButton>(new UIGamepadBindingButton(
        50.f, 12.f, 16.5f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::WALK
    ));
    sprintButtonGamepad->setSelectionId(1);
    _inputBindingsMenu->addElement(sprintButtonGamepad);

    std::shared_ptr<UIGamepadBindingButton> dodgeButtonGamepad = std::shared_ptr<UIGamepadBindingButton>(new UIGamepadBindingButton(
        50.f, 18.f, 14.f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::DODGE
    ));
    dodgeButtonGamepad->setSelectionId(2);
    _inputBindingsMenu->addElement(dodgeButtonGamepad);

    std::shared_ptr<UIGamepadBindingButton> reloadButtonGamepad = std::shared_ptr<UIGamepadBindingButton>(new UIGamepadBindingButton(
        50.f, 24.f, 14.f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::RELOAD
    ));
    reloadButtonGamepad->setSelectionId(3);
    _inputBindingsMenu->addElement(reloadButtonGamepad);

    std::shared_ptr<UIGamepadBindingButton> shootButtonGamepad = std::shared_ptr<UIGamepadBindingButton>(new UIGamepadBindingButton(
        50.f, 30.f, 14.f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::SHOOT
    ));
    shootButtonGamepad->setSelectionId(4);
    _inputBindingsMenu->addElement(shootButtonGamepad);

    std::shared_ptr<UIGamepadBindingButton> interactButtonGamepad = std::shared_ptr<UIGamepadBindingButton>(new UIGamepadBindingButton(
        50.f, 36.f, 14.f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::INTERACT
    ));
    interactButtonGamepad->setSelectionId(5);
    _inputBindingsMenu->addElement(interactButtonGamepad);

    std::shared_ptr<UIGamepadBindingButton> inventoryButtonGamepad = std::shared_ptr<UIGamepadBindingButton>(new UIGamepadBindingButton(
        50.f, 42.f, 20.f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::TOGGLE_INVENTORY
    ));
    inventoryButtonGamepad->setSelectionId(6);
    _inputBindingsMenu->addElement(inventoryButtonGamepad);

    std::shared_ptr<UIGamepadBindingButton> pauseButtonGamepad = std::shared_ptr<UIGamepadBindingButton>(new UIGamepadBindingButton(
        50.f, 48.f, 14.f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::TOGGLE_PAUSE
    ));
    pauseButtonGamepad->setSelectionId(7);
    _inputBindingsMenu->addElement(pauseButtonGamepad);

    std::shared_ptr<UIGamepadBindingButton> skipCooldownButtonGamepad = std::shared_ptr<UIGamepadBindingButton>(new UIGamepadBindingButton(
        50.f, 54.f, 14.f, 3.f, _font, InputBindingManager::BINDABLE_ACTION::SKIP_COOLDOWN
    ));
    skipCooldownButtonGamepad->setSelectionId(8);
    _inputBindingsMenu->addElement(skipCooldownButtonGamepad);


    std::shared_ptr<UIButton> resetBindingsButton = std::shared_ptr<UIButton>(new UIButton(
        38.f, 80.f, 14.f, 3.f, "reset to default", _font, this, "resetbindings"
    ));
    resetBindingsButton->setSelectionId(9);
    _inputBindingsMenu->addElement(resetBindingsButton);

    _inputBindingsMenu->useGamepadConfiguration = true;
    _inputBindingsMenu->defineSelectionGrid(
        {
            {backBindingsMenuButton->getSelectionId()},
            {sprintButtonGamepad->getSelectionId()},
            {dodgeButtonGamepad->getSelectionId()},
            {reloadButtonGamepad->getSelectionId()},
            {shootButtonGamepad->getSelectionId()},
            {interactButtonGamepad->getSelectionId()},
            {inventoryButtonGamepad->getSelectionId()},
            {pauseButtonGamepad->getSelectionId()},
            {skipCooldownButtonGamepad->getSelectionId()},
            {resetBindingsButton->getSelectionId()}
        }
    );
    _ui->addMenu(_inputBindingsMenu);


    // Stats menu from pause
    _currentSaveStatsLabel = std::shared_ptr<UILabel>(new UILabel(
        "", 15.f, 13.5f, 1.25f, _font
    ));
    _statsMenu_pauseMenu->addElement(_currentSaveStatsLabel);

    _overallStatsLabel_pauseMenu = std::shared_ptr<UILabel>(new UILabel(
        "", 45.f, 13.5f, 1.25f, _font
    ));
    _statsMenu_pauseMenu->addElement(_overallStatsLabel_pauseMenu);

    std::shared_ptr<UIButton> back_stats_pauseButton = std::shared_ptr<UIButton>(new UIButton(
        5.f, 5.f, 9.f, 3.f, "back", _font, this, "back_stats_pause"
    ));
    back_stats_pauseButton->setSelectionId(0);
    _statsMenu_pauseMenu->addElement(back_stats_pauseButton);

    _statsMenu_pauseMenu->useGamepadConfiguration = true;
    _statsMenu_pauseMenu->defineSelectionGrid(
        {
            {back_stats_pauseButton->getSelectionId()}
        }
    );
    _ui->addMenu(_statsMenu_pauseMenu);


    // Stats menu from main
    _overallStatsLabel_mainMenu = std::shared_ptr<UILabel>(new UILabel(
        "", 15.f, 15.f, 1.25f, _font
    ));
    _statsMenu_mainMenu->addElement(_overallStatsLabel_mainMenu);

    std::shared_ptr<UIButton> back_stats_mainButton = std::shared_ptr<UIButton>(new UIButton(
        5.f, 5.f, 9.f, 3.f, "back", _font, this, "back_stats_main"
    ));
    back_stats_mainButton->setSelectionId(0);
    _statsMenu_mainMenu->addElement(back_stats_mainButton);

    _statsMenu_mainMenu->useGamepadConfiguration = true;
    _statsMenu_mainMenu->defineSelectionGrid(
        {
            {back_stats_mainButton->getSelectionId()}
        }
    );
    _ui->addMenu(_statsMenu_mainMenu);

   
    // Death screen
    std::shared_ptr<UILabel> youDiedLabel = std::shared_ptr<UILabel>(new UILabel(
        "you died :(", 50.f, 3.f, 3.f, _font
    ));
    _deathMenu->addElement(youDiedLabel);

    _waveReachedLabel = std::shared_ptr<UILabel>(new UILabel(
        "you made it to wave ", 49.5f, 9.f, 1.25f, _font
    ));
    _deathMenu->addElement(_waveReachedLabel);

    _statsLabel_deathMenu = std::shared_ptr<UILabel>(new UILabel(
        "", 36.f, 18.f, 1.f, _font
    ));
    _deathMenu->addElement(_statsLabel_deathMenu);

    std::shared_ptr<UIButton> mainMenuButton_deathMenu = std::shared_ptr<UIButton>(new UIButton(
        45.f, 87.f, 9.f, 3.f, "main menu", _font, this, "death_mm"
    ));
    mainMenuButton_deathMenu->setSelectionId(0);
    _deathMenu->addElement(mainMenuButton_deathMenu);

    _deathMenu->useGamepadConfiguration = true;
    _deathMenu->defineSelectionGrid(
        {
            {mainMenuButton_deathMenu->getSelectionId()}
        }
    );
    _ui->addMenu(_deathMenu);
    //


    // Command prompt menu
    _cmdPrompt = std::shared_ptr<UICommandPrompt>(new UICommandPrompt(&_world, _font));
    _commandMenu->addElement(_cmdPrompt);
    _ui->addMenu(_commandMenu);

    // Virtual keyboard
    initVirtualKeyboard();
}

void Game::update() {
    if (STEAMAPI_INITIATED && SteamAPI_IsSteamRunning()) {
        SteamAPI_RunCallbacks();
    }

    if (!_world.playerIsInShop() && _shopMenu->isActive()) toggleShopMenu();

    _ui->update();
    if (!_isPaused && _gameStarted) {
        if (!Tutorial::isCompleted()) {
            Tutorial::completeStep(TUTORIAL_STEP::START);

            if (currentTimeMillis() - Tutorial::tutorialStartTime >= Tutorial::tutorialAutoDisableTime) {
                Tutorial::completeStep(TUTORIAL_STEP::END);
                MessageManager::displayMessage("Tutorial auto-disabled", 5, DEBUG);

                updateSettingsFiles();

                _completeTutorialButton_startSettings->setLabelText((!Tutorial::isCompleted() ? "disable" : "enable") + (std::string)" tutorial");
            }
        }

        if (_world.playerIsInShop() && !_shopKeep->isActive() && _shopMenu->isActive()) toggleShopMenu();
        _world.update();

        if (_world.bossIsActive() && !_bossHUDMenu->isActive()) {
            auto& boss = _world.getCurrentBoss();
            _bossHPMeter->setText(boss->getDisplayName());
            _bossHUDMenu->show();
        } else if (!_world.bossIsActive() && _bossHUDMenu->isActive()) _bossHUDMenu->hide();
        else if (_world.bossIsActive()) {
            _bossHPMeter->setPercentFull(((float)_world.getCurrentBoss()->getHitPoints() / (float)_world.getCurrentBoss()->getMaxHitPoints()));
        }

        int equippedTool = _player->getInventory().getEquippedItemId(EQUIPMENT_TYPE::TOOL);
        if (equippedTool != NOTHING_EQUIPPED && Item::ITEMS[equippedTool]->isGun()) {
            int ammoIndex = _player->getInventory().getEquippedIndex(EQUIPMENT_TYPE::AMMO);
            int ammoCount = ammoIndex == NOTHING_EQUIPPED ? 0 : _player->getInventory().getItemAmountAt(ammoIndex);

            _magazineMeter->show();
            if (ammoIndex != NOTHING_EQUIPPED && _player->getInventory().getItemIdAt(ammoIndex) == Item::ITEMS[equippedTool]->getAmmoId()) {
                _magazineMeter->setText(sf::String(std::to_string(_player->getMagazineContents())));
            } else {
                _magazineMeter->setText(sf::String(std::to_string(_player->getMagazineContents())));
            }
        } else {
            _magazineMeter->hide();
        }

        if (_showWaveMeter) {
            const long long cooldownTimeAtStart = _world.getEnemySpawnCooldownTimeMilliseconds() / 1000;
            const long long cooldownTimeRemaining = _world.getTimeUntilNextEnemyWave() / 1000;

            const int timeRemainingMinutes = cooldownTimeRemaining / 60;
            const int timeRemainingSeconds = cooldownTimeRemaining % 60;

            std::string timerString = "";
            if (_world.onEnemySpawnCooldown() && !_world.playerIsInShop()) {
                timerString = " in " + std::to_string(timeRemainingMinutes) + ":"
                    + (timeRemainingSeconds < 10 ? "0" : "") + std::to_string(timeRemainingSeconds);

                _waveCounterMeter->setPercentFull(((float)cooldownTimeRemaining / (float)cooldownTimeAtStart));

                if (sf::Keyboard::isKeyPressed(InputBindingManager::getKeyboardBinding(InputBindingManager::BINDABLE_ACTION::SKIP_COOLDOWN))
                    || (GamePad::isButtonPressed(InputBindingManager::getGamepadBinding(InputBindingManager::BINDABLE_ACTION::SKIP_COOLDOWN)))) {
                    _world._cooldownStartTime -= _world.getEnemySpawnCooldownTimeMilliseconds() * 0.00657;
                }
            } else if (!_world.playerIsInShop()) {
                _waveCounterMeter->setPercentFull(0.f);
            }

            if (!_world.playerIsInShop()) {
                _waveCounterMeter->setText("WAVE " + std::to_string(_world._currentWaveNumber)
                    + timerString);
            }
        }

        displayEnemyWaveCountdownUpdates();
        _shopArrow.update();
        AltarArrow::update();

        if (_player->getStamina() < _player->getMaxStamina()) _staminaMeter->show();
        else _staminaMeter->hide();

        if (!_player->isActive()) {
            onPlayerDeath();
        } else {
            autoSave();
        }
    } else if (!_isPaused && !_gameStarted && _gameLoading) {
        _world.dumpChunkBuffer();
        if (_world._chunks.size() == 4) {
            _gameLoading = false;
            _gameStarted = true;
            _HUDMenu->show();
            
            MUSIC_SITUTAION situation = MUSIC_SITUTAION::WAVE;
            if (_world.onEnemySpawnCooldown()) {
                situation = MUSIC_SITUTAION::COOLDOWN;
                if (_world.bossIsActive()) situation = MUSIC_SITUTAION::BOSS;
            }
            MusicManager::setSituation(situation);
        }
    }
    _camera->setCenter(_player->getPosition().x + (float)PLAYER_WIDTH / 2, _player->getPosition().y + (float)PLAYER_HEIGHT / 2);
}

void Game::displayEnemyWaveCountdownUpdates() {
    if (_world.onEnemySpawnCooldown() && currentTimeMillis() - _lastCooldownUpdateTime > 1000 && !_world.playerIsInShop() && !_isPaused && !_world.bossIsActive()) {
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
    if (!_isPaused) ShaderManager::updateShaders();
    
    if (_gameStarted) {
        _world.draw(surface);
        _shopArrow.draw(surface);
        AltarArrow::draw(surface);
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

        if (!_deathMenu->isActive()) _titleScreenBackground->render(surface, ShaderManager::getShader("waves_frag"));
        else {
            sf::RectangleShape deathStatsBg;
            deathStatsBg.setFillColor(sf::Color(0x333337FF));
            deathStatsBg.setOutlineColor(sf::Color(0x3C3C40FF));
            deathStatsBg.setOutlineThickness(UIElement::getRelativeWidth(1.f));
            deathStatsBg.setPosition(UIElement::getRelativePos(34.f, 16.f));
            deathStatsBg.setSize(UIElement::getRelativePos(32.f, 65.f));
            surface.draw(deathStatsBg);
        }

        if (_gameLoading) {
            std::string elipsesString = "";
            int elipsesCount = ((_frameCounter / 20) % 4);
            for (int i = 0; i < elipsesCount; i++) {
                elipsesString += ".";
            }

            if (randomInt(0, 2000) == 0) _loadingScreenMessageIndex = randomInt(0, _loadingScreenMessages.size() - 1);

            _loadingStatusLabel.setString(_loadingScreenMessages.at(_loadingScreenMessageIndex) + elipsesString);
            sf::Text messageWithoutElipses = _loadingStatusLabel;
            messageWithoutElipses.setString(_loadingScreenMessages.at(_loadingScreenMessageIndex));
            float labelWidth = messageWithoutElipses.getGlobalBounds().width;
            _loadingStatusLabel.setPosition(UIElement::getRelativeWidth(50) - labelWidth / 2, UIElement::getRelativeHeight(50));
            surface.draw(_loadingStatusLabel);
            _frameCounter++;
        }
    }

    if (!_hideUI) {
        if (_gameStarted && (_controlsMenu->isActive() || _inputBindingsMenu->isActive() || _statsMenu_pauseMenu->isActive() || _audioMenu->isActive())) {
            sf::RectangleShape controlsMenuBg;
            controlsMenuBg.setFillColor(sf::Color(0x444448FF));
            controlsMenuBg.setPosition(0, 0);
            controlsMenuBg.setSize(sf::Vector2f(surface.getSize().x, surface.getSize().y));
            surface.draw(controlsMenuBg);
        }
        _ui->draw(surface);
    }

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

        _hardModeEnabledLabel.setString("hardmode: " + (std::string)(HARD_MODE_ENABLED ? "1" : "0"));
        surface.draw(_hardModeEnabledLabel);

        _frameCounter++;
    }
}

void Game::buttonPressed(std::string buttonCode) {
    if (buttonCode == "exit") {
        _window->close();
    } else if (buttonCode == "newgame") {
        if (!Tutorial::isCompleted() && _tips.size() > 0) {
            _tipLabel->setText("Tip: " + _tips.at(randomInt(0, _tips.size() - 1)), true);
        } else {
            _tipLabel->setText("");
        }

        enableGamepadInput(_newGameMenu);
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
            MessageManager::displayMessage((std::string)ex.what() + "\nseedText was: \"" + seedText + "\"", 10, ERR);
            seed = currentTimeMillis();
        }

        // fixes tried to reset chunks while chunks were loading
        _world.resetChunks();
        //
        _world.init(seed);
        //if (!Tutorial::isCompleted()) _world._newGameCooldownLength = 45000LL;
        //else _world._newGameCooldownLength = 15000LL;
        //_world.startNewGameCooldown();
        _world._cooldownActive = true;
        _world._cooldownStartTime = currentTimeMillis();
        if (!Tutorial::isCompleted()) _world._enemySpawnCooldownTimeMilliseconds = 45000LL;
        else _world._enemySpawnCooldownTimeMilliseconds = 15000LL;

        _virtualKeyboardMenu_lower->hide();
        _virtualKeyboardMenu_upper->hide();
        _newGameMenu->hide();
        //_HUDMenu->show();
        _magazineMeter->hide();

        std::vector<unsigned int> startingItems;
        for (const auto& item : Item::ITEMS) {
            if (item->isStartingItem()) {
                startingItems.push_back(item->getId());
            }
        }
        const size_t numStartingItems = startingItems.size();

        std::shared_ptr<const Item> startingItem = (Tutorial::isCompleted() ? Item::ITEMS[startingItems[randomInt(0, numStartingItems - 1)]] : Item::ITEMS[Item::SLIME_BALL.getId()]);

        constexpr int gunStartChance = 49;
        if (Tutorial::isCompleted() && randomInt(0, gunStartChance) == 0) {
            std::vector<unsigned int> startingGuns;
            for (const auto& item : Item::ITEMS) {
                if (item->isGun() && item->isBuyable()) {
                    startingGuns.push_back(item->getId());
                }
            }

            const size_t numStartingGuns = startingGuns.size();
            startingItem = Item::ITEMS[startingGuns[randomInt(0, numStartingGuns - 1)]];
        }

        std::shared_ptr<DroppedItem> startingItemDropped 
            = std::shared_ptr<DroppedItem>(new DroppedItem(
                sf::Vector2f(_player->getPosition().x, _player->getPosition().y - 48), 2, startingItem->getId(), 1, startingItem->getTextureRect())
              );
        startingItemDropped->setWorld(&_world);
        startingItemDropped->loadSprite(_world.getSpriteSheet());
        _world.addEntity(startingItemDropped);

        startLoading();
        if (!Tutorial::isCompleted()) {
            std::string msg;
            if (GamePad::isConnected()) msg = "Press the left bumper to dodge";
            else msg = "Press spacebar to dodge";
            MessageManager::displayMessage(msg, 15);
        }

        _lastAutosaveTime = currentTimeMillis();
    } else if (buttonCode == "back_newgame") {
        _newGameMenu->hide();
        _startMenu->show();

        _virtualKeyboardMenu_lower->hide();
        _virtualKeyboardMenu_upper->hide();
    } else if (buttonCode == "mainmenu") {
        MusicManager::setSituation(MUSIC_SITUTAION::MAIN_MENU);

        _bossHUDMenu->hide();

        if (_inventoryMenu->isActive()) toggleInventoryMenu();
        if (_shopMenu->isActive()) toggleShopMenu();
        HARD_MODE_ENABLED = false;
        _hardModeToggleButton->setLabelText("hard mode: off");

        _gameStarted = false;
        _gameLoading = false;
        _isPaused = false;
        _pauseMenu->hide();
        _HUDMenu->hide();

        _cmdPrompt->unlock();
        _cmdPrompt->processCommand("killall");
        _cmdPrompt->processCommand("clear inventory");
        _cmdPrompt->processCommand("setmaxhp:100");
        _cmdPrompt->processCommand("addhp:100");
        _cmdPrompt->processCommand("respawn");
        if (!DEBUG_MODE) _cmdPrompt->lock();
        
        ProjectilePoolManager::removeAll();
        AbilityManager::resetAbilities();
        PlayerVisualEffectManager::clearPlayerEffects();
        StatManager::resetStatsForThisSave();

        PLAYER_SCORE = 1.f;
        _world.setMaxActiveEnemies(INITIAL_MAX_ACTIVE_ENEMIES);
        _world._enemiesSpawnedThisRound = 0;
        _world._waveCounter = 0;
        _world._currentWaveNumber = 1;
        _world._highestPlayerHp = 100;
        _world._maxEnemiesReached = false;
        _world._destroyedProps.clear();
        _world._seenShops.clear();
        _world._activatedAltars.clear();
        _world._deadShopKeeps.clear();
        _world._bossIsActive = false;

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
        _player->_stamina = INITIAL_MAX_STAMINA;
        _player->_staminaRefreshRate = INITIAL_STAMINA_REFRESH_RATE;
        _player->_coinMagnetCount = 0;
        _player->_speedMultiplier = 0.f;

        _world.resetChunks();
        _world.resetEnemySpawnCooldown();

        _worldNameField->setDefaultText("New World");
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
        FULLSCREEN = !FULLSCREEN;

        MessageManager::displayMessage("The game will launch in " + (std::string)(FULLSCREEN == 1 ? "fullscreen" : "windowed") + " mode next time", 5);
        updateSettingsFiles();

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

            //_HUDMenu->show();
            _magazineMeter->hide();

            startLoading();
            Tutorial::completeStep(TUTORIAL_STEP::END);
            
            _lastAutosaveTime = currentTimeMillis();
        } else {
            _loadGameMenu->hide();
            _loadGameMenu->clearElements();
            buttonPressed("mainmenu");
        }
    } else if (buttonCode == "controls") {
        if (_gameStarted) _pauseMenu->hide();
        _startMenu->hide();
        _controlsMenu->show();
    } else if (buttonCode == "back_controls") {
        _controlsMenu->hide();
        if (!_gameStarted) _startMenu->show();
        else _pauseMenu->show();
    } else if (buttonCode == "skiptutorial") {
        if (!Tutorial::isCompleted()) Tutorial::completeStep(TUTORIAL_STEP::END);
        else Tutorial::reset();
           
        updateSettingsFiles();
        MessageManager::displayMessage((Tutorial::isCompleted() ? "Disabled" : "Enabled") + (std::string)" the tutorial", 5);

        _completeTutorialButton_startSettings->setLabelText((!Tutorial::isCompleted() ? "disable" : "enable") + (std::string)" tutorial");
    } else if (buttonCode == "togglevsync") {
        VSYNC_ENABLED = !VSYNC_ENABLED;
        _window->setVerticalSyncEnabled(VSYNC_ENABLED);
        updateSettingsFiles();

        MessageManager::displayMessage((VSYNC_ENABLED ? "Enabled" : "Disabled") + (std::string)" vsync", 5);

        _vsyncToggleButton_pauseMenu->setLabelText((VSYNC_ENABLED ? "disable" : "enable") + (std::string)" vsync");
        _vsyncToggleButton_mainMenu->setLabelText((VSYNC_ENABLED ? "disable" : "enable") + (std::string)" vsync");

        if (VSYNC_ENABLED) {
            MessageManager::displayMessage("WARNING:\nOn some devices, there may be a graphical glitch when vsync is enabled while\nin fullscreen mode that can cause flashing lights", 20, WARN);
        }
    } else if (buttonCode == "bindings") {
        _controlsMenu->hide();
        _inputBindingsMenu->show();
    } else if (buttonCode == "back_bindings") {
        _inputBindingsMenu->hide();
        _controlsMenu->show();
    } else if (buttonCode == "resetbindings") {
        InputBindingManager::resetBindings();
    } else if (buttonCode == "stats_pause") {
        _pauseMenu->hide();
        _statsMenu_pauseMenu->show();

        std::string currentStatsText = "Stats for this save\n\n\n";
        generateStatsString(currentStatsText, false);

        std::string overallStatsText = "Overall stats\n\n\n";
        generateStatsString(overallStatsText, true);

        _currentSaveStatsLabel->setText(currentStatsText);
        _overallStatsLabel_pauseMenu->setText(overallStatsText);
    } else if (buttonCode == "back_stats_pause") {
        _statsMenu_pauseMenu->hide();
        _pauseMenu->show();
    } else if (buttonCode == "stats_main") {
        _startMenu->hide();
        _statsMenu_mainMenu->show();

        std::string overallStatsText = "Stats\n\n\n";
        generateStatsString(overallStatsText, true);
        _overallStatsLabel_mainMenu->setText(overallStatsText);
    } else if (buttonCode == "back_stats_main") {
        _statsMenu_mainMenu->hide();
        _startMenu->show();
    } else if (stringStartsWith(buttonCode, "textfieldarmedbygamepad:")) {
        disableGamepadInput(_newGameMenu);
        _virtualKeyboardMenu_lower->show();
        _armedTextFieldId = splitString(buttonCode, ":")[1];
    } else if (stringStartsWith(buttonCode, "virtkey:")) {
        std::string key = splitString(buttonCode, ":")[1];

        auto enterCharacter = [&](char character) {
            std::shared_ptr<UITextField> field = nullptr;
            if (_armedTextFieldId == _worldNameField->getId()) {
                field = _worldNameField;
            } else if (_armedTextFieldId == _seedField->getId()) {
                field = _seedField;
            }

            if (field != nullptr) {
                field->textEntered(character);
            }
        };

        if (key == "done") {
            _virtualKeyboardMenu_lower->hide();
            _virtualKeyboardMenu_upper->hide();
            enableGamepadInput(_newGameMenu);
        } else if (key == "caps") {
            if (_virtualKeyboardMenu_lower->isActive()) {
                _virtualKeyboardMenu_lower->hide();
                _virtualKeyboardMenu_upper->show();
            } else if (_virtualKeyboardMenu_upper->isActive()) {
                _virtualKeyboardMenu_upper->hide();
                _virtualKeyboardMenu_lower->show();
            }
        } else if (key == "back") {
            enterCharacter('\b');
        } else if (key == "space") {
            enterCharacter(' ');
        } else {
            enterCharacter(key.at(0));
        }
    } else if (buttonCode == "death_mm") {
        _deathMenu->hide();
        buttonPressed("mainmenu");
    } else if (buttonCode == "hardmode") {
        HARD_MODE_ENABLED = !HARD_MODE_ENABLED;
        _hardModeToggleButton->setLabelText(HARD_MODE_ENABLED ? "hard mode: on" : "hard mode: off");
    } else if (buttonCode == "audiosettings_pause") {
        _pauseMenu_settings->hide();
        _audioMenu->show();
    } else if (buttonCode == "audiosettings_main") {
        _startMenu_settings->hide();
        _audioMenu->show();
    } else if (buttonCode == "back_audio") {
        _audioMenu->hide();
        if (_gameStarted) _pauseMenu_settings->show();
        else _startMenu_settings->show();

        SFX_VOLUME = _sfxSlider->getValue();
        MUSIC_VOLUME = _musicSlider->getValue();
        updateSettingsFiles();
    } else if (buttonCode == "sfxslider") {
        SoundManager::setSfxVolume(_sfxSlider->getValue());
    } else if (buttonCode == "musicslider") {
        SoundManager::setMusicVolume(_musicSlider->getValue());
    }
}

void Game::keyPressed(sf::Keyboard::Key& key) {
    _ui->keyPressed(key);
    _player->keyPressed(key);
}

void Game::keyReleased(sf::Keyboard::Key& key) {
    switch (key) {
    case sf::Keyboard::F3:
        _showDebug = !_showDebug;
        ProjectilePoolManager::setDebug(_showDebug);
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
    case sf::Keyboard::F9:
        _hideUI = !_hideUI;
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
    }

    if (key == InputBindingManager::getKeyboardBinding(InputBindingManager::BINDABLE_ACTION::TOGGLE_PAUSE)) togglePauseMenu();
    else if (key == InputBindingManager::getKeyboardBinding(InputBindingManager::BINDABLE_ACTION::TOGGLE_INVENTORY)) toggleInventoryMenu();
    else if (key == InputBindingManager::getKeyboardBinding(InputBindingManager::BINDABLE_ACTION::INTERACT)) toggleShopMenu();

    _ui->keyReleased(key);
    _player->keyReleased(key);
}

void Game::mouseButtonPressed(const int mx, const int my, const int button) {
    _ui->mouseButtonPressed(mx, my, button);
}

void Game::mouseButtonReleased(const int mx, const int my, const int button) {
    _ui->mouseButtonReleased(mx, my, button);
    if (!_inventoryMenu->isActive() && !_shopMenu->isActive() && _gameStarted) _player->mouseButtonReleased(mx, my, button);
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
    if (button == InputBindingManager::getGamepadBinding(InputBindingManager::BINDABLE_ACTION::TOGGLE_PAUSE)) {
        togglePauseMenu();
    } else if (button == InputBindingManager::getGamepadBinding(InputBindingManager::BINDABLE_ACTION::TOGGLE_INVENTORY)) {
        toggleInventoryMenu();
    } else if (button == InputBindingManager::getGamepadBinding(InputBindingManager::BINDABLE_ACTION::INTERACT)) {
        toggleShopMenu();
    } else if (button == GAMEPAD_BUTTON::LEFT_STICK || button == GAMEPAD_BUTTON::RIGHT_STICK) {
        if (_virtualKeyboardMenu_lower->isActive()) {
            _virtualKeyboardMenu_lower->hide();
            _virtualKeyboardMenu_upper->show();
        } else if (_virtualKeyboardMenu_upper->isActive()) {
            _virtualKeyboardMenu_upper->hide();
            _virtualKeyboardMenu_lower->show();
        }
    } else if (button == GAMEPAD_BUTTON::B && (_virtualKeyboardMenu_lower->isActive() || _virtualKeyboardMenu_upper->isActive())) {
        buttonPressed("virtkey:back");
    }

    if (_shopMenu->isActive()) _shopManager.controllerButtonReleased(button);
}

void Game::gamepadDisconnected() {
    interruptPause();
}

void Game::togglePauseMenu() {
    if (_gameStarted && !_commandMenu->isActive() && !_inventoryMenu->isActive() && !_shopMenu->isActive()) {
        if (_pauseMenu->isActive()) {
            _pauseMenu->hide();
            _isPaused = !_isPaused;
        } else if (!_pauseMenu_settings->isActive() && !_controlsMenu->isActive() && !_inputBindingsMenu->isActive() && !_statsMenu_pauseMenu->isActive() && !_audioMenu->isActive()) {
            _pauseMenu->show();
            _isPaused = !_isPaused;
        } else if (_pauseMenu_settings->isActive()) buttonPressed("back_pausesettings");
        else if (_controlsMenu->isActive()) buttonPressed("back_controls");
        else if (_inputBindingsMenu->isActive()) buttonPressed("back_bindings");
        else if (_statsMenu_pauseMenu->isActive()) buttonPressed("back_stats_pause");
        else if (_audioMenu->isActive()) buttonPressed("back_audio");
    } else if (_gameStarted && _inventoryMenu->isActive()) toggleInventoryMenu();
    else if (_gameStarted && _shopMenu->isActive()) toggleShopMenu();

    if (_world.onEnemySpawnCooldown() && _isPaused) {
        _pauseStartTimeMillis = currentTimeMillis();
    } else if (_world.onEnemySpawnCooldown() && !_isPaused) {
        _world._cooldownStartTime = currentTimeMillis() - (_pauseStartTimeMillis - _world._cooldownStartTime);
    }

}

void Game::toggleInventoryMenu() {
    if (_gameStarted && !_commandMenu->isActive() && !_shopMenu->isActive() && !_pauseMenu->isActive()) {
        if (_inventoryMenu->isActive()) _inventoryMenu->hide();
        else _inventoryMenu->show();

        _player->_inventoryMenuIsOpen = _inventoryMenu->isActive();

        // This causes a problem but i keep not writing down what it is DON'T UNCOMMENT THIS
        //_isPaused = _inventoryMenu->isActive();
    } else if (_gameStarted && _shopMenu->isActive()) toggleShopMenu();
}

void Game::toggleShopMenu() {
    if (_world.playerIsInShop() && _shopKeep->isActive() && !_inventoryMenu->isActive() && !_shopMenu->isActive() && !_isPaused) {
        for (auto& entity : _world.getEntities()) {
            if (entity->isActive() && entity->getEntityType() == "shopkeep") {
                if (_player->getHitBox().intersects(entity->getHitBox())) {
                    _shopMenu->show();

                    std::string controlsMsg = "Left click to buy/sell 1 item\nRight click to buy/sell a stack\nMiddle click to buy/sell 25";
                    if (GamePad::isConnected()) controlsMsg = "Press A to buy/sell 1 item\nPress Y to buy/sell a stack\nPress down on the right joystick to buy/sell 25";
                    MessageManager::displayMessage(controlsMsg, 5);

                    if (GamePad::isConnected()) MessageManager::displayMessage("Use the bumpers to switch between buying and selling", 8);
                    break;
                }
            }
        }
    } else if (_shopMenu->isActive()) {
        _shopMenu->hide();
    } else if (!_shopMenu->isActive() && _inventoryMenu->isActive()) {
        toggleInventoryMenu();
    }

    _player->_inventoryMenuIsOpen = _shopMenu->isActive();
}

void Game::interruptPause() {
    if (!_isPaused) {
        if (_inventoryMenu->isActive()) toggleInventoryMenu();
        if (_shopMenu->isActive()) toggleShopMenu();
        togglePauseMenu();
    }
}

void Game::onPlayerDeath() {
    constexpr long long deathCallCooldown = 1000LL;
    if (currentTimeMillis() - _lastPlayerDeathCallTime > deathCallCooldown) {
        bool tutorialComplete = Tutorial::isCompleted();

        StatManager::increaseStat(TIMES_DIED, 1.f);

        if (tutorialComplete) {
            SaveManager::deleteSaveFile();
            _bossHUDMenu->hide();
            _HUDMenu->hide();
            if (_inventoryMenu->isActive()) toggleInventoryMenu();
            if (_shopMenu->isActive()) toggleShopMenu();
            _gameStarted = false;

            _waveReachedLabel->setText("you made it to wave " + std::to_string(_world._currentWaveNumber));
            std::string statsText = "Stats:\n\n\n";
            generateStatsString(statsText, false, false);
            _statsLabel_deathMenu->setText(statsText);
            _deathMenu->show();
        } else {
            MessageManager::displayMessage("You died :(\nYou made it to wave " + std::to_string(_world._currentWaveNumber), 5);
            MessageManager::displayMessage("TIP: Be sure to dodge in order to help keep distance between enemies and yourself", 8);
            int playerPennyIndex = _player->getInventory().findItem(Item::PENNY.getId());
            if (playerPennyIndex != NO_ITEM) _player->getInventory().removeItem(Item::PENNY.getId(), _player->getInventory().getItemAmountAt(playerPennyIndex));

            HARD_MODE_ENABLED = false;

            _cmdPrompt->unlock();
            _cmdPrompt->processCommand("removedroppeditems");
            _cmdPrompt->processCommand("killenemies");
            _cmdPrompt->processCommand("setmaxhp:100");
            _cmdPrompt->processCommand("addhp:100");
            _cmdPrompt->processCommand("respawn");

            if (!DEBUG_MODE) _cmdPrompt->lock();

            PLAYER_SCORE = 1.f;
            _world.setMaxActiveEnemies(INITIAL_MAX_ACTIVE_ENEMIES);
            _world._enemiesSpawnedThisRound = 0;
            _world._waveCounter = 0;
            _world._currentWaveNumber = 1;
            _world._maxEnemiesReached = false;

            _world._isPlayerInShop = false;

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
            _player->_speedMultiplier = 0.f;

            _world.resetChunks();
            _world.loadChunksAroundPlayer();
            _world.resetEnemySpawnCooldown();
            _world.startNewGameCooldown();

            if (_player->getInventory().findItem(Item::SLIME_BALL.getId()) == NO_ITEM) {
                std::shared_ptr<DroppedItem> droppedSlimeBall
                    = std::shared_ptr<DroppedItem>(new DroppedItem(
                        sf::Vector2f(_player->getPosition().x, _player->getPosition().y - 48), 2, Item::SLIME_BALL.getId(), 1, Item::SLIME_BALL.getTextureRect())
                        );
                droppedSlimeBall->setWorld(&_world);
                droppedSlimeBall->loadSprite(_world.getSpriteSheet());
                _world.addEntity(droppedSlimeBall);

                Tutorial::reset();
            }
        }
    }

    _lastPlayerDeathCallTime = currentTimeMillis();
}

void Game::startLoading() {
    _gameLoading = true;

    boost::random::mt19937 gen = boost::random::mt19937();
    gen.seed(currentTimeMillis());
    boost::random::uniform_int_distribution<> messageDist(0, _loadingScreenMessages.size() - 1);
    _loadingScreenMessageIndex = messageDist(gen);
    _frameCounter = 0;
}

void Game::loadLoadingScreenMessages() {
    const std::string path = "res/lsm.txt";
    std::ifstream in(path);

    if (!in.good()) {
        MessageManager::displayMessage("Could not find lsm.txt", 5, WARN);
        in.close();
    } else {
        std::string line;
        while (getline(in, line)) {
            _loadingScreenMessages.push_back(line);
        }
        in.close();
    }

    _loadingScreenMessages.push_back("loading");

    _loadingScreenMessageIndex = randomInt(0, _loadingScreenMessages.size() - 1);
}

void Game::loadTips() {
    const std::string path = "res/tips.txt";
    std::ifstream in(path);

    if (!in.good()) {
        MessageManager::displayMessage("Could not find tips.txt", 5, WARN);
        in.close();
    } else {
        std::string line;
        while (getline(in, line)) {
            _tips.push_back(line);
        }
        in.close();
    }
}

void Game::textEntered(sf::Uint32 character) {
    _ui->textEntered(character);
}

void Game::displayStartupMessages() const {

}

void Game::autoSave() {
    if (AUTOSAVE_ENABLED && !_world.playerIsInShop() && currentTimeMillis() - _lastAutosaveTime >= AUTOSAVE_INTERVAL_SECONDS * 1000) {
        SaveManager::saveGame(false);
        _lastAutosaveTime = currentTimeMillis();
        MessageManager::displayMessage("Autosaved", 0, DEBUG);
        MessageManager::displayMessage(">>" + splitString(SaveManager::getCurrentSaveFileName(), ".")[0] + ">>SFN", 0, DEBUG);
    }
}

void Game::generateStatsString(std::string& statsString, bool overall, bool useUnderscores) {
    for (int i = 0; i < StatManager::NUM_STATS; i++) {
        const std::string statName = STAT_NAMES[(STATISTIC)i];
        float statValue = overall ? StatManager::getOverallStat((STATISTIC)i) : StatManager::getStatThisSave((STATISTIC)i);

        std::string unit = "";
        if (stringStartsWith(statName, "Distance")) {
            unit = " meters";
            if (statValue >= 1000) {
                statValue /= 1000;
                unit = " kilometers";
            }
        }
        const std::string statString = unit != "" ? trimString(std::to_string(statValue)) : std::to_string((int)statValue);

        statsString += statName + ":  " + statString + unit + (useUnderscores ? "\n_______________\n" : "\n\n");
    }
}

void Game::onSteamOverlayActivated(GameOverlayActivated_t* pCallback) {
    if (pCallback->m_bActive) {
        MessageManager::displayMessage("Steam overlay opened", 2, DEBUG);
        interruptPause();
    } else MessageManager::displayMessage("Steam overlay closed", 2, DEBUG);
}

void Game::disableGamepadInput(std::shared_ptr<UIMenu> menu) {
    menu->useGamepadConfiguration = false;
    for (auto& element : menu->getElements()) {
        element->blockControllerInput = true;
    }
}

void Game::enableGamepadInput(std::shared_ptr<UIMenu> menu) {
    menu->useGamepadConfiguration = true;
    for (auto& element : menu->getElements()) {
        element->blockControllerInput = false;
    }
}

void Game::initVirtualKeyboard() {
    const std::string row0_lower = "1234567890-=\b";
    const std::string row0_upper = "!@#$%^&*()_+\b";
    const std::string row1_lower = " qwertyuiop[]";
    const std::string row1_upper = " QWERTYUIOP{}";
    const std::string row2_lower = " asdfghjkl;'€";
    const std::string row2_upper = " ASDFGHJKL:\"€";
    const std::string row3_lower = "§ zxcvbnm,./ ";
    const std::string row3_upper = "§ ZXCVBNM<>? ";

    const std::vector<std::string> lowerRows = {row0_lower, row1_lower, row2_lower, row3_lower};
    const std::vector<std::string> upperRows = {row0_upper, row1_upper, row2_upper, row3_upper };

    constexpr float keyboaryX = 18.f;
    constexpr float keyboardY = 60.f;
    const std::vector<sf::Vector2f> rowCoords = { {
            keyboaryX, keyboardY}, {keyboaryX, keyboardY + 8.f}, {keyboaryX, keyboardY + 15.f}, {keyboaryX, keyboardY + 22.f} 
    };

    std::shared_ptr<UIButton> spaceBar = std::shared_ptr<UIButton>(new UIButton(
        45, 93, 10, 3, "space", _font, this, "virtkey:space"
    ));
    spaceBar->setSelectionId(4 * 13);
    _virtualKeyboardMenu_lower->addElement(spaceBar);
    _virtualKeyboardMenu_upper->addElement(spaceBar);

    auto initKeyboard = [&](std::shared_ptr<UIMenu> keyboard, const std::vector<std::string> rows) {
        constexpr float keyWidth = 5.f;
        constexpr float keyHeight = 5.f;
        std::vector<std::vector<int>> grid;
        for (int row = 0; row < rows.size(); row++) {
            grid.push_back({});

            const float x = rowCoords.at(row).x;
            const float y = rowCoords.at(row).y;
            for (int i = 0; i < rows.at(row).length(); i++) {
                const std::string rowString = rows.at(row);
                std::string character(1, rowString.at(i));

                if (character == "\b") character = "back";
                else if (character == "€") character = "done";
                else if (character == "§") character = "caps";

                std::shared_ptr<UIButton> keyButton = std::shared_ptr<UIButton>(new UIButton(
                    x + keyWidth * i, y, keyWidth, keyHeight, character, _font, this, "virtkey:" + character
                ));
                keyButton->setSelectionId(i + row * 13);
                grid.at(row).push_back(keyButton->getSelectionId());

                keyboard->addElement(keyButton);
            }
        }
        grid.push_back({ {spaceBar->getSelectionId()} });

        keyboard->useGamepadConfiguration = true;
        keyboard->defineSelectionGrid(grid);

        _ui->addMenu(keyboard);
    };

    initKeyboard(_virtualKeyboardMenu_lower, lowerRows);
    initKeyboard(_virtualKeyboardMenu_upper, upperRows);
}