#pragma once
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include "Main.h"
#include <sstream>
#include <string>


class WrapperClass;
class MapMaker;

struct TileSet
{
	TileSet(const char* FileName, int xOff, int yOff, int xGap, int yGap, int xCells, int yCells, sfg::ComboBox::Ptr TSL)
	{
		xOffset=xOff;
		yOffset=yOff;
		GapW=xGap;
		GapH=yGap;
		CellsX=xCells;
		CellsY=yCells;

		TileSheetTex.loadFromFile(FileName);
#ifdef MapMakerMode
		TSL->AppendItem(FileName);
#endif
		UpdateSprites();

	}

	void UpdateSprites()
	{
		TileSheetSprite.setTexture(TileSheetTex);

		if (Tiles.size() != 0)
		{
			for (int i = 0; i < CellsX; i++)
				for (int ii = 0; ii < CellsX; ii++)
				{
					Tiles[i + ii*CellsX].setTexture(TileSheetTex);
					Tiles[i + ii*CellsX].setTextureRect(sf::IntRect(sf::Vector2i(xOffset + (32 + GapW) * i, yOffset + (32 + GapH) * ii), sf::Vector2i(32, 32)));
				}
					
		}
		else
			for (int i = 0; i < CellsX; i++)
				for (int ii = 0; ii < CellsX; ii++)
					Tiles.emplace_back(TileSheetTex, sf::IntRect(sf::Vector2i(xOffset + (32 + GapW) * i, yOffset + (32 + GapH) * ii), sf::Vector2i(32, 32)));
	}

	sf::Texture TileSheetTex;
	vector<sf::Sprite> Tiles;
	sf::Sprite TileSheetSprite;
	string Name;
	int xOffset;
	int yOffset;
	int GapW;
	int GapH;
	int CellsX;
	int CellsY;
};

class MapMaker
{
private:
	void setBlock(int BLK);
	int getBlock();
public:
	//GUI stuff
	sfg::SFGUI sfgui;
	sfg::Desktop desktop;

	sf::View mmView;

	sf::RenderWindow mmWindow;
	sf::Event event;

	//Main Window
	sfg::Window::Ptr windowSFG;//Main window with all buttons
	sfg::Button::Ptr blockButtons[7];
	sfg::Button::Ptr lvlIdMinus;
	sfg::Button::Ptr lvlIdPlus;
	sfg::Entry::Ptr MapIDEntry;
	sfg::Label::Ptr MapIDLabel;
	sfg::Box::Ptr box;
	sfg::Box::Ptr MapBox;

	//Tile Window
	sfg::Canvas::Ptr TileCanvas;
	sfg::Window::Ptr windowTiles;//Window to select drawn tile.
	sfg::Box::Ptr TileBoxVert;
	//vector<sfg::Box::Ptr> TileBoxesHor;
	//vector<sfg::Image::Ptr> TileImages;

	sfg::ComboBox::Ptr TilesetList;
	
	vector<TileSet> TileSets;//Tilesets

	//Tiles extracted from tile sheets;

	bool WindowIsHovered[2];

	void loadTiles();
	void updateTiles();
	void loadTile(const char*);
	void changeTileSet();

	int MouseOverWindow=0;
	bool pressedOffScreen = false;//if pressed off screen, don't place blocks until released and pressed again on screen.
	
	//sfg::Button::Ptr button2;
	//sfg::ComboBox::Ptr DrpDown;
	//sfg::Box::Ptr box;
	sf::Clock clock;

	int curTile=0;
	int curTileSet = 1;

	vector<sf::String> BlockNames;
	void pressTiles(int);
	void buttonPressChangeMapID(int);
	void ButtonPress(int);
	void mouseEnterWindow(int);
	void mouseLeaveWindow(int);
	//---
	WrapperClass &WCR;
	void Step();
	void Draw();
	bool LoadMap(int);
	bool SaveMap(int);
	void ClearMap();
	void PollGUIEvents();
	int CurBlock = 1;
	sf::View MapMakrView;
	MapMaker(WrapperClass &WCR_);
	~MapMaker();
};

