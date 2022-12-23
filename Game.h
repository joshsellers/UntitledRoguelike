#ifndef _GAME_H
#define _GAME_H

#include <SFML/Graphics.hpp>
#include "World.h"

constexpr int SCALE = 4;
constexpr int WIDTH = 360; //480
constexpr int HEIGHT = 202; //270

class Game {
public:
	Game(sf::View* camera);

	void update();

	void draw(sf::RenderTexture& surface);
	void drawUI(sf::RenderTexture& surface);

	void keyPressed(sf::Keyboard::Key& key);
	void keyReleased(sf::Keyboard::Key& key);

private:
	sf::View* _camera;

	sf::Font _font;
	sf::Text _fpsLabel;
	sf::Text _activeChunksLabel;

	sf::Clock _clock;
	unsigned int _frameCounter = 0;

	Player _player = Player(sf::Vector2f(0, 0));
	World _world = World(&_player);
};

#endif 
