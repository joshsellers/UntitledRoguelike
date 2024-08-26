#ifndef _GAME_H
#define _GAME_H

#include <SFML/Graphics.hpp>
#include "../world/World.h"
#include "../ui/UIHandler.h"
#include "../ui/UIButton.h"
#include "../world/ParticleSystem.h"
#include "gamepad/GamePad.h"
#include "../ui/UIAttributeMeter.h"
#include "../ui/UITextField.h"
#include "../ui/UICommandPrompt.h"
#include "../world/entities/ShopKeep.h"
#include "../inventory/ShopManager.h"
#include "../world/entities/ShopArrow.h"
#include "../ui/UILabel.h"
#include "../../SteamworksHeaders/steam_api.h"


const std::string GAME_NAME = "rolmi";

class Game : public UIButtonListener, public GamePadListener {
public:
	Game(sf::View* camera, sf::RenderWindow* window);

	void update();

	void draw(sf::RenderTexture& surface);
	void drawUI(sf::RenderTexture& surface);
	
	void buttonPressed(std::string buttonCode);

	void keyPressed(sf::Keyboard::Key& key);
	void keyReleased(sf::Keyboard::Key& key); 
	
	void mouseButtonPressed(const int mx, const int my, const int button);
	void mouseButtonReleased(const int mx, const int my, const int button);
	void mouseMoved(const int mx, const int my);
	void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);

	void controllerButtonPressed(GAMEPAD_BUTTON button);
	void controllerButtonReleased(GAMEPAD_BUTTON button);
	void gamepadDisconnected();

	void textEntered(sf::Uint32 character);

private:
	void initUI();

	bool _hideUI = false;
	bool _showDebug = false;
	bool _isPaused = false;

	bool _gameStarted = false;
	bool _gameLoading = false;
	
	long long _lastCooldownUpdateTime = 0;
	void displayEnemyWaveCountdownUpdates();
	bool _showWaveMeter = true;

	sf::RenderWindow* _window;

	sf::View* _camera;

	std::shared_ptr<UIHandler> _ui = std::shared_ptr<UIHandler>(new UIHandler());
	std::shared_ptr<UIMenu> _pauseMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _pauseMenu_settings = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _inventoryMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _shopMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _commandMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _HUDMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _startMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _startMenu_settings = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _newGameMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _joinGameMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _messageDispMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _loadGameMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _controlsMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _inputBindingsMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _bossHUDMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _statsMenu_mainMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _statsMenu_pauseMenu = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _virtualKeyboardMenu_lower = std::shared_ptr<UIMenu>(new UIMenu());
	std::shared_ptr<UIMenu> _virtualKeyboardMenu_upper = std::shared_ptr<UIMenu>(new UIMenu());

	std::shared_ptr<UILabel> _overallStatsLabel_mainMenu;
	std::shared_ptr<UILabel> _overallStatsLabel_pauseMenu;
	std::shared_ptr<UILabel> _currentSaveStatsLabel;

	std::shared_ptr<UIAttributeMeter> _bossHPMeter;

	std::shared_ptr<UIButton> _completeTutorialButton_startSettings;

	std::shared_ptr<UIButton> _vsyncToggleButton_mainMenu;
	std::shared_ptr<UIButton> _vsyncToggleButton_pauseMenu;

	std::shared_ptr<UITextField> _worldNameField;
	std::shared_ptr<UITextField> _seedField;
	std::shared_ptr<UITextField> _steamNameField;

	std::shared_ptr<UICommandPrompt> _cmdPrompt;

	int _maxMagPercentage = 100;
	std::shared_ptr<UIAttributeMeter> _magazineMeter;
	std::shared_ptr<UIAttributeMeter> _staminaMeter;
	std::shared_ptr<UIAttributeMeter> _waveCounterMeter;

	std::shared_ptr<UILabel> _titleScreenBackground;

	sf::Font _font;
	sf::Text _versionLabel;
	sf::Text _fpsLabel;
	sf::Text _activeChunksLabel;
	sf::Text _entityCountLabel;
	sf::Text _enemyCountLabel;
	sf::Text _onEnemySpawnCooldownLabel;
	sf::Text _seedLabel;
	std::string _textSeed = "NONE";
	sf::Text _playerPosLabel;
	sf::Text _currentBiomeLabel;
	sf::Text _coinMagnetCountLabel;

	sf::Text _loadingStatusLabel;
	std::vector<std::string> _loadingScreenMessages;
	int _loadingScreenMessageIndex = 0;
	void loadLoadingScreenMessages();

	sf::Clock _clock;
	unsigned int _frameCounter = 0;
	
	std::shared_ptr<sf::Texture> _spriteSheet = std::shared_ptr<sf::Texture>(new sf::Texture());
	std::shared_ptr<Player> _player;

	std::shared_ptr<ShopKeep> _shopKeep;
	ShopManager _shopManager = ShopManager();

	ShopArrow _shopArrow;

	World _world;

	void onPlayerDeath();
	long long _lastPlayerDeathCallTime = 0LL;

	void togglePauseMenu();
	void toggleInventoryMenu();
	void toggleShopMenu();

	void interruptPause();

	void displayStartupMessages() const;

	void startLoading();

	void autoSave();
	long long _lastAutosaveTime = 0;

	void startGameplayMusic() const;

	void disableGamepadInput(std::shared_ptr<UIMenu> menu);
	void enableGamepadInput(std::shared_ptr<UIMenu> menu);
	void initVirtualKeyboard();
	std::string _armedTextFieldId = "";

	void generateStatsString(std::string& statsString, bool overall);

	STEAM_CALLBACK(Game, onSteamOverlayActivated, GameOverlayActivated_t);
};

#endif 
