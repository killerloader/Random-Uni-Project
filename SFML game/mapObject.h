#pragma once
#include "Main.h"

class mapObject
{
public:
	mapObject(int, int, int);
	mapObject();
	~mapObject();

	int objectType;
	int tileID;
	int tileSetID;
};