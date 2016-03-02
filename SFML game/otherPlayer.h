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

	sf::RectangleShape PlayerImage;
	sf::Rect<int> BoundBox;
	bool falling;
	float vspeed, hspeed, gravity, haccel, hspeedmax, hfric, x, y, xstart, ystart, SPD;

	void ContractDir(Edirection);
	void MovePlayer(float Xmove, float Ymove);
};

