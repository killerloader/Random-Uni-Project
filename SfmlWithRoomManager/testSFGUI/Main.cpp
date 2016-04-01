#include "MainFrame.h"

int main() {
	
	//Run the main game instance as a singleton.
	MainFrame::Instance().runGame();
	return 0;
}