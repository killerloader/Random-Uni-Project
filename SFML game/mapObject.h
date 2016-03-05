#pragma once
#include "Main.h"

class mapObject
{
public:
	mapObject(int, int, int, bool);
	mapObject();
	~mapObject();

	int objectType;
	int tileID;
	int tileSetID;
	bool pixelPerfect;
};