#include "MapMaker.h"



MapMaker::MapMaker(WrapperClass &WCR_) : WCR(WCR_)
{

	WindowIsHovered[0] = false;
	WindowIsHovered[1] = false;
	MapMakrView.setSize(sf::Vector2f(640, 480));
	windowSFG = sfg::Window::Create();
	windowSFG->SetPosition(sf::Vector2f(WCR.MapPtr->ViewWidth - 200, WCR.MapPtr->ViewHeight - 400));
	windowSFG->SetTitle("Main Menu");
	windowSFG->SetRequisition(sf::Vector2f(200, 200));

	MapIDEntry = sfg::Entry::Create();
	//MapIDEntry->SetRequisition(sf::Vector2f(64, 0));
	MapIDLabel = sfg::Label::Create();
	MapIDLabel->SetText("Map ID: ");
	MapIDEntry->SetMaximumLength(2);//Only allow 2 digit number max.

	windowSFG->GetSignal(sfg::Button::OnMouseEnter).Connect(bind(&MapMaker::mouseEnterWindow, this, 0));
	windowSFG->GetSignal(sfg::Button::OnMouseLeave).Connect(bind(&MapMaker::mouseLeaveWindow, this, 0));

	//setup block names
	BlockNames.push_back("asdasd");//FIrst object is ID 0 which is reserved for empty space.
	BlockNames.push_back("Solid");
	BlockNames.push_back("Bouncy Block");
	BlockNames.push_back("Lava");
	BlockNames.push_back("Finish block");
	BlockNames.push_back("Select Tile");
	BlockNames.push_back("Save Map");
	BlockNames.push_back("Load Map");
	BlockNames.push_back("Clear Map");

	

	/*
			BUG might be due to storing them in pointers, although its strange that it always breaks on the same part.
	*/

	box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
	MapBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 5.0f);
	for (int i = 0; i < 9; i++)
	{
		blockButtons[i] = sfg::Button::Create(BlockNames[i]);
		blockButtons[i]->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::ButtonPress, this, i));
		//box->SetPosition(sf::Vector2f(0.f, 20.f * i));
		box->Pack(blockButtons[i], false);
	}
	TilesetList = sfg::ComboBox::Create();

	loadTiles();
	updateTiles();

	lvlIdMinus = sfg::Button::Create("-");
	lvlIdPlus = sfg::Button::Create("+");

	lvlIdMinus->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::buttonPressChangeMapID, this, 0));
	lvlIdPlus->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::buttonPressChangeMapID, this, 1));

	box->Pack(MapIDLabel, false);
	MapBox->Pack(lvlIdMinus, false);
	MapIDEntry->SetText("0");
	MapBox->Pack(MapIDEntry, true, true);
	MapBox->Pack(lvlIdPlus, false);
	box->Pack(MapBox, false);
	
	windowTiles = sfg::Window::Create();
	windowTiles->SetPosition(sf::Vector2f(WCR.MapPtr->ViewWidth - 200, WCR.MapPtr->ViewHeight - 200));
	windowTiles->SetTitle("Tile Selector");
	windowTiles->SetRequisition(sf::Vector2f(200, 200));

	TilesetList->GetSignal(sfg::ComboBox::OnSelect).Connect(std::bind(&MapMaker::changeTileSet, this));
	TileCanvas = sfg::Canvas::Create();
	TileCanvas->SetRequisition(sf::Vector2f(200.0f, 200.0f));

	//TileBoxVert

	windowTiles->GetSignal(sfg::Button::OnMouseEnter).Connect(bind(&MapMaker::mouseEnterWindow, this, 1));
	windowTiles->GetSignal(sfg::Button::OnMouseLeave).Connect(bind(&MapMaker::mouseLeaveWindow, this, 1));

	TileBoxVert = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
	TileBoxVert->Pack(TilesetList, false, false);
	TileBoxVert->Pack(TileCanvas);
	//int upto_ = 0;
	//int HorAmt_ = 5;

	//TileBoxesHor.emplace_back();
	//TileBoxesHor[upto_] = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 5.0f);
	/*TileBoxVert->Pack(TileBoxesHor[upto_], false);
	upto_++;
	
	for (int i = 0; i < TileImages.size(); i++)
	{
		if (i - upto_*HorAmt_ >= 0)
		{
			TileBoxesHor.emplace_back();
			TileBoxesHor[upto_] = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 5.0f);
			TileBoxVert->Pack(TileBoxesHor[upto_], false);
			upto_++;
		}
		TileBoxesHor[upto_-1]->Pack(TileImages[i], false);
		TileImages[i]->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::pressTiles, this, i));
	}*/

	//DrpDown = sfg::ComboBox::Create();
	//DrpDown->SetRequisition(sf::Vector2f(100, 100));
	//windowSFG->Add(DrpDown);
	//DrpDown->Add(button1);
	//DrpDown->Add(button2);
	//box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
	//box->Pack(DrpDown, false);
	//windowSFG->Add(box);
	
	windowSFG->Add(box);
	desktop.Add(windowSFG);
	//windowTiles->Add(TileBoxVert);
	windowTiles->Add(TileBoxVert);
	desktop.Add(windowTiles);
}

MapMaker::~MapMaker()
{

}

void MapMaker::pressTiles(int v)
{
	curTile = v+1;
	cout << "Set tile to " << v+1 << endl;
}

void MapMaker::loadTile(const char* chr)
{
	//GameImages.emplace_back();
	//GameImages[GameImages.size()-1].loadFromFile(chr);
	//GameTextures.emplace_back();
	//GameTextures[GameTextures.size() - 1].loadFromFile(chr);
}

void MapMaker::changeTileSet()
{
	//cout << "Item " << TilesetList->GetSelectedItem() << " selected with text \"" << static_cast<std::string>(TilesetList->GetSelectedText()) << "\"" << endl;
	curTileSet = TilesetList->GetSelectedItem();
	//cout << curTileSet << endl;
	windowTiles->SetRequisition(sf::Vector2f(TileSets[curTileSet].TileSheetSprite.getLocalBounds().width, TileSets[curTileSet].TileSheetSprite.getLocalBounds().height));
}

void MapMaker::updateTiles()
{
	for (int i = 0; i < TileSets.size(); i++)
	{
		TileSets[i].UpdateSprites();
	}		
}

void MapMaker::loadTiles()
{
	TileSets.emplace_back("Data/Sprites/TileSheet1.png", 0, 0, 1, 1, 3, 3, TilesetList);
	TileSets.emplace_back("Data/Sprites/TileSheet3.png", 2, 1, 2, 2, 18, 11, TilesetList);
	TileSets.emplace_back("Data/Sprites/TileSheet4.png", 20, 10, 2, 2, 30, 16, TilesetList);
		//const char* FileName, int xOff, int yOff, int xGap, int yGap, int xCells, int yCells
	//loadTile("Data/Sprites/TileSheet3.png");//Load first tilesheet.
}

void MapMaker::buttonPressChangeMapID(int test)
{
	switch (test)
	{
		case 0://minus
		{
			int buf_number(0);
			std::stringstream sstr(std::string(MapIDEntry->GetText()));
			sstr >> buf_number;
			buf_number--;
			if (buf_number < 0)
				buf_number = 0;
			sstr.clear();
			sstr << buf_number;
			MapIDEntry->SetText(sstr.str());
			break;
		}
		case 1://plus
		{
			int buf_number(0);
			std::stringstream sstr(std::string(MapIDEntry->GetText()));
			sstr >> buf_number;
			buf_number++;
			if (buf_number > 99)
				buf_number = 99;
			sstr.clear();
			sstr << buf_number;
			MapIDEntry->SetText(sstr.str());
			break;
		}
	}
}

void MapMaker::PollGUIEvents()
{
	desktop.HandleEvent(WCR.event);
}

void MapMaker::mouseEnterWindow(int WINID)
{
	WindowIsHovered[WINID] = true;
	MouseOverWindow++;
}

void MapMaker::mouseLeaveWindow(int WINID)
{
	WindowIsHovered[WINID] = false;
	MouseOverWindow--;
}

void MapMaker::ButtonPress(int test)
{
	//button1->SetLabel("Tagggggggg");
	string Astr = BlockNames[test].toAnsiString();
	
	switch (test)
	{
	case 0:setBlock(0);
		break;
	case 1:setBlock(1);
		break;
	case 2:setBlock(2);
		break;
	case 3:setBlock(3);
		break;
	case 4:setBlock(4);
		break;
	case 5://set tile
	{
		break;
	}
	case 6:
	{//add scope for buffer
		unsigned int buf_number(0);
		std::stringstream sstr(std::string(MapIDEntry->GetText()));
		sstr >> buf_number;
		SaveMap(buf_number);
		break;
	}
	case 7:
	{
		unsigned int buf_number(0);
		std::stringstream sstr(std::string(MapIDEntry->GetText()));
		sstr >> buf_number;
		LoadMap(buf_number);
		break;
	}
	case 8:ClearMap();
		break;
	}
}

bool MapMaker::LoadMap(int MID)
{
	if (MID < 0 || MID>99)
		return false;

	//curmapID
	stringstream MapName;
	MapName << "Maps/Map[" << MID << "].txt";
	fileManager FMuse(WCR);//Create a shortcut reference to file manager.
	if (!FMuse.openFile(true, MapName.str().c_str()))
	{
		cout << "Could not load map!" << endl;
		return false;
	}

	WCR.MapPtr->MapWidth = FMuse.load4Bytes();
	WCR.MapPtr->MapHeight = FMuse.load4Bytes();
	WCR.PlrPtr->x = FMuse.load4Bytes() * 32;
	WCR.PlrPtr->y = FMuse.load4Bytes() * 32;
	WCR.PlrPtr->PlayerImage.setPosition(WCR.PlrPtr->x, WCR.PlrPtr->y);
	WCR.PlrPtr->xstart = WCR.PlrPtr->x;
	WCR.PlrPtr->ystart = WCR.PlrPtr->y;
	//Error here, when different map size.
	WCR.MapPtr->MapMatrix = vector<vector<mapObject>>(WCR.MapPtr->MapWidth, vector<mapObject>(WCR.MapPtr->MapHeight));

	for (int i = 0; i < WCR.MapPtr->MapWidth; i++)
		for (int ii = 0; ii < WCR.MapPtr->MapHeight; ii++)
		{
			WCR.MapPtr->MapMatrix[i][ii].objectType = FMuse.loadByte();
			WCR.MapPtr->MapMatrix[i][ii].tileID = FMuse.loadByte() - 1;
			WCR.MapPtr->MapMatrix[i][ii].tileSetID = FMuse.loadByte();
		}

	WCR.MapPtr->setupBorders();
	cout << "Loaded MAP ID: " << MID << endl;

#ifdef MapMakerMode//start server.
	if (WCR.connected)
	{
		sf::Packet mapData;
		mapData << (sf::Int32)0;
		mapData << (sf::Int32)WCR.MapPtr->MapWidth << (sf::Int32)WCR.MapPtr->MapHeight << (sf::Int32)WCR.PlrPtr->x << (sf::Int32)WCR.PlrPtr->y;

		for (int i = 0; i < WCR.MapPtr->MapWidth; i++)
			for (int ii = 0; ii < WCR.MapPtr->MapHeight; ii++)
				mapData << (sf::Int32)WCR.MapPtr->MapMatrix[i][ii].objectType << (sf::Int32)WCR.MapPtr->MapMatrix[i][ii].tileID << (sf::Int32)WCR.MapPtr->MapMatrix[i][ii].tileSetID;
		for (int i = 0; i < WCR.clients.size(); i++)
			if (WCR.clients[i] != nullptr)
				WCR.clients[i]->send(mapData);
		
		cout << "Sent map data!" << endl;
	}
#endif

	return true;
}

bool MapMaker::SaveMap(int MID)
{
	if (MID < 0 || MID>99)
		return false;
	stringstream MapName;
	MapName << "Maps/Map[" << MID << "].txt";
	fileManager FMuse(WCR);//Create a shortcut reference to file manager.
	if (!FMuse.openFile(false, MapName.str().c_str()))
		return false;
	FMuse.save4Bytes(WCR.MapPtr->MapWidth);
	FMuse.save4Bytes(WCR.MapPtr->MapHeight);
	FMuse.save4Bytes(round(WCR.PlrPtr->x / 32));
	FMuse.save4Bytes(round(WCR.PlrPtr->y / 32));
	for (int i = 0; i < WCR.MapPtr->MapWidth; i++)
		for (int ii = 0; ii < WCR.MapPtr->MapHeight; ii++)
		{
			FMuse.saveByte(WCR.MapPtr->MapMatrix[i][ii].objectType);
			FMuse.saveByte(WCR.MapPtr->MapMatrix[i][ii].tileID+1);//+1 because -1 is reserved for nothing.
			FMuse.saveByte(WCR.MapPtr->MapMatrix[i][ii].tileSetID);
		}
	cout << "Saved MAP ID: " << MID << endl;
	return true;
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
	PlaceRect.setPosition(PlaceX * 32, PlaceY * 32);

	WCR.RenderRef.draw(PlaceRect);

	TileCanvas->Bind();
	TileCanvas->Clear(sf::Color(50, 50, 50));
	//PlaceRect.setPosition(0, 0);
	TileSets[curTileSet].TileSheetSprite.setPosition(0, 0);
	TileCanvas->Draw(TileSets[curTileSet].TileSheetSprite);
	sf::Vector2f absolutePosition = TileCanvas->GetAbsolutePosition();
	sf::Vector2i mousePos = sf::Mouse::getPosition(WCR.RenderRef) - sf::Vector2i(absolutePosition.x, absolutePosition.y);
	//if (TileCanvas->IsActiveWidget())

	sf::RectangleShape TilesetSelectedRect;
	TilesetSelectedRect.setFillColor(sf::Color::Transparent);
	TilesetSelectedRect.setOutlineColor(sf::Color::Blue);
	TilesetSelectedRect.setOutlineThickness(1);
	TilesetSelectedRect.setSize(sf::Vector2f(32, 32));
	int TssR_x = TileSets[curTileSet].xOffset + curTile * (32 + TileSets[curTileSet].GapW) - (floor(curTile / TileSets[curTileSet].CellsX) * (32+ TileSets[curTileSet].GapW) * TileSets[curTileSet].CellsX);
	int TssR_y = TileSets[curTileSet].yOffset + floor(curTile / TileSets[curTileSet].CellsX) * (32 + TileSets[curTileSet].GapH);
	TilesetSelectedRect.setPosition(TssR_x, TssR_y);
	TileCanvas->Draw(TilesetSelectedRect);

	/*
	int curTile=0;
	int curTileSet = 0;
	*/
	if (WindowIsHovered[1])//If over tile window
		if (mousePos.x > 0 && mousePos.y > 0 && mousePos.x <= TileSets[curTileSet].TileSheetSprite.getLocalBounds().width && mousePos.y <= TileSets[curTileSet].TileSheetSprite.getLocalBounds().height)
		{
			PlaceX = floor((mousePos.x - TileSets[curTileSet].xOffset) / (32 + TileSets[curTileSet].GapW));// -TileSets[curTileSet].xOffset;
			PlaceY = floor((mousePos.y - TileSets[curTileSet].yOffset) / (32 + TileSets[curTileSet].GapH));// -TileSets[curTileSet].yOffset;
			PlaceRect.setPosition(TileSets[curTileSet].xOffset + PlaceX * (32+ TileSets[curTileSet].GapW), TileSets[curTileSet].yOffset + PlaceY * (32+ TileSets[curTileSet].GapH));
			TileCanvas->Draw(PlaceRect);
			//cout << sf::Vector2i(absolutePosition.x, absolutePosition.y).x << "_" << sf::Vector2i(absolutePosition.x, absolutePosition.y).y << endl;
		}
	
	//cout << MouseOverWindow << "_" << WindowIsHovered[0] << "_" << WindowIsHovered[1] << endl;
	TileCanvas->Display();
	TileCanvas->Unbind();

	sfgui.Display(WCR.RenderRef);
}


void MapMaker::Step()
{
	if (MouseOverWindow == 0 && !pressedOffScreen && WCR.RenderRef.hasFocus())//Or just check if false?
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			int PlaceX = floor((sf::Mouse::getPosition(WCR.RenderRef).x + MapMakrView.getCenter().x - MapMakrView.getSize().x / 2) / 32);
			int PlaceY = floor((sf::Mouse::getPosition(WCR.RenderRef).y + MapMakrView.getCenter().y - MapMakrView.getSize().y / 2) / 32);
			if(!WCR.MapPtr->isObject(PlaceX, PlaceY))//If no object already here
				WCR.MapPtr->SetObject(PlaceX, PlaceY, CurBlock, curTile, curTileSet);
			//cout << CurBlock << "_" << curTile << "_" << curTileSet << endl;
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
		{
			int PlaceX = floor((sf::Mouse::getPosition(WCR.RenderRef).x + MapMakrView.getCenter().x - MapMakrView.getSize().x / 2) / 32);
			int PlaceY = floor((sf::Mouse::getPosition(WCR.RenderRef).y + MapMakrView.getCenter().y - MapMakrView.getSize().y / 2) / 32);
			if (WCR.MapPtr->isObject(PlaceX, PlaceY))//If object is here
				WCR.MapPtr->SetObject(PlaceX, PlaceY, 0, -1, 0);
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
		{
			int Newx = floor((sf::Mouse::getPosition(WCR.RenderRef).x + MapMakrView.getCenter().x - MapMakrView.getSize().x / 2) / 32) * 32;
			int Newy = floor((sf::Mouse::getPosition(WCR.RenderRef).y + MapMakrView.getCenter().y - MapMakrView.getSize().y / 2) / 32) * 32;
			WCR.LimitVariable(0, WCR.MapPtr->MapWidth * 32, Newx);
			WCR.LimitVariable(0, WCR.MapPtr->MapWidth * 32, Newy);
			WCR.PlrPtr->x = Newx;
			WCR.PlrPtr->y = Newy;
			WCR.PlrPtr->PlayerImage.setPosition(WCR.PlrPtr->x, WCR.PlrPtr->y);
		}
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		pressedOffScreen = true;
	else
		pressedOffScreen = false;
	
	if (WindowIsHovered[1] && WCR.RenderRef.hasFocus())//If over tile window
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			/*
			sf::Vector2f absolutePosition = TileCanvas->GetAbsolutePosition();
			sf::Vector2i mousePos = sf::Mouse::getPosition(WCR.RenderRef) - sf::Vector2i(absolutePosition.x, absolutePosition.y);
			PlaceX = floor(mousePos.x / 33);
			PlaceY = floor(mousePos.y / 33);
			//cout << sf::Vector2i(absolutePosition.x, absolutePosition.y).x << "_" << sf::Vector2i(absolutePosition.x, absolutePosition.y).y << endl;
			PlaceRect.setPosition(PlaceX * 33, PlaceY * 33);
			*/
			sf::Vector2f absolutePosition = TileCanvas->GetAbsolutePosition();
			sf::Vector2i mousePos = sf::Mouse::getPosition(WCR.RenderRef) - sf::Vector2i(absolutePosition.x, absolutePosition.y);
			//if(TileCanvas->IsActiveWidget())
			if (WindowIsHovered[1])//If over tile window
				if (mousePos.x > 0 && mousePos.y > 0 && mousePos.x <= TileSets[curTileSet].TileSheetSprite.getLocalBounds().width && mousePos.y <= TileSets[curTileSet].TileSheetSprite.getLocalBounds().height)
				{
					curTile = floor((mousePos.x -TileSets[curTileSet].xOffset)/(32+TileSets[curTileSet].GapW))+ floor((mousePos.y - TileSets[curTileSet].yOffset) /(32+TileSets[curTileSet].GapH))*TileSets[curTileSet].CellsX;
					//curTileSet = 0;
				}
			//WCR.MapPtr->SetObject(PlaceX, PlaceY, CurBlock, curTile);
		}
	}

	if (WCR.RenderRef.hasFocus())
	{
		int ViewMoveSPD = 5;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			MapMakrView.move(0, -ViewMoveSPD);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			MapMakrView.move(-ViewMoveSPD, 0);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			MapMakrView.move(ViewMoveSPD, 0);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			MapMakrView.move(0, ViewMoveSPD);
	}


	/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
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
			WCR.MapPtr->MapMatrix = vector<vector<mapObject>>(WCR.MapPtr->MapWidth, vector<mapObject>(WCR.MapPtr->MapHeight));
			WCR.MapPtr->setupBorders();
		}
	}*/
	desktop.Update(clock.restart().asSeconds());
}

void MapMaker::ClearMap()
{
	//cout << "Enter a new map size, width: " << endl;
	//cin >> WCR.MapPtr->MapWidth;
	//cout << "Height: " << endl;
	//cin >> WCR.MapPtr->MapHeight;
	WCR.MapPtr->MapMatrix = vector<vector<mapObject>>(WCR.MapPtr->MapWidth, vector<mapObject>(WCR.MapPtr->MapHeight));
	WCR.MapPtr->setupBorders();
}

void MapMaker::setBlock(int BLK)
{
	cout << "Setting block to: " << BLK << endl;
	//0 = nothing, 2 = last object type
	if (BLK >= 0 && BLK <= 4)
		CurBlock = BLK;
	else
		cout << "This object ID does not exist!" << endl;
}

int MapMaker::getBlock()
{
	return CurBlock;
}