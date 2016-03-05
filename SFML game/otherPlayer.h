#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

using namespace std;

struct Map;
class mapObject;
class WrapperClass;

class otherPlayer
{
public:
	otherPlayer(WrapperClass &WCR_);
	~otherPlayer();

	void step();
	void draw();

	WrapperClass &WCR;

	enum Edirection
	{
		E_left, E_right, E_up, E_down
	};

	int PID;
	vector<sf::Sprite> AfterImage;
	sf::Sprite PlayerImage;
	sf::Rect<int> BoundBox;
	bool falling;
	float vspeed, hspeed, gravity, haccel, haccelSlip, hspeedmax, hfric, x, y, xstart, ystart, SPD, hfricSlip;
	float xAct, yAct;
	int vspeedMax;
	int xdir_;

	char* myName;

	sf::Color MyCol = sf::Color();
	sf::Text NameText;

	void ChangeName(const char* NewName);
	void ContractDir(Edirection);
	void MovePlayer(float Xmove, float Ymove);
};

