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
	TileSet(const char* FileName, int xOff, int yOff, int xGap, int yGap, int xCells, int yCells, sfg::ComboBox::Ptr TSL, int cellSizeX = 32, int cellSizeY = 32)
	{
		xOffset = xOff;
		yOffset = yOff;
		GapW = xGap;
		GapH = yGap;
		CellsX = xCells;
		CellsY = yCells;
		CSizeX = cellSizeX;
		CSizeY = cellSizeY;
		TileSheetTex = new sf::Texture;
		if (!TileSheetTex->loadFromFile(FileName))
			cout << "Could not load image" << endl;
		TSL->AppendItem(FileName);
		UpdateSprites();
	}

	TileSet(const char* FileName, int xOff, int yOff, int xGap, int yGap, int xCells, int yCells, int cellSizeX, int cellSizeY, int FrmTm)
	{
		xOffset = xOff;
		yOffset = yOff;
		GapW = xGap;
		GapH = yGap;
		CellsX = xCells;
		CellsY = yCells;
		CSizeX = cellSizeX;
		CSizeY = cellSizeY;
		TileSheetTex = new sf::Texture;
		if (!TileSheetTex->loadFromFile(FileName))
			cout << "Could not load image" << endl;
		FrameTime = FrmTm;
		UpdateSprites();
	}

	TileSet(sf::Texture* TexPtr, int xOff, int yOff, int xGap, int yGap, int xCells, int yCells, sfg::ComboBox::Ptr TSL, const char* TSName = " ", int cellSizeX = 32, int cellSizeY = 32)
	{
		xOffset = xOff;
		yOffset = yOff;
		GapW = xGap;
		GapH = yGap;
		CellsX = xCells;
		CellsY = yCells;
		CSizeX = cellSizeX;
		CSizeY = cellSizeY;
		TileSheetTex = TexPtr;
		UpdateSprites();
		TSL->AppendItem(TSName);
	}

	TileSet(sf::Texture* TexPtr, int xOff, int yOff, int xGap, int yGap, int xCells, int yCells, const char* TSName, int cellSizeX, int cellSizeY, int FrmTm)
	{
		xOffset = xOff;
		yOffset = yOff;
		GapW = xGap;
		GapH = yGap;
		CellsX = xCells;
		CellsY = yCells;
		CSizeX = cellSizeX;
		CSizeY = cellSizeY;
		TileSheetTex = TexPtr;
		UpdateSprites();
		FrameTime = FrmTm;
	}

	void UpdateSprites()
	{
		//Texture wont change location, so no point in calling this more than once.
		TileSheetSprite.setTexture(*TileSheetTex);

		for (int ii = 0; ii < CellsY; ii++)
			for (int i = 0; i < CellsX; i++)
				Tiles.emplace_back(*TileSheetTex, sf::IntRect(sf::Vector2i(xOffset + (CSizeX + GapW) * i, yOffset + (CSizeY + GapH) * ii), sf::Vector2i(CSizeX, CSizeY)));
	}

	void CreateCollisionMaps()
	{
		//Doesn't require a copy of the texture, so no point in calling more than once.
		for (int ii = 0; ii < CellsY; ii++)
			for (int i = 0; i < CellsX; i++)
			{
				CollisionMaps.emplace_back();
				CollisionMaps[CollisionMaps.size() -1].create(32, 32);
				CollisionMaps[CollisionMaps.size() - 1].copy(TileSheetTex->copyToImage(), 0, 0, sf::IntRect(xOffset + (CSizeX + GapW) * i, yOffset + (CSizeY + GapH) * ii, 32, 32));
			}
	}

	vector<sf::Image> CollisionMaps;
	sf::Texture* TileSheetTex;
	vector<sf::Sprite> Tiles;
	sf::Sprite TileSheetSprite;
	string Name;
	int xOffset;
	int yOffset;
	int GapW;
	int GapH;
	int CellsX;
	int CellsY;
	int CSizeX, CSizeY;
	int FrameTime;//For use with animations.
};

class MapMaker
{
private:
	void setBlock(int BLK);
	int getBlock();
public:
	void MapMaker::exitMapMaker();
	void MapMaker::enterMapMaker();
	bool MapMaker::isInMapMaker();
	//GUI stuff
	sfg::SFGUI sfgui;
	sfg::Desktop desktop;

	sf::View mmView;

	sf::RenderWindow mmWindow;
	sf::Event event;

	//Main Window
	sfg::Window::Ptr windowSFG;//Main window with all buttons
	sfg::Button::Ptr blockButtons[7];
	sfg::ComboBox::Ptr ObjectTypeList;

	sfg::Button::Ptr lvlIdMinus;
	sfg::Button::Ptr lvlIdPlus;
	sfg::Entry::Ptr MapIDEntry;
	sfg::Box::Ptr box;
	sfg::Box::Ptr MapBox;
	sfg::Button::Ptr ConsoleButton;
	sfg::CheckButton::Ptr PixelPerfectToggle;
	sfg::CheckButton::Ptr PlacingObjects;
	sfg::CheckButton::Ptr PlacingTiles;
	sfg::Button::Ptr LayersButton;
	sfg::Button::Ptr ExitMapMakerButton;

	//Layer Window
	sfg::Window::Ptr LayerWindow;//Window to select drawn tile.
	sfg::Box::Ptr LWTileBoxVert;
	sfg::Entry::Ptr NewLayerEntry;
	sfg::Button::Ptr AddLayerButton;
	sfg::Button::Ptr ClearLayerButton;
	sfg::ComboBox::Ptr LayerList;
	sfg::CheckButton::Ptr OnlyShowThisLayer;

	//Console window
	sfg::Window::Ptr CommandWindow;//Window to select drawn tile.
	sfg::Box::Ptr CWTileBoxVert;
	sfg::Entry::Ptr CommandEntry;
	sfg::Button::Ptr SendCommand;
	sfg::ScrolledWindow::Ptr CommandLog;
	sfg::Box::Ptr CLogBoxVert;

	void sendCommand();


	//Tile Window
	sfg::Canvas::Ptr TileCanvas;
	sfg::Window::Ptr windowTiles;//Window to select drawn tile.
	sfg::Box::Ptr TileBoxVert;

	//Settings
	sfg::CheckButton::Ptr ShowObjectTypes;
	sfg::CheckButton::Ptr HideObjectSprites;
	sfg::CheckButton::Ptr GiveAllPermissions;


	//vector<sfg::Box::Ptr> TileBoxesHor;
	//vector<sfg::Image::Ptr> TileImages;

	sfg::ComboBox::Ptr TilesetList;
	
	vector<TileSet> TileSets;//Tilesets

	//Tiles extracted from tile sheets;

	bool WindowIsHovered[2];

	void loadTiles();
	void loadTile(const char*);
	void changeTileSet();
	void changeObjectType();
	void tickButtonPress(int);
	int curTileLayer = 0;

	bool viewObjectTypes = false;
	bool pixelPerfectBlocks = false;

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

