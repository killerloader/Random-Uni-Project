#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

using namespace std;

struct Map;
class mapObject;

class otherPlayer
{
public:
	otherPlayer();
	~otherPlayer();

	float x, y;
};

