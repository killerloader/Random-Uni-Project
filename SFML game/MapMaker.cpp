#include "MapMaker.h"

MapMaker::MapMaker(WrapperClass &WCR_) : WCR(WCR_)
{
	MapMakrView.setSize(sf::Vector2f(640, 480));
}

MapMaker::~MapMaker()
{

}

bool MapMaker::LoadMap(int MID)
{
	//curmapID
	stringstream MapName;
	MapName << "L:\\Map[" << MID << "].txt";
	fileManager FMuse(WCR);//Create a shortcut reference to file manager.
	if (!FMuse.openFile(true, MapName.str().c_str()))
		return false;
	WCR.MapPtr->MapWidth = FMuse.load4Bytes();
	WCR.MapPtr->MapHeight = FMuse.load4Bytes();
	WCR.PlrPtr->x = FMuse.load4Bytes() * 32;
	WCR.PlrPtr->y = FMuse.load4Bytes() * 32;
	WCR.PlrPtr->PlayerImage.setPosition(WCR.PlrPtr->x, WCR.PlrPtr->y);
	WCR.PlrPtr->xstart = WCR.PlrPtr->x;
	WCR.PlrPtr->ystart = WCR.PlrPtr->y;
	//Error here, when different map size.
	WCR.MapPtr->MapMatrix = vector<vector<int>>(WCR.MapPtr->MapWidth, vector<int>(WCR.MapPtr->MapHeight, 0));
	for (int i = 0; i < WCR.MapPtr->MapWidth; i++)
		for (int ii = 0; ii < WCR.MapPtr->MapHeight; ii++)
			WCR.MapPtr->MapMatrix[i][ii] = FMuse.loadByte();
	WCR.MapPtr->setupBorders();
	return true;
}

bool MapMaker::SaveMap(int MID)
{
	stringstream MapName;
	MapName << "L:\\Map[" << MID << "].txt";
	fileManager FMuse(WCR);//Create a shortcut reference to file manager.
	if (!FMuse.openFile(false, MapName.str().c_str()))
		return false;
	FMuse.save4Bytes(WCR.MapPtr->MapWidth);
	FMuse.save4Bytes(WCR.MapPtr->MapHeight);
	FMuse.save4Bytes(round(WCR.PlrPtr->x / 32));
	FMuse.save4Bytes(round(WCR.PlrPtr->y / 32));
	for (int i = 0; i < WCR.MapPtr->MapWidth; i++)
		for (int ii = 0; ii < WCR.MapPtr->MapHeight; ii++)
			FMuse.saveByte(WCR.MapPtr->MapMatrix[i][ii]);
}

void MapMaker::Draw()
{
	int PlaceX = floor((sf::Mouse::getPosition(WCR.RenderRef).x + MapMakrView.getCenter().x - MapMakrView.getSize().x / 2) / 32);
	int PlaceY = floor((sf::Mouse::getPosition(WCR.RenderRef).y + MapMakrView.getCenter().y - MapMakrView.getSize().y / 2) / 32);
	WCR.LimitVariable(0, WCR.MapPtr->MapWidth - 1, PlaceX);
	WCR.LimitVariable(0, WCR.MapPtr->MapWidth - 1, PlaceY);
	sf::RectangleShape PlaceRect;
	PlaceRect.setFillColor(sf::Color::Transparent);
	PlaceRect.setOutlineColor(sf::Color::Red);
	PlaceRect.setOutlineThickness(1);
	PlaceRect.setSize(sf::Vector2f(32, 32));
	PlaceRect.setPosition(PlaceX*32, PlaceY*32);
	WCR.RenderRef.draw(PlaceRect);
}

void MapMaker::Step()
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		int PlaceX = floor((sf::Mouse::getPosition(WCR.RenderRef).x + MapMakrView.getCenter().x - MapMakrView.getSize().x / 2) / 32);
		int PlaceY = floor((sf::Mouse::getPosition(WCR.RenderRef).y + MapMakrView.getCenter().y - MapMakrView.getSize().y / 2) / 32);
		WCR.MapPtr->SetObject(PlaceX, PlaceY, CurBlock);
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
	{
		int PlaceX = floor((sf::Mouse::getPosition(WCR.RenderRef).x + MapMakrView.getCenter().x - MapMakrView.getSize().x / 2) / 32);
		int PlaceY = floor((sf::Mouse::getPosition(WCR.RenderRef).y + MapMakrView.getCenter().y - MapMakrView.getSize().y / 2) / 32);
		WCR.MapPtr->SetObject(PlaceX, PlaceY, 0);
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		int Newx = floor((sf::Mouse::getPosition(WCR.RenderRef).x + MapMakrView.getCenter().x - MapMakrView.getSize().x / 2) / 32) * 32;
		int Newy = floor((sf::Mouse::getPosition(WCR.RenderRef).y + MapMakrView.getCenter().y - MapMakrView.getSize().y / 2) / 32) * 32;
		WCR.LimitVariable(0, WCR.MapPtr->MapWidth*32 , Newx);
		WCR.LimitVariable(0, WCR.MapPtr->MapWidth*32 , Newy);
		WCR.PlrPtr->x = Newx;
		WCR.PlrPtr->y = Newy;
		WCR.PlrPtr->PlayerImage.setPosition(WCR.PlrPtr->x, WCR.PlrPtr->y);
	}
	int ViewMoveSPD = 5;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		MapMakrView.move(0, -ViewMoveSPD);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		MapMakrView.move(-ViewMoveSPD, 0);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		MapMakrView.move(ViewMoveSPD, 0);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		MapMakrView.move(0, ViewMoveSPD);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		char Command[11];
		cout << "Please enter a command: " << endl;
		cin >> Command;
		if (strcmp(Command, "save") == 0)
		{
			cout << "Enter a map ID: " << endl;
			int Input;
			cin >> Input;
			SaveMap(Input);
		}
		if (strcmp(Command, "load") == 0)
		{
			cout << "Enter a map ID: " << endl;
			int Input;
			cin >> Input;
			LoadMap(Input);
		}
		if (strcmp(Command, "setblock") == 0)
		{
			int BlockID = 1;
			cin >> BlockID;
			setBlock(BlockID);
		}
		if (strcmp(Command, "clear") == 0)
		{
			cout << "Enter a new map size, width: " << endl;
			cin >> WCR.MapPtr->MapWidth;
			cout << "Height: " << endl;
			cin >> WCR.MapPtr->MapHeight;
			WCR.MapPtr->MapMatrix = vector<vector<int>>(WCR.MapPtr->MapWidth, vector<int>(WCR.MapPtr->MapHeight, 0));
			WCR.MapPtr->setupBorders();
		}
	}
}

void MapMaker::setBlock(int BLK)
{
	//0 = nothing, 2 = last object type
	if (BLK >= 1 && BLK <= 4)
		CurBlock = BLK;
	else
		cout << "This object ID does not exist!" << endl;
}

int MapMaker::getBlock()
{
	return CurBlock;
}