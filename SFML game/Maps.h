#pragma once
#include "Main.h"

class mapObject;

struct Map
{
	WrapperClass &WCR;
	int MapWidth, MapHeight;
	Map(WrapperClass &WCR_);
	vector<vector<mapObject>> MapMatrix;
	vector<vector<vector<mapObject>>> BackgroundMatrix;//Can Be layered
	vector<int> OrderedBackgroundLayers;
	vector<int> BackgroundLayers;
	int addLayer(int Layer_);
	vector<int> ObjectArray;
	void DrawMap(sf::View&);
	enum E_Ground
	{
		E_vForeground,
		E_vBackground,
		E_vMiddleground
	};
	void Drawbackground(sf::View&, E_Ground);
	int ViewWidth, ViewHeight, ViewX, ViewY;
	int CellSize;
	sf::RectangleShape MapBorders[4];
	sf::RectangleShape SolidObj;
	void SetObject(int, int, int, int, int, bool PP = false, int = -1, bool = false);
	void SetBG(int, int, int, int, int, int = -1, bool = false);
	void setupBorders();
	void drawBorders();
	void resetMap();
	int CheckCollision(sf::Rect<int> CheckRect, int X, int Y, int);
	void expandMap(int newSizeX, int newSizeY);
	bool isObject(int x, int y);
	bool isBg(int x, int y, int Layer);
};
