#pragma once
#include <SFML/Graphics.hpp>

class RoomManager;

class MainFrame
{
public:
	void runGame();
	static MainFrame& Instance();

	//Other object pointers
	RoomManager &m_roomManagerRef;
private:
	MainFrame();
	~MainFrame();
	static MainFrame* thisInstance;
};

