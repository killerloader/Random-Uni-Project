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
		//UpdateSprites();
		//CreateCollisionMaps();
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
		//UpdateSprites();
		//CreateCollisionMaps();
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
		FrameTime = FrmTm;
	}

	void UpdateSprites()
	{
		TileSheetSprite.setTexture(*TileSheetTex);

		if (Tiles.size() != 0)
		{
			for (int i = 0; i < CellsX; i++)
				for (int ii = 0; ii < CellsY; ii++)
				{
					//Tiles[i + ii*CellsX].setTexture(*TileSheetTex);
					//Tiles[i + ii*CellsX].setTextureRect(sf::IntRect(sf::Vector2i(xOffset + (CSizeX + GapW) * i, yOffset + (CSizeY + GapH) * ii), sf::Vector2i(CSizeX, CSizeY)));
					Tiles[i + ii*CellsX].setTexture(ImgTex[i + ii*CellsX]);
					Tiles[i + ii*CellsX].setTextureRect(sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(CSizeX, CSizeY)));
					
				}
					
		}
		else
			for (int i = 0; i < CellsX; i++)
				for (int ii = 0; ii < CellsY; ii++)
					Tiles.emplace_back(*TileSheetTex, sf::IntRect(sf::Vector2i(xOffset + (CSizeX + GapW) * i, yOffset + (CSizeY + GapH) * ii), sf::Vector2i(CSizeX, CSizeY)));
	}

	void CreateCollisionMaps()
	{
		cout << "Creating pixel perfect collision map for tileset!" << endl;
		//sf::RenderTexture TempRenderTextures;
		//TempRenderTextures.create(32, 32);
		if(CollisionMaps.size()==0)
			for (int i = 0; i < CellsX; i++)
				for (int ii = 0; ii < CellsY; ii++)
				{
					CollisionMaps.emplace_back();
					ImgTex.emplace_back();
				}
					
		for (int i = 0; i < CellsX; i++)
			for (int ii = 0; ii < CellsY; ii++)
			{
				//cout << "test" << endl;
				//TempRenderTextures.clear();
				//Tiles[i + ii*CellsX].setPosition(0, 0);
				//TempRenderTextures.draw(Tiles[i + ii*CellsX]);
				//TempRenderTextures.display();
				//Tiles[i + ii*CellsX].setTextureRect(sf::IntRect(sf::Vector2i(xOffset + (CSizeX + GapW) * i, yOffset + (CSizeY + GapH) * ii), sf::Vector2i(CSizeX, CSizeY)));
				
				CollisionMaps[i + ii*CellsX].create(32, 32, sf::Color(255,255,255,0));
				CollisionMaps[i + ii*CellsX].copy(TileSheetTex->copyToImage(), 0,0, sf::IntRect(xOffset + (CSizeX + GapW) * i, yOffset + (CSizeY + GapH) * ii, 32, 32), true);
				ImgTex[i + ii*CellsX].loadFromImage(CollisionMaps[i + ii*CellsX]);
				//sf::Image test((*TileSheetTex).copyToImage(), 0, 0, sf::IntRect(xOffset + (CSizeX + GapW) * i, yOffset + (CSizeY + GapH) * ii, 32, 32), true);
				//CollisionMaps[CollisionMaps.size() - 1] = TempRenderTextures.getTexture().copyToImage();
			}
		//TempRenderTextures.clear();
	}

	vector<sf::Image> CollisionMaps;
	sf::Texture* TileSheetTex;
	vector<sf::Sprite> Tiles;
	vector<sf::Texture> ImgTex;
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
	sfg::Button::Ptr ExitMapMakerButton;

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
	sfg::CheckButton::Ptr GiveAllPermissions;


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
	void changeObjectType();
	void tickButtonPress(int);

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

