#pragma once
#include "Main.h"

struct Map
{
	WrapperClass &WCR;
	int MapWidth, MapHeight;
	Map(WrapperClass &WCR_);
	vector<vector<int>> MapMatrix;
	vector<int> ObjectArray;
	void DrawMap(sf::View&);
	int ViewWidth, ViewHeight, ViewX, ViewY;
	int CellSize;
	sf::RectangleShape MapBorders[4];
	sf::RectangleShape SolidObj;
	void SetObject(int, int, int);
	void setupBorders();
	void drawBorders();
	int CheckCollision(sf::Rect<int> CheckRect, int X, int Y, int);
};
