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
	WrapperClass &WCR;
	void Step();
	void Draw();
	bool LoadMap(int);
	bool SaveMap(int);
	int CurBlock = 1;
	sf::View MapMakrView;
	MapMaker(WrapperClass &WCR_);
	~MapMaker();
};

