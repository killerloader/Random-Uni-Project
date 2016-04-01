#pragma once

class RoomManager;

class BaseRoom
{
public:
	BaseRoom(RoomManager& rmMngrRef);
	virtual ~BaseRoom();

	virtual void update() = 0;
	virtual void draw() = 0;

	RoomManager& m_roomManagerRef;
};