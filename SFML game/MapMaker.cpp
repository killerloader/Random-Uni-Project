#include "MapMaker.h"



MapMaker::MapMaker(WrapperClass &WCR_) : WCR(WCR_)
{
#ifdef MapMakerMode//Start window straight away for server.
	mmWindow.create(sf::VideoMode(640, 480), "Map Editor");
	mmWindow.setFramerateLimit(60);
	mmWindow.setVerticalSyncEnabled(true);
	mmView.setSize(sf::Vector2f(640, 480));
	mmWindow.setView(mmView);
#endif
	/*
	sf::Font my_font;
	my_font.loadFromFile("Data/Fonts/OptimusPrinceps.ttf");
	desktop.GetEngine().GetResourceManager().AddFont("custom_font", my_font);

	desktop.SetProperty<("", "Font", "OptimusPrinceps.ttf");*/

	WindowIsHovered[0] = false;
	WindowIsHovered[1] = false;
	MapMakrView.setSize(sf::Vector2f(640, 480));
	windowSFG = sfg::Window::Create(sfg::Window::Style::TITLEBAR | sfg::Window::Style::BACKGROUND);
	windowSFG->SetPosition(sf::Vector2f(0, 0));
	windowSFG->SetTitle("Main Menu");
	windowSFG->SetRequisition(sf::Vector2f(200, 200));

	windowSFG->GetSignal(sfg::Button::OnMouseEnter).Connect(bind(&MapMaker::mouseEnterWindow, this, 0));
	windowSFG->GetSignal(sfg::Button::OnMouseLeave).Connect(bind(&MapMaker::mouseLeaveWindow, this, 0));

	//setup block names
	/*BlockNames.push_back("asdasd");//FIrst object is ID 0 which is reserved for empty space.
	BlockNames.push_back("Solid");
	BlockNames.push_back("Bouncy Block");
	BlockNames.push_back("Lava");
	BlockNames.push_back("Finish block");
	BlockNames.push_back("Select Tile");*/
	BlockNames.push_back("Save Map");
	BlockNames.push_back("Load Map");
	BlockNames.push_back("Clear Map");
	BlockNames.push_back("Tile Selector");

	box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);

	ObjectTypeList = sfg::ComboBox::Create();

	ObjectTypeList->AppendItem("Invisible");
	ObjectTypeList->AppendItem("Solid");
	ObjectTypeList->AppendItem("Bouncy");
	ObjectTypeList->AppendItem("Death");
	ObjectTypeList->AppendItem("Next Level");
	ObjectTypeList->AppendItem("Slippery");
	ObjectTypeList->SelectItem(CurBlock);

	ObjectTypeList->GetSignal(sfg::ComboBox::OnSelect).Connect(std::bind(&MapMaker::changeObjectType, this));
	
	/*
			BUG might be due to storing them in pointers, although its strange that it always breaks on the same part.
	*/

	TilesetList = sfg::ComboBox::Create();

	loadTiles();
	updateTiles();


	lvlIdMinus = sfg::Button::Create("-");
	lvlIdPlus = sfg::Button::Create("+");

	lvlIdMinus->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::buttonPressChangeMapID, this, 0));
	lvlIdPlus->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::buttonPressChangeMapID, this, 1));

	MapIDEntry = sfg::Entry::Create();
	//MapIDEntry->SetRequisition(sf::Vector2f(64, 0));
	MapIDEntry->SetMaximumLength(2);//Only allow 2 digit number max.
	MapBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 5.0f);

	


	//First three are save/load/clear

#ifdef MapMakerMode//No console on client map editor
	box->Pack(sfg::Label::Create("--Map Management--"), false);
	box->Pack(sfg::Separator::Create(), false);
	MapBox->Pack(lvlIdMinus, false);
	MapIDEntry->SetText("0");
	MapBox->Pack(MapIDEntry, true, true);
	MapBox->Pack(lvlIdPlus, false);

	box->Pack(sfg::Label::Create("Map ID"), false);
	box->Pack(MapBox, false);
	for (int i = 0; i < 3; i++)
	{
		blockButtons[i] = sfg::Button::Create(BlockNames[i]);
		blockButtons[i]->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::ButtonPress, this, i));
		//box->SetPosition(sf::Vector2f(0.f, 20.f * i));
		box->Pack(blockButtons[i], false);
	}

	box->Pack(sfg::Separator::Create(), false);
#endif
	box->Pack(sfg::Label::Create("--Map Editing--"), false);
	box->Pack(sfg::Separator::Create(), false);
	box->Pack(sfg::Label::Create("Object Type"), false);
	box->Pack(ObjectTypeList, false);

	//Map editor things
	for (int i = 3; i < BlockNames.size(); i++)
	{
		blockButtons[i] = sfg::Button::Create(BlockNames[i]);
		blockButtons[i]->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::ButtonPress, this, i));
		//box->SetPosition(sf::Vector2f(0.f, 20.f * i));
		box->Pack(blockButtons[i], false);
	}

	PixelPerfectToggle = sfg::CheckButton::Create("Precise Collision");
	PixelPerfectToggle->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::tickButtonPress, this, 1));

	PlacingObjects = sfg::CheckButton::Create("Place Objects");
	PlacingObjects->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::tickButtonPress, this, 2));
	PlacingObjects->SetActive(true);
	PlacingTiles = sfg::CheckButton::Create("Place Tiles");
	PlacingTiles->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::tickButtonPress, this, 3));
	LayersButton = sfg::Button::Create("Layer Settings");
	LayersButton->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::ButtonPress, this, 101));
	box->Pack(LayersButton, false);

	box->Pack(PixelPerfectToggle, false);
	box->Pack(PlacingObjects, false);
	box->Pack(PlacingTiles, false);

	LayerWindow = sfg::Window::Create(sfg::Window::Style::CLOSE | sfg::Window::Style::TOPLEVEL);
	LayerWindow->SetPosition(sf::Vector2f(200, 0));
	LayerWindow->SetTitle("Layer Settings");
	LayerWindow->SetRequisition(sf::Vector2f(200, 200));
	LayerWindow->Show(false);

	LWTileBoxVert = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
	NewLayerEntry = sfg::Entry::Create();
	
	AddLayerButton = sfg::Button::Create("Add Layer");
	AddLayerButton->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::ButtonPress, this, 102));

	ClearLayerButton = sfg::Button::Create("Clear Layer");
	ClearLayerButton->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::ButtonPress, this, 104));

	LayerList = sfg::ComboBox::Create();
	LayerList->AppendItem("1");

	LWTileBoxVert->Pack(LayerList, true, false);
	LWTileBoxVert->Pack(NewLayerEntry, true, false);
	LWTileBoxVert->Pack(AddLayerButton, true, false);
	LWTileBoxVert->Pack(ClearLayerButton, true, false);
	LayerWindow->Add(LWTileBoxVert);

	LayerList->GetSignal(sfg::ComboBox::OnSelect).Connect(std::bind(&MapMaker::ButtonPress, this, 103));

	//Editor Settings
	ShowObjectTypes = sfg::CheckButton::Create("View Object Types");
	ShowObjectTypes->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::tickButtonPress, this, 0));
	box->Pack(sfg::Separator::Create(), false);
	box->Pack(sfg::Label::Create("--Settings--"), false);
	box->Pack(sfg::Separator::Create(), false);
	box->Pack(ShowObjectTypes, false);
#ifdef MapMakerMode//No console on client map editor
	GiveAllPermissions = sfg::CheckButton::Create("Give All Edit Permission");
	box->Pack(GiveAllPermissions, false);
	ConsoleButton = sfg::Button::Create("Console");
	ConsoleButton->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::ButtonPress, this, 100));
	box->Pack(ConsoleButton, false);
#else//Only allow clients to exit and enter map maker mode.
	box->Pack(sfg::Separator::Create(), false);
	ExitMapMakerButton = sfg::Button::Create("Exit map maker");
	ExitMapMakerButton->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::exitMapMaker, this));
	box->Pack(ExitMapMakerButton, false);
#endif
	//end
	//Tile window
	windowTiles = sfg::Window::Create(sfg::Window::Style::CLOSE | sfg::Window::Style::TOPLEVEL);
	windowTiles->SetPosition(sf::Vector2f(200,0));
	windowTiles->SetTitle("Tile Selector");
	windowTiles->SetRequisition(sf::Vector2f(200, 200));
	windowTiles->Show(false);

	TilesetList->GetSignal(sfg::ComboBox::OnSelect).Connect(std::bind(&MapMaker::changeTileSet, this));
	TileCanvas = sfg::Canvas::Create();
	TileCanvas->SetRequisition(sf::Vector2f(200.0f, 200.0f));
	TilesetList->SelectItem(curTileSet);
	windowTiles->GetSignal(sfg::Button::OnMouseEnter).Connect(bind(&MapMaker::mouseEnterWindow, this, 1));
	windowTiles->GetSignal(sfg::Button::OnMouseLeave).Connect(bind(&MapMaker::mouseLeaveWindow, this, 1));

	TileBoxVert = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
	TileBoxVert->Pack(TilesetList, false, false);
	TileBoxVert->Pack(TileCanvas);
	//end
#ifdef MapMakerMode//No console on client map editor
	//Command window
	CommandWindow = sfg::Window::Create(sfg::Window::Style::CLOSE | sfg::Window::Style::TOPLEVEL);
	CommandWindow->SetPosition(sf::Vector2f(200, 0));
	CommandWindow->SetTitle("Command Console");
	CommandWindow->SetRequisition(sf::Vector2f(200, 200));
	CommandWindow->Show(false);
	CWTileBoxVert = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	CLogBoxVert = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);;
	SendCommand = sfg::Button::Create("Send");
	//CommandLog = sfg::ScrolledWindow::Create(); 
	CommandEntry = sfg::Entry::Create();
	SendCommand->GetSignal(sfg::Button::OnLeftClick).Connect(bind(&MapMaker::sendCommand, this));
	CWTileBoxVert->Pack(CommandEntry);
	CWTileBoxVert->Pack(SendCommand);

	CommandWindow->Add(CWTileBoxVert);
#endif
	/*
	sfg::Window::Ptr CommandWindow;//Window to select drawn tile.
	sfg::Box::Ptr CWTileBoxVert;
	sfg::Entry::Ptr CommandEntry;
	sfg::Button::Ptr SendCommand;
	sfg::Notebook CommandLog;
	*/
	//end


	windowSFG->Add(box);
	desktop.Add(windowSFG);
	windowTiles->Add(TileBoxVert);
	desktop.Add(windowTiles);
	desktop.Add(LayerWindow);
#ifdef MapMakerMode//No console on client map editor
	desktop.Add(CommandWindow);
#endif
	mmWindow.resetGLStates();
}

MapMaker::~MapMaker()
{

}

void MapMaker::exitMapMaker()
{
	WCR.inMapMaker = false;
	mmWindow.close();
}

void MapMaker::enterMapMaker()
{
	if (!WCR.mapMakerPermissions)
	{
		cout << "No permission to open map editor!" << endl;
		return;
	}
	WCR.inMapMaker = true;
	mmWindow.create(sf::VideoMode(640, 480), "Map Editor");
	mmWindow.setFramerateLimit(60);
	mmWindow.setVerticalSyncEnabled(true);
	mmView.setSize(sf::Vector2f(640, 480));
	mmWindow.setView(mmView);
	mmWindow.resetGLStates();
}

bool MapMaker::isInMapMaker()
{
	return WCR.inMapMaker;
}

void MapMaker::sendCommand()
{
	cout << "Command entered: " << CommandEntry->GetText().toAnsiString() << endl;
	stringstream Command;
	char* WholeStr = new char[CommandEntry->GetText().toAnsiString().size()+1];
	strcpy(WholeStr, CommandEntry->GetText().toAnsiString().c_str());
	for (int i = 0; i < strlen(WholeStr); i++)
	{
		if (WholeStr[i] > 65 && WholeStr[i] < 90)
			WholeStr[i] += 32;
		//-32
	}
	Command << WholeStr;
	string baseCommand;
	if (Command >> baseCommand)
	{
		//Kick
		if (baseCommand == "kick")
		{
			int PID;
			if (Command >> PID)
				if (PID >= 0 && PID < WCR.clients.size())
					if (WCR.clients[PID] != nullptr)
					{
						WCR.MHandle.deletePlayer(PID);
						cout << "Player with ID: " << PID << " has been kicked from the server!" << endl;
					}
		}
		else if (baseCommand == "setpermission")
		{
			string permType;
			if (Command >> permType)
			{
				if (permType == "edit")
				{
					int PID;
					if (Command >> PID)
						if (PID >= 0 && PID < WCR.clients.size())
							if (WCR.clients[PID] != nullptr)
							{
								int PVal;
								if (Command >> PVal)
								{
									WCR.otherPlayers[PID]->MapEditPermissions = PVal;
									sf::Packet PermSend;
									PermSend << (sf::Int32)2 << (sf::Int32)6 << (sf::Int32)0 << (sf::Int32)PVal;
									WCR.MHandle.sendData(PermSend, PID);
									cout << "Player with ID: " << PID << ", permission: [" << permType << "] is now set to: " << PVal << endl;
								}
							}
				}
			}
		}
		else if (baseCommand == "setallpermission")
		{
			string permType;
			if (Command >> permType)
			{
				if (permType == "edit")
				{
					int PVal;
					if (Command >> PVal)
					{
						sf::Packet PermSend;
						PermSend << (sf::Int32)2 << (sf::Int32)6 << (sf::Int32)0 << (sf::Int32)PVal;
						for (int i = 0; i < WCR.clients.size(); i++)
						{
							if (WCR.clients[i] == nullptr)
								continue;
							WCR.otherPlayers[i]->MapEditPermissions = PVal;
							WCR.MHandle.sendData(PermSend, i);
						}
						cout << "Player with any ID, permission: [" << permType << "] is now set to: " << PVal << endl;
					}
				}
			}
		}
		else if (baseCommand == "changemapsize")
		{
			int nsx_, nsy_;
			if (Command >> nsx_)
				if (Command >> nsy_)
				{
					WCR.MapPtr->expandMap(nsx_, nsy_);
					cout << "Changed map size to: (" << nsx_ << ", " << nsy_ << ")" << endl;
				}
		}
		//Teleport
		}
	CommandEntry->SetText("");
}

void MapMaker::tickButtonPress(int TID)
{
	switch (TID)
	{
	case 0://Show objects
		viewObjectTypes = ShowObjectTypes->IsActive();//0 if not ticked, 1 if ticked.
			//sfg::CheckButton::Widget::
		//sfg::CheckButton::L
		break;
	case 1:
		pixelPerfectBlocks = PixelPerfectToggle->IsActive();
		//cout << "Set PPB to " << pixelPerfectBlocks << endl;
		break;
	case 2://Placing objects
		PlacingTiles->SetActive(!PlacingObjects->IsActive());
		break;
	case 3://Placing tiles
		PlacingObjects->SetActive(!PlacingTiles->IsActive());
		break;
	}
}

void MapMaker::changeObjectType()
{
	setBlock(ObjectTypeList->GetSelectedItem());
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
	//cout << TilesetList->GetSelectedText().toAnsiString() << endl;
	windowTiles->SetRequisition(sf::Vector2f(TileSets[curTileSet].TileSheetSprite.getLocalBounds().width, TileSets[curTileSet].TileSheetSprite.getLocalBounds().height));
}

void MapMaker::updateTiles()
{
	for (int i = 0; i < TileSets.size(); i++)
	{
		TileSets[i].UpdateSprites();
	}
	for (int i = 0; i < TileSets.size(); i++)
	{
		TileSets[i].CreateCollisionMaps();
	}
}

void MapMaker::loadTiles()
{
	//const char* FileName, int xOff, int yOff, int xGap, int yGap, int xCells, int yCells, sfg::ComboBox::Ptr TSL, bool isMapTileSet = true, int cellSizeX = 32, int cellSizeY = 32
	TileSets.emplace_back("Data/Sprites/TileSheet1.png", 0, 0, 1, 1, 3, 3, TilesetList);
	TileSets.emplace_back("Data/Sprites/TileSheet2.png", 0, 0, 0, 0, 10, 15, TilesetList);
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
	desktop.HandleEvent(event);
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
	string Astr;
	if(test<100)
	Astr = BlockNames[test].toAnsiString();
	switch (test)
	{
	case 0:
	{//add scope for buffer
		unsigned int buf_number(0);
		std::stringstream sstr(std::string(MapIDEntry->GetText()));
		sstr >> buf_number;
		SaveMap(buf_number);
		break;
	}
	case 1:
	{
		unsigned int buf_number(0);
		std::stringstream sstr(std::string(MapIDEntry->GetText()));
		sstr >> buf_number;
		LoadMap(buf_number);
		break;
	}
	case 2:ClearMap();
		break;
	case 3://Tile Selector
	{
		if (windowTiles->IsLocallyVisible())
			windowTiles->Show(false);
		else
			windowTiles->Show(true);
		break;
	}
	case 100://Console
		if (CommandWindow->IsLocallyVisible())
			CommandWindow->Show(false);
		else
			CommandWindow->Show(true);
		break;
	case 101://Layers
		if (LayerWindow->IsLocallyVisible())
			LayerWindow->Show(false);
		else
			LayerWindow->Show(true);
		break;
	case 102:
	{
		int buf_number(0);
		std::stringstream sstr(std::string(NewLayerEntry->GetText()));
		sstr >> buf_number;
		int Index_ = WCR.MapPtr->addLayer(buf_number);
		std::stringstream backtoStr;
		backtoStr << buf_number;
		LayerList->InsertItem(Index_, backtoStr.str());
		NewLayerEntry->SetText("");
		break;
		//NewLayerEntry
	}
	case 103:curTileLayer = WCR.MapPtr->OrderedBackgroundLayers[LayerList->GetSelectedItem()]; cout << "Tile layer is now: " << curTileLayer << endl; break;
	case 104:WCR.MapPtr->BackgroundMatrix[LayerList->GetSelectedItem()] = vector<vector<mapObject>>(WCR.MapPtr->MapWidth, vector<mapObject>(WCR.MapPtr->MapHeight));break;
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
	
	FMuse.loadByte();//R
	FMuse.loadByte();//G
	FMuse.loadByte();//B

	int Blocks_ = FMuse.load4Bytes();
	for (int i = 0; i < Blocks_; i++)
	{
		int BlockX = FMuse.load4Bytes();
		int BlockY = FMuse.load4Bytes();
		WCR.MapPtr->MapMatrix[BlockX][BlockY].objectType = FMuse.loadByte();
		WCR.MapPtr->MapMatrix[BlockX][BlockY].tileID = FMuse.loadByte();
		WCR.MapPtr->MapMatrix[BlockX][BlockY].tileSetID = FMuse.loadByte();
		WCR.MapPtr->MapMatrix[BlockX][BlockY].pixelPerfect = FMuse.loadByte();
	}
	int Layers_ = FMuse.load4Bytes();
	WCR.MapPtr->BackgroundMatrix = vector<vector<vector<mapObject>>>(Layers_, vector<vector<mapObject>>(WCR.MapPtr->MapWidth, vector<mapObject>(WCR.MapPtr->MapHeight)));
	WCR.MapPtr->BackgroundLayers.clear();
	WCR.MapPtr->OrderedBackgroundLayers.clear();
	LayerList->Clear();
	for (int i = 0; i < Layers_; i++)
	{
		int LayerId_ = FMuse.load4Bytes();
		WCR.MapPtr->BackgroundLayers.push_back(LayerId_);
		stringstream BacktoStr_;
		BacktoStr_ << LayerId_;
		bool foundSpot = false;
		for (int ii = 0; ii < i; ii++)
			if (LayerId_ > WCR.MapPtr->BackgroundLayers[WCR.MapPtr->OrderedBackgroundLayers[ii]])
			{
				WCR.MapPtr->OrderedBackgroundLayers.insert(WCR.MapPtr->OrderedBackgroundLayers.begin() + ii, i);
				foundSpot = true;
				LayerList->InsertItem(ii, BacktoStr_.str());
				break;
			}
		if (!foundSpot)
		{
			LayerList->AppendItem(BacktoStr_.str());
			WCR.MapPtr->OrderedBackgroundLayers.push_back(i);
		}

		Blocks_ = FMuse.load4Bytes();
		for (int ii = 0; ii < Blocks_; ii++)
		{
			int BlockX = FMuse.load4Bytes();
			int BlockY = FMuse.load4Bytes();
			WCR.MapPtr->BackgroundMatrix[i][BlockX][BlockY].tileID = FMuse.loadByte();
			WCR.MapPtr->BackgroundMatrix[i][BlockX][BlockY].tileSetID = FMuse.loadByte();
			cout << "Adding layer " << i << "block at " << BlockX << ", " << BlockY << " with TID: " << WCR.MapPtr->BackgroundMatrix[i][BlockX][BlockY].tileID << endl;
		}
	}
	WCR.MapPtr->setupBorders();
	cout << "Loaded MAP ID: " << MID << endl;

#ifdef MapMakerMode//Send map data on load from server
	if (WCR.connected)
	{
		sf::Packet mapData;
		mapData << (sf::Int32)0;
		mapData << (sf::Int32)WCR.MapPtr->MapWidth << (sf::Int32)WCR.MapPtr->MapHeight << WCR.PlrPtr->x << WCR.PlrPtr->y;

		for (int i = 0; i < WCR.MapPtr->MapWidth; i++)
			for (int ii = 0; ii < WCR.MapPtr->MapHeight; ii++)
				mapData << (sf::Int32)WCR.MapPtr->MapMatrix[i][ii].objectType << (sf::Int32)WCR.MapPtr->MapMatrix[i][ii].tileID << (sf::Int32)WCR.MapPtr->MapMatrix[i][ii].tileSetID << WCR.MapPtr->MapMatrix[i][ii].pixelPerfect;
		for (int i = 0; i < WCR.clients.size(); i++)
			if (WCR.clients[i] != nullptr)
				WCR.clients[i]->send(mapData);
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
	//Placeholder for new map files, as they will have RGB
	FMuse.saveByte(255);//R
	FMuse.saveByte(255);//G
	FMuse.saveByte(255);//B
	int count_ = 0;
	for (int i = 0; i < WCR.MapPtr->MapWidth; i++)
		for (int ii = 0; ii < WCR.MapPtr->MapHeight; ii++)
			if (WCR.MapPtr->MapMatrix[i][ii].tileID != -1)
				count_++;
	FMuse.save4Bytes(count_);//Map Blocks to save
	for (int i = 0; i < WCR.MapPtr->MapWidth; i++)
		for (int ii = 0; ii < WCR.MapPtr->MapHeight; ii++)
			if (WCR.MapPtr->MapMatrix[i][ii].tileID != -1)
			{
				FMuse.save4Bytes(i);
				FMuse.save4Bytes(ii);
				FMuse.saveByte(WCR.MapPtr->MapMatrix[i][ii].objectType);
				FMuse.saveByte(WCR.MapPtr->MapMatrix[i][ii].tileID);//+1 because -1 is reserved for nothing.
				FMuse.saveByte(WCR.MapPtr->MapMatrix[i][ii].tileSetID);
				FMuse.saveByte(WCR.MapPtr->MapMatrix[i][ii].pixelPerfect);
			}
	FMuse.save4Bytes(WCR.MapPtr->BackgroundMatrix.size());//Layers to save
	for (int i = 0; i < WCR.MapPtr->BackgroundMatrix.size(); i++)
	{
		FMuse.save4Bytes(WCR.MapPtr->BackgroundLayers[i]);//Layer ID (Not vector ID, but what was typed in, can order them later on load)
		count_ = 0;
		for (int ii = 0; ii < WCR.MapPtr->MapWidth; ii++)
			for (int iii = 0; iii < WCR.MapPtr->MapHeight; iii++)
				if (WCR.MapPtr->BackgroundMatrix[i][ii][iii].tileID != -1)
					count_++;
		FMuse.save4Bytes(count_);//Amoount of blocks on this layer.
		for (int ii = 0; ii < WCR.MapPtr->MapWidth; ii++)
			for (int iii = 0; iii < WCR.MapPtr->MapHeight; iii++)
				if (WCR.MapPtr->BackgroundMatrix[i][ii][iii].tileID != -1)
				{
					FMuse.save4Bytes(ii);
					FMuse.save4Bytes(iii);
					FMuse.saveByte(WCR.MapPtr->BackgroundMatrix[i][ii][iii].tileID);//+1 because -1 is reserved for nothing.
					FMuse.saveByte(WCR.MapPtr->BackgroundMatrix[i][ii][iii].tileSetID);
				}
	}

	cout << "Saved MAP ID: " << MID << endl;
	return true;
}

void MapMaker::Draw()
{
	mmWindow.clear(sf::Color(0,0,0,0));
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
	sf::Vector2i mousePos = sf::Mouse::getPosition(mmWindow) - sf::Vector2i(absolutePosition.x, absolutePosition.y);
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
	
	//cout << MouseOverWindow << "_" << WindowIsHovered[0] << "_" << WindowIsHovered[1] << endl

	TileCanvas->Display();
	TileCanvas->Unbind();
	sfgui.Display(mmWindow);

	mmWindow.display();
}


void MapMaker::Step()
{
	while (mmWindow.pollEvent(event))
	{
		PollGUIEvents();
		if (event.type == sf::Event::Closed)
		{
			mmWindow.close();
			WCR.RenderRef.close();//Close main window.
		}
		if (event.type == sf::Event::Resized)
		{
			mmView.setSize(mmWindow.getSize().x, mmWindow.getSize().y);
			mmWindow.setSize(sf::Vector2u(floorf(mmWindow.getSize().x / 2) * 2, floorf(mmWindow.getSize().y / 2) * 2));
		}
	}

	static bool LCtrlPressed_ = false;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && WCR.RenderRef.hasFocus())
	{
		PixelPerfectToggle->SetActive(true);
		pixelPerfectBlocks = true;
		LCtrlPressed_ = true;
	}
	else
		if (LCtrlPressed_)
		{
			PixelPerfectToggle->SetActive(false);
			pixelPerfectBlocks = false;
			LCtrlPressed_ = false;
		}


	if (MouseOverWindow == 0 && !pressedOffScreen && WCR.RenderRef.hasFocus())//Or just check if false?
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			int PlaceX = floor((sf::Mouse::getPosition(WCR.RenderRef).x + MapMakrView.getCenter().x - MapMakrView.getSize().x / 2) / 32);
			int PlaceY = floor((sf::Mouse::getPosition(WCR.RenderRef).y + MapMakrView.getCenter().y - MapMakrView.getSize().y / 2) / 32);
			if (!PlacingObjects->IsActive())
			{
				if (!WCR.MapPtr->isBg(PlaceX, PlaceY, curTileLayer))//If no object already here
					WCR.MapPtr->SetBG(PlaceX, PlaceY, curTile, curTileSet, curTileLayer);
			}
			else
			{
				if (!WCR.MapPtr->isObject(PlaceX, PlaceY))//If no object already here
					WCR.MapPtr->SetObject(PlaceX, PlaceY, CurBlock, curTile, curTileSet, pixelPerfectBlocks);
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
		{
			int PlaceX = floor((sf::Mouse::getPosition(WCR.RenderRef).x + MapMakrView.getCenter().x - MapMakrView.getSize().x / 2) / 32);
			int PlaceY = floor((sf::Mouse::getPosition(WCR.RenderRef).y + MapMakrView.getCenter().y - MapMakrView.getSize().y / 2) / 32);
			if (!PlacingObjects->IsActive())
			{
				if (WCR.MapPtr->isBg(PlaceX, PlaceY, curTileLayer))//If object is here
					WCR.MapPtr->SetBG(PlaceX, PlaceY, -1, 0, curTileLayer);
			}
			else
			{
				if (WCR.MapPtr->isObject(PlaceX, PlaceY))//If object is here
					WCR.MapPtr->SetObject(PlaceX, PlaceY, 0, -1, 0);
			}
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
	
	if (WindowIsHovered[1] && mmWindow.hasFocus())//If over tile window
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
			sf::Vector2i mousePos = sf::Mouse::getPosition(mmWindow) - sf::Vector2i(absolutePosition.x, absolutePosition.y);
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

#ifdef MapMakerMode//Send new map data on clear
	if (WCR.connected)
	{
		sf::Packet mapData;
		mapData << (sf::Int32)0;
		mapData << (sf::Int32)WCR.MapPtr->MapWidth << (sf::Int32)WCR.MapPtr->MapHeight << WCR.PlrPtr->x << WCR.PlrPtr->y;

		for (int i = 0; i < WCR.MapPtr->MapWidth; i++)
			for (int ii = 0; ii < WCR.MapPtr->MapHeight; ii++)
				mapData << (sf::Int32)WCR.MapPtr->MapMatrix[i][ii].objectType << (sf::Int32)WCR.MapPtr->MapMatrix[i][ii].tileID << (sf::Int32)WCR.MapPtr->MapMatrix[i][ii].tileSetID << WCR.MapPtr->MapMatrix[i][ii].pixelPerfect;
		for (int i = 0; i < WCR.clients.size(); i++)
			if (WCR.clients[i] != nullptr)
				WCR.clients[i]->send(mapData);
	}
#endif

}

void MapMaker::setBlock(int BLK)
{
	cout << "Setting block to: " << BLK << endl;
	if (BLK >= 0 && BLK <= 5)
		CurBlock = BLK;
	else
		cout << "This object ID does not exist!" << endl;
}

int MapMaker::getBlock()
{
	return CurBlock;
}