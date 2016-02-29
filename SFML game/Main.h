#pragma once
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

using namespace std;

#include "MapMaker.h"
#include "fileManager.h"
#include "Maps.h"
#include "mapObject.h"

struct PlayerObject;
class MapMaker;
struct Map;
class fileManager;
class mapObject;

struct WrapperClass
{
	sf::Event event;
	int curmapID = 0;
	void LimitVariable(int, int, int&);
	sf::RenderWindow &RenderRef;
	Map* MapPtr;//Maybe vector if more than one is loaded at a time?
	PlayerObject* PlrPtr;
	MapMaker* MMPtr;
	fileManager* FMPtr;
	WrapperClass(sf::RenderWindow &RenderRef_);
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
	float vspeed, hspeed, gravity, haccel, hspeedmax, hfric;
	void StepPlayer();
	void DrawPlayer();
	void ResetMovement();
	enum Edirection
	{
		E_left,E_right,E_up,E_down
	};
	void ContractDir(Edirection);
	//Positioning of player
	bool falling;
	float x, y;
	float xstart, ystart;
	sf::Rect<int> BoundBox;
	float SPD;
};