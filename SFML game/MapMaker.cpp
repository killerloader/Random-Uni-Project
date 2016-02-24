#include "MapMaker.h"

MapMaker::MapMaker(WrapperClass &WCR_) : WCR(WCR_)
{
	MapMakrView.setSize(sf::Vector2f(1280, 960));
	windowSFG = sfg::Window::Create();
	windowSFG->SetPosition(sf::Vector2f(WCR.MapPtr->ViewWidth - 200, WCR.MapPtr->ViewHeight - 960));
	windowSFG->SetTitle("Object Selector");
	windowSFG->SetRequisition(sf::Vector2f(200, 200));

	lvlIdMinus = sfg::Button::Create("-");
	lvlIdPlus = sfg::Button::Create("+");

	lvlIdMinus->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::buttonPressChangeMapID, this, 0));
	lvlIdPlus->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::buttonPressChangeMapID, this, 1));

	MapIDEntry = sfg::Entry::Create();
	//MapIDEntry->SetRequisition(sf::Vector2f(64, 0));
	MapIDLabel = sfg::Label::Create();
	MapIDLabel->SetText("Map ID: ");
	MapIDEntry->SetMaximumLength(2);//Only allow 2 digit number max.

	windowSFG->GetSignal(sfg::Button::OnMouseEnter).Connect(bind(&MapMaker::mouseEnterWindow, this, 1));
	windowSFG->GetSignal(sfg::Button::OnMouseLeave).Connect(bind(&MapMaker::mouseLeaveWindow, this, 1));

	//setup block names
	BlockNames.push_back("NULL");
	BlockNames.push_back("Solid");
	BlockNames.push_back("Bouncy Block");
	BlockNames.push_back("Lava");
	BlockNames.push_back("Finish block");

	BlockNames.push_back("Save Map");
	BlockNames.push_back("Load Map");
	BlockNames.push_back("Clear Map");

	box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
	MapBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 5.0f);
	for (int i = 0; i < 7; i++)
	{
		blockButtons[i] = sfg::Button::Create(BlockNames[i+1]);
		blockButtons[i]->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::ButtonPress, this, i+1));
		//box->SetPosition(sf::Vector2f(0.f, 20.f * i));

		box->Pack(blockButtons[i], false);
	}
	
	box->Pack(MapIDLabel, false);
	MapBox->Pack(lvlIdMinus, false);
	MapBox->Pack(MapIDEntry, true,true);
	MapBox->Pack(lvlIdPlus, false);
	box->Pack(MapBox, false);
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
}

MapMaker::~MapMaker()
{

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

void MapMaker::mouseEnterWindow(int)
{
	MouseOverWindow++;
}

void MapMaker::mouseLeaveWindow(int)
{
	MouseOverWindow--;
}

void MapMaker::ButtonPress(int test)
{
	//button1->SetLabel("Tagggggggg");
	string Astr = BlockNames[test].toAnsiString();
	
	switch (test)
	{
	case 0:
		break;
	case 1:setBlock(1);
		break;
	case 2:setBlock(2);
		break;
	case 3:setBlock(3);
		break;
	case 4:setBlock(4);
		break;
	case 5:
	{
		unsigned int buf_number(0);
		std::stringstream sstr(std::string(MapIDEntry->GetText()));
		sstr >> buf_number;
		SaveMap(buf_number);
		break;
	}
	case 6:
	{
		unsigned int buf_number(0);
		std::stringstream sstr(std::string(MapIDEntry->GetText()));
		sstr >> buf_number;
		LoadMap(buf_number);
		break;
	}
	case 7:ClearMap();
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
	WCR.MapPtr->MapMatrix = vector<vector<int>>(WCR.MapPtr->MapWidth, vector<int>(WCR.MapPtr->MapHeight, 0));
	for (int i = 0; i < WCR.MapPtr->MapWidth; i++)
		for (int ii = 0; ii < WCR.MapPtr->MapHeight; ii++)
			WCR.MapPtr->MapMatrix[i][ii] = FMuse.loadByte();
	WCR.MapPtr->setupBorders();
	cout << "Loaded MAP ID: " << MID << endl;
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
			FMuse.saveByte(WCR.MapPtr->MapMatrix[i][ii]);
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
	PlaceRect.setPosition(PlaceX*32, PlaceY*32);
	WCR.RenderRef.draw(PlaceRect);
	sfgui.Display(WCR.RenderRef);
}


void MapMaker::Step()
{
	if (MouseOverWindow == 0 && !pressedOffScreen)//Or just check if false?
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
	desktop.Update(clock.restart().asSeconds());
}

void MapMaker::ClearMap()
{
	//cout << "Enter a new map size, width: " << endl;
	//cin >> WCR.MapPtr->MapWidth;
	//cout << "Height: " << endl;
	//cin >> WCR.MapPtr->MapHeight;
	WCR.MapPtr->MapMatrix = vector<vector<int>>(WCR.MapPtr->MapWidth, vector<int>(WCR.MapPtr->MapHeight, 0));
	WCR.MapPtr->setupBorders();
}

void MapMaker::setBlock(int BLK)
{
	cout << "Setting block to: " << BLK << endl;
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