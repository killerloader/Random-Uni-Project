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
	sfg::Window::Ptr windowSFG;
	sfg::Button::Ptr blockButtons[7];
	sfg::Entry::Ptr MapIDEntry;
	sfg::Label::Ptr MapIDLabel;

	int MouseOverWindow=0;
	bool pressedOffScreen = false;//if pressed off screen, don't place blocks until released and pressed again on screen.

	sfg::Box::Ptr box;
	//sfg::Button::Ptr button2;
	sfg::Desktop desktop;
	//sfg::ComboBox::Ptr DrpDown;
	//sfg::Box::Ptr box;
	sf::Clock clock;

	vector<sf::String> BlockNames;

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

