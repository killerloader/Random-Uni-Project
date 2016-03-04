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
	float vspeed, hspeed, gravity, haccel, hspeedmax, hfric, x, y, xstart, ystart, SPD;
	int vspeedMax;
	int xdir_;

	sf::Color MyCol = sf::Color();

	void ContractDir(Edirection);
	void MovePlayer(float Xmove, float Ymove);
};

