#include "MainFrame.h"
#include "RoomManager.h"
#include "Room_MainMenu.h"

MainFrame* MainFrame::thisInstance = nullptr;

//Initialize roomManagerRef reference in initialiser list.
MainFrame::MainFrame() : m_roomManagerRef(RoomManager::Instance(*this))
{
	m_roomManagerRef.rooms.push(new Room_MainMenu(m_roomManagerRef));
}

MainFrame::~MainFrame()
{

}

void MainFrame::runGame()
{
	// Create SFML's window.
	sf::RenderWindow render_window(sf::VideoMode(600, 400), "Test Windowing Game");

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
		//Update the room manager.
		m_roomManagerRef.update();

		// Rendering.
		render_window.clear();

		//Call room manager to update the game.
		m_roomManagerRef.draw();

		//Display to window.
		render_window.display();

		//No rooms are open in the game, end the game.
		if (m_roomManagerRef.rooms.empty())
			render_window.close();
	}
}

//Can return a reference as there is no chance that it wont return the instance.
MainFrame& MainFrame::Instance()
{
	if (thisInstance == nullptr)
		thisInstance = new MainFrame;
	return *thisInstance;
}
