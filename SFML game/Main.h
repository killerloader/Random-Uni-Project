#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

using namespace std;

#include "MapMaker.h"
#include "fileManager.h"
#include "Maps.h"
#include "mapObject.h"
#include "otherPlayer.h"
#include "PlayerObject.h"

class PlayerObject;
class MapMaker;
struct Map;
class fileManager;
class mapObject;
class otherPlayer;

class WrapperClass
{
public:
	vector<otherPlayer*> otherPlayers;
	sf::Event event;
	int curmapID;
	void LimitVariable(int, int, int&);
	sf::RenderWindow &RenderRef;
	Map* MapPtr;//Maybe vector if more than one is loaded at a time?
	PlayerObject* PlrPtr;
	MapMaker* MMPtr;
	fileManager* FMPtr;
	WrapperClass(sf::RenderWindow &RenderRef_);
	sf::TcpListener listener;
	sf::TcpSocket* client;
	vector<sf::TcpSocket*> clients;
	sf::TcpSocket socket;
	//vector<int> clientIds;
	//bool idFree[256];
	bool connected = false;
	bool online = false;
};