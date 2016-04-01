#pragma once

#include "BaseRoom.h"

class Room_MainMenu : public BaseRoom
{
public:
	Room_MainMenu(RoomManager& rmMgrRef);
	~Room_MainMenu();

	void update();
	void draw();
};

