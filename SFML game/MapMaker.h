#pragma once
#include "Main.h"
#include <sstream>

struct WrapperClass;

class MapMaker
{
private:
	void setBlock(int BLK);
	int getBlock();
public:
	//GUI stuff
	sfg::SFGUI sfgui;
	sfg::Desktop desktop;

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
	sfg::Window::Ptr windowTiles;//Window to select drawn tile.
	sfg::Box::Ptr TileBoxVert;
	vector<sfg::Box::Ptr> TileBoxesHor;
	vector<sfg::Image::Ptr> TileImages;
	
	vector<sf::Texture> GameTextures;
	vector<sf::Image> GameImages;
	vector<sf::Sprite> GameSprites;

	void loadTiles();
	void updateTiles();
	void loadTile(const char*);

	int MouseOverWindow=0;
	bool pressedOffScreen = false;//if pressed off screen, don't place blocks until released and pressed again on screen.
	
	//sfg::Button::Ptr button2;
	//sfg::ComboBox::Ptr DrpDown;
	//sfg::Box::Ptr box;
	sf::Clock clock;

	int curTile=1;

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

