#ifndef _GAME_H
#define _GAME_H

#include <SFML/Graphics.hpp>
#include "World.h"
#include "UIHandler.h"
#include "UIButton.h"

constexpr int SCALE = 4;
constexpr int WIDTH = 360; //360
constexpr int HEIGHT = 202; //202

class Game : public UIButtonListener {
public:
	Game(sf::View* camera, sf::RenderWindow* window);

	void update();

	void draw(sf::RenderTexture& surface);
	void drawUI(sf::RenderTexture& surface);
	
	void buttonPressed(std::string buttonCode);

	void keyPressed(sf::Keyboard::Key& key);
	void keyReleased(sf::Keyboard::Key& key); 
	
	void mouseButtonPressed(const int mx, const int my);
	void mouseButtonReleased(const int mx, const int my);
	void mouseMoved(const int mx, const int my);

	void textEntered(sf::Uint32 character);

private:
	void initUI();

	bool _isPaused = false;

	sf::RenderWindow* _window;

	sf::View* _camera;

	UIHandler _ui;
	std::shared_ptr<UIMenu> _pauseMenu = std::shared_ptr<UIMenu>(new UIMenu());

	sf::Font _font;
	sf::Text _fpsLabel;
	sf::Text _activeChunksLabel;
	sf::Text _seedLabel;

	sf::Clock _clock;
	unsigned int _frameCounter = 0;
	
	std::shared_ptr<sf::Texture> _spriteSheet = std::shared_ptr<sf::Texture>(new sf::Texture());
	std::shared_ptr<Player> _player = std::shared_ptr<Player>(new Player(sf::Vector2f(0, 0)));
	World _world = World(_player);


};

#endif 
