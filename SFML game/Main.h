#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

using namespace std;

#include "MapMaker.h"
#include "Animation.h"
#include "fileManager.h"
#include "Maps.h"
#include "mapObject.h"
#include "otherPlayer.h"
#include "PlayerObject.h"
#include "MessagesHangle.h"
#include "fireBall.h"

/* Should not be needed as headers are above!
class PlayerObject;
class MapMaker;
struct Map;
class fileManager;
class mapObject;
class otherPlayer;
*/

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
	sf::Font MainFont;
	
	sf::TcpListener listener;
	sf::TcpSocket* client;
	vector<sf::TcpSocket*> clients;
	sf::TcpSocket socket;
	MessagesHangle MHandle;//Handle for messages for both client and server.

	char* ConnectIp;//000.000.000.000\0 (16 characters including null terminator at max.

	//vector<int> clientIds;
	//bool idFree[256];
	
	enum E_GameType
	{
		ET_Server,
		ET_Client
	};
	bool inMapMaker;
	bool mapMakerPermissions;
	bool connected;
	E_GameType gameType;
};