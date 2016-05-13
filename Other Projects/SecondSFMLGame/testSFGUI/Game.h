#pragma once
#include <SFML/Graphics.hpp>

class Game
{
	Game();
	~Game();
	static Game* singleton;
public:

	void Run();
	void Step();
	void Draw();
	static Game& instance();

	sf::RenderWindow render_window;
};

