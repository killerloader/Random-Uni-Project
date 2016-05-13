#include "Game.h"

Game* Game::singleton = nullptr;

Game::Game()
{

}

Game::~Game()
{

}

Game& Game::instance()
{
	if (singleton == nullptr)
		singleton = new Game();

	return *singleton;
}

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void Game::Run()
{
	// Create SFML's window.
	render_window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Game");

	// Main loop!
	sf::Event event;
	sf::Clock clock;

	while (render_window.isOpen()) {
		// Event processing.
		while (render_window.pollEvent(event)) {

			// If window is about to be closed, leave program.
			if (event.type == sf::Event::Closed)
				render_window.close();
		}

		//Step Function
		Step();

		// Rendering.
		render_window.clear();

		//Draw Function
		Draw();

		//Finish rendering and display (switches buffers)
		render_window.display();
	}
}

void Game::Step()
{

}

void Game::Draw()
{

}