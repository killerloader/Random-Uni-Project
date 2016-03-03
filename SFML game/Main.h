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

struct PlayerObject;
class MapMaker;
struct Map;
class fileManager;
class mapObject;
class otherPlayer;

struct WrapperClass
{
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

struct PlayerObject
{
	vector<sf::Sprite> AfterImage;
	WrapperClass &WCR;
	sf::View PlayerView;
	sf::Texture PlayerTex;
	sf::Sprite PlayerImage;
	PlayerObject(WrapperClass &WCR_);
	void PollControls();
	void MovePlayer(float Xmove, float Ymove);
	float vspeed, hspeed, gravity, haccel, hspeedmax, hfric, x, y, xstart, ystart, SPD;
	float xDirOld, pressWOld;
	void StepPlayer();
	void DrawPlayer();
	void ResetMovement();
	void sendXChange();
	void sendJump();
	enum Edirection
	{
		E_left,E_right,E_up,E_down
	};
	void ContractDir(Edirection);
	//Positioning of player
	bool falling;
	sf::Rect<int> BoundBox;
};