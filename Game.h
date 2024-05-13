#ifndef _GAME_H
#define _GAME_H

#include <SFML/Graphics.hpp>
#include "World.h"
#include "UIHandler.h"
#include "UIButton.h"
#include "ParticleSystem.h"
#include "GameController.h"
#include "UIAttributeMeter.h"
#include "UITextField.h"
#include "UICommandPrompt.h"
#include "ShopKeep.h"
#include "ShopManager.h"
#include "ShopArrow.h"

const std::string VERSION = "0.0353";

class Game : public UIButtonListener, public GameControllerListener, public MultiplayerMessageListener {
public:
	Game(sf::View* camera, sf::RenderWindow* window);

	void update();

	void draw(sf::RenderTexture& surface);
	void drawUI(sf::RenderTexture& surface);
	
	void buttonPressed(std::string buttonCode);

	void messageReceived(MultiplayerMessage message, SteamNetworkingIdentity identityPeer);

	void keyPressed(sf::Keyboard::Key& key);
	void keyReleased(sf::Keyboard::Key& key); 
	
	void mouseButtonPressed(const int mx, const int my, const int button);
	void mouseButtonReleased(const int mx, const int my, const int button);
	void mouseMoved(const int mx, const int my);
	void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);

	void controllerButtonPressed(CONTROLLER_BUTTON button);
	void controllerButtonReleased(CONTROLLER_BUTTON button);

	void textEntered(sf::Uint32 character);

	void disconnectMultiplayer();

private:
	void initUI();

	bool _showDebug = false;
	bool _isPaused = false;

	bool _gameStarted = false;
	
	long long _lastCooldownUpdateTime = 0;
	void displayEnemyWaveCountdownUpdates();

	void sendMultiplayerUpdates();
	bool _connectedAsClient = false;
	unsigned int _packetsOutThisTick = 0;
	long long _lastPacketsOutCountReset = 0;

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

	std::shared_ptr<UITextField> _worldNameField;
	std::shared_ptr<UITextField> _seedField;
	std::shared_ptr<UITextField> _steamNameField;

	std::shared_ptr<UICommandPrompt> _cmdPrompt;

	std::shared_ptr<UIAttributeMeter> _magazineMeter;

	sf::Font _font;
	sf::Text _versionLabel;
	sf::Text _fpsLabel;
	sf::Text _activeChunksLabel;
	sf::Text _entityCountLabel;
	sf::Text _onEnemySpawnCooldownLabel;
	sf::Text _seedLabel;
	std::string _textSeed = "NONE";
	sf::Text _playerPosLabel;
	sf::Text _currentBiomeLabel;

	sf::Clock _clock;
	unsigned int _frameCounter = 0;
	
	std::shared_ptr<sf::Texture> _spriteSheet = std::shared_ptr<sf::Texture>(new sf::Texture());
	std::shared_ptr<Player> _player;

	std::shared_ptr<ShopKeep> _shopKeep;
	ShopManager _shopManager = ShopManager();

	ShopArrow _shopArrow;

	World _world;

	void togglePauseMenu();
	void toggleInventoryMenu();
	void toggleShopMenu();
};

#endif 
