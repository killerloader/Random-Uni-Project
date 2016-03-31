#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <SFGUI/Widgets.hpp>

#include <SFML/Graphics.hpp>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

class HelloWorld {
public:
	// Our button click handler.
	void OnButtonClick();

	void Run();

private:
	// Create an SFGUI. This is required before doing anything with SFGUI.
	sfg::SFGUI m_sfgui;

	// Create the label pointer here to reach it from OnButtonClick().
	sfg::Label::Ptr m_label;
};

void HelloWorld::OnButtonClick() {
	m_label->SetText("Hello SFGUI, pleased to meet you!");
}

void HelloWorld::Run() {
	// Create SFML's window.
	sf::RenderWindow render_window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Hello world!");

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

		// Rendering.
		render_window.clear();
		m_sfgui.Display(render_window);
		render_window.display();
	}
}

int main() {
	HelloWorld hello_world;
	hello_world.Run();

	return 0;
}