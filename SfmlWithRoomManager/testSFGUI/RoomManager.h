#pragma once

#include <stack>

class BaseRoom;
class MainFrame;

class RoomManager
{
public:
	void update();
	void draw();
	static RoomManager& Instance(MainFrame& MFrameRef);

	//Change to private and create functions to access.
	std::stack<BaseRoom*> rooms;

	MainFrame& MainFrameRef;
private:
	RoomManager(MainFrame& MFrameRef);
	~RoomManager();
	static RoomManager* thisInstance;
};