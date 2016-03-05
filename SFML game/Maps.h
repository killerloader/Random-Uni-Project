#pragma once
#include "Main.h"

class mapObject;

struct Map
{
	WrapperClass &WCR;
	int MapWidth, MapHeight;
	Map(WrapperClass &WCR_);
	vector<vector<mapObject>> MapMatrix;
	vector<int> ObjectArray;
	void DrawMap(sf::View&);
	int ViewWidth, ViewHeight, ViewX, ViewY;
	int CellSize;
	sf::RectangleShape MapBorders[4];
	sf::RectangleShape SolidObj;
	void SetObject(int, int, int, int, int, bool = false);
	void setupBorders();
	void drawBorders();
	int CheckCollision(sf::Rect<int> CheckRect, int X, int Y, int);
	bool Map::isObject(int x, int y);
};
