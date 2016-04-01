#include "RoomManager.h"
#include "BaseRoom.h"

RoomManager* RoomManager::thisInstance = nullptr;

RoomManager::RoomManager(MainFrame& MFrameRef) : MainFrameRef(MFrameRef)
{

}

RoomManager::~RoomManager()
{

}

void RoomManager::update()
{
	rooms.top()->update();
}

void RoomManager::draw()
{
	rooms.top()->draw();
}

//Can return a reference as there is no chance that it wont return the instance.
RoomManager& RoomManager::Instance(MainFrame& MFrameRef)
{
	if (RoomManager::thisInstance == nullptr)
		RoomManager::thisInstance = new RoomManager(MFrameRef);
	return *RoomManager::thisInstance;
}

