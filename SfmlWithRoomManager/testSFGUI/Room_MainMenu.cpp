#include "Room_MainMenu.h"
#include <iostream>

Room_MainMenu::Room_MainMenu(RoomManager& rmMgrRef) : BaseRoom(rmMgrRef)
{

}

Room_MainMenu::~Room_MainMenu()
{

}

void Room_MainMenu::update()
{
	std::cout << "Updating main menu..." << std::endl;
}

void Room_MainMenu::draw()
{

}