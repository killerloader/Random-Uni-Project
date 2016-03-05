#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

class Tileset;

class Animation
{
public:
	Animation();
	~Animation();

	Tileset* MyTileset;
};

