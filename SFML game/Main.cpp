#include "Main.h"
/*
TODO: 
	- Add other players to server.
	- Clear on client.
	- Maximum fall speed.
	- Port player class and wrapper class to own files.
	- Colours for players.
	- Server commands and 
	- Fix headers, do not include .h files in headers (or include one which only has predefinitions)
*/

void WrapperClass::LimitVariable(int Min, int Max, int& Var)
{
	if (Var < Min)
		Var = Min;
	if (Var > Max)
		Var = Max;
}

WrapperClass::WrapperClass(sf::RenderWindow &RenderRef_) : RenderRef(RenderRef_)
{
	otherPlayers = vector<otherPlayer*>(256, nullptr);

	curmapID = 0;
}

PlayerObject::PlayerObject(WrapperClass &WCR_) : WCR(WCR_) {
	PlayerTex.loadFromFile("Data/Sprites/Player.png");
	PlayerImage.setTexture(PlayerTex);
	PlayerView.setSize(sf::Vector2f(640, 480));
	//vspeed, hspeed, gravity, haccel, hspeedmax, hfric;
	hspeed = 0;
	haccel = 0.4;
	hspeedmax = 5;
	hfric = 0.3;//How fast you slow down if you release all keys
	x = 300.f;
	y = 300.f;
	BoundBox.width = 32;
	BoundBox.height = 32;
	SPD = 5.f;
	falling = true;
	gravity = 0.5;//acceleration on vspeed
	vspeed = 0;
	xDirOld = 0; 
	pressWOld = false;
	//PlayerImage.setSize(sf::Vector2f(32,32));
	PlayerImage.setPosition(x, y);
}

void PlayerObject::ContractDir(Edirection DIrr)
{
	int Xdir=0;
	int Ydir=0;
	switch (DIrr)
	{
	case E_up:Ydir = -1; break;
	case E_down:Ydir = 1; break;
	case E_left:Xdir = -1; break;
	case E_right:Xdir = 1; break;
	}

	while (WCR.MapPtr->CheckCollision(BoundBox, x + Xdir, y + Ydir, 1) != 1 && WCR.MapPtr->CheckCollision(BoundBox, x + Xdir, y + Ydir, 0) != 1)//only solid objects
		MovePlayer(Xdir, Ydir);
}

void PlayerObject::StepPlayer()
{
	AfterImage.emplace_back(PlayerTex);
	AfterImage[AfterImage.size() - 1].setPosition(x, y);
	//AfterImage[AfterImage.size() - 1].setFillColor(sf::Color::White);
	//after image step
	int AlphaDec = 15;
	for (int i = 0; i < AfterImage.size(); i++)
	{
		if(AfterImage[i].getColor().a- AlphaDec <=0)
			AfterImage.erase(AfterImage.begin() + i);
		AfterImage[i].setColor(sf::Color(255, 255, 255,AfterImage[i].getColor().a- AlphaDec));
	}
	
	PollControls();
	//vspeed, hspeed, gravity, haccel, hspeedmax;
	if (hspeed != 0)
	{
		if (WCR.MapPtr->CheckCollision(BoundBox, x + hspeed, y, 1) == 1 || WCR.MapPtr->CheckCollision(BoundBox, x + hspeed, y, 0) == 1)
		{
			if (hspeed > 0)
				ContractDir(E_right);
			if (hspeed < 0)
				ContractDir(E_left);
			hspeed = 0;
		}
		else
			MovePlayer(hspeed, 0);
	}
	if (falling)
	{
		if (vspeed < 0)
		{
			//WCR.MapPtr->CheckCollision(BoundBox, x + hspeed, y)
			if (WCR.MapPtr->CheckCollision(BoundBox, x, y + vspeed, 1)==1 || WCR.MapPtr->CheckCollision(BoundBox, x, y + vspeed, 0) == 1)
			{
				ContractDir(E_up);
				vspeed = 0;
			}
			else
				MovePlayer(0, vspeed);
		}
		else
		{
			if (WCR.MapPtr->CheckCollision(BoundBox, x, y + vspeed, 1) == 1 || WCR.MapPtr->CheckCollision(BoundBox, x, y + vspeed, 0) == 1)
			{
				falling = false;
				vspeed = 0;
				//Contract to ground.
				ContractDir(E_down);
			}
			else
				MovePlayer(0, vspeed);
		}
		vspeed += gravity;
	}
	else
	{
		if (WCR.MapPtr->CheckCollision(BoundBox, x, y + 1, 1) != 1)//not a solid
			falling = true;
	}
	if (WCR.MapPtr->CheckCollision(BoundBox, x, y, 2) == 2)//Bouncy Block
	{
		vspeed = -15;
		falling = true;
	}
	if (WCR.MapPtr->CheckCollision(BoundBox, x, y, 3) == 3 || WCR.MapPtr->CheckCollision(BoundBox, x, y, -1) == -1)//Bouncy Block
	{
		x = xstart;
		y = ystart;
		PlayerImage.setPosition(x, y);
	}
	if (WCR.MapPtr->CheckCollision(BoundBox, x, y, 4) == 4)//Next Level Block
	{
		WCR.curmapID++;//next map
		ResetMovement();
		if (!WCR.MMPtr->LoadMap(WCR.curmapID))
			cout << "Map load failed!" << endl;
	}
	
	PlayerView.setCenter(sf::Vector2f((int)x+16, (int)y+16));
}

void PlayerObject::ResetMovement()
{
	vspeed = 0;
	hspeed = 0;
	falling = false;
}

void PlayerObject::PollControls() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && WCR.RenderRef.hasFocus())
	{
		//vspeed, hspeed, gravity, haccel, hspeedmax;
		if (hspeed - haccel >= -hspeedmax)
			hspeed -= haccel;
		else
			hspeed = -hspeedmax;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && WCR.RenderRef.hasFocus())
	{
		//vspeed, hspeed, gravity, haccel, hspeedmax;
		if (hspeed + haccel <= hspeedmax)
			hspeed += haccel;
		else
			hspeed = hspeedmax;
	}
	else if (hspeed != 0)
	{
			if (hspeed > 0)
			{
				if (hspeed - hfric >= 0)
					hspeed -= hfric;
				else
					hspeed = 0;
			}
			else
			{
				if (hspeed + hfric <= 0)
					hspeed += hfric;
				else
					hspeed = 0;
			}

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && WCR.RenderRef.hasFocus())
		if (WCR.MapPtr->CheckCollision(BoundBox, x, y + 1, 1))
		{
			vspeed = -10;
			sendJump();
			falling = true;
		}
	sendXChange();
}

void PlayerObject::MovePlayer(float Xmove, float Ymove)
{
	x += Xmove;
	y += Ymove;
	PlayerImage.setPosition(x,y);
}

void PlayerObject::sendJump()
{
	if (!WCR.online)
		return;
	sf::Packet sendData;
	sendData << (sf::Int32)2 << (sf::Int32)0 << (sf::Int32)x << (sf::Int32)y;
	WCR.socket.send(sendData);
}

void PlayerObject::sendXChange()
{
	if (!WCR.online)
		return;
	/*
	xDirOld = 0;
	pressWOld = false;
	*/
	int xdir_ = 0;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && WCR.RenderRef.hasFocus())
	{
		xdir_ = -1;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && WCR.RenderRef.hasFocus())
	{
		xdir_ = 1;
	}
	if (xdir_ != xDirOld)
	{
		xDirOld = xdir_;
		sf::Packet sendData;
		sendData << (sf::Int32)2 << (sf::Int32)1 << (sf::Int32)x << (sf::Int32)y << (sf::Int32)xdir_;
		WCR.socket.send(sendData);
	}
}

void PlayerObject::DrawPlayer() {

	for (int i = 0; i < AfterImage.size(); i++)
		WCR.RenderRef.draw(AfterImage[i]);
	WCR.RenderRef.draw(PlayerImage);
}

enum Etest//Random pointless enum test
{
	test1,//id = 0
	test2,//id = 1
	test3//id = 2
};

int main()
{
	//Setup window and window settings.
	sf::RenderWindow window(sf::VideoMode(640, 480), "Unnamed project");
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);

	//Class Setup
	WrapperClass WC(window);
	PlayerObject obj_Player(WC);
	Map MainMap(WC);
	WC.PlrPtr = &obj_Player;
	WC.MapPtr = &MainMap;
	//Initialize mapmaker if in mapmaker mode.
	//#if MapMakerMode == true
	MapMaker MapMkr(WC);
#ifndef MapMakerMode
	if (!MapMkr.LoadMap(WC.curmapID))
		cout << "Map load failed!" << endl;
#endif
	WC.MMPtr = &MapMkr;
	//#endif

#ifdef MapMakerMode//start server.
	WC.listener.setBlocking(false);
	cout << "Attemtping to bind port..." << endl;
	// bind the listener to a port
	if (WC.listener.listen(53000) != sf::Socket::Done)
	{
		// error...
		cout << "Cannot bind port!" << endl;
	}
	else
	{
		WC.connected = true;
		cout << "Port bind successful!" << endl;
	}
	// accept a new connection
	WC.client = new sf::TcpSocket;
	WC.client->setBlocking(false);
	//WC.client.setBlocking(false);
	cout << "Listening for connection..." << endl;
#else
	cout << "Attempting to connect to server..." << endl;
	WC.socket.setBlocking(false);
#endif
	

	//Start the game loop.
	while (window.isOpen())
	{
#ifdef MapMakerMode//start server.
		if (WC.listener.accept(*WC.client) == sf::Socket::Done)
		{
			cout << "Connection found!" << endl;
			WC.connected = true;
			//Send map:
			sf::Packet mapData;
			mapData << (sf::Int32)0;
			mapData << (sf::Int32)WC.MapPtr->MapWidth << (sf::Int32)WC.MapPtr->MapHeight << (sf::Int32)WC.PlrPtr->x << (sf::Int32)WC.PlrPtr->y;

			for (int i = 0; i < WC.MapPtr->MapWidth; i++)
				for (int ii = 0; ii < WC.MapPtr->MapHeight; ii++)
				{
					mapData << (sf::Int32)WC.MapPtr->MapMatrix[i][ii].objectType << (sf::Int32)WC.MapPtr->MapMatrix[i][ii].tileID << (sf::Int32)WC.MapPtr->MapMatrix[i][ii].tileSetID;
				}
					
			WC.client->send(mapData);
			cout << "Sent map data!" << endl;

			int foundEmpty = -1;

			for (int i = 0; i < WC.clients.size(); i++)
				if (WC.clients[i] == nullptr)
				{
					foundEmpty = i;
					break;
				}
			if (foundEmpty != -1)
				WC.clients[foundEmpty] = WC.client;
			else
				WC.clients.push_back(WC.client);

			if (foundEmpty == -1)
				foundEmpty = WC.clients.size()-1;

			//Send to other players
			mapData.clear();
			mapData << (sf::Int32)2 << (sf::Int32)0 << (sf::Int32)foundEmpty;
			for (int i = 0; i < WC.clients.size(); i++)//hasn't been added to vector yet so this wont find self!
				if (WC.clients[i] != nullptr && i != foundEmpty)//only send alive players
				{
					WC.clients[i]->send(mapData);
					//cout << "(1) Sending " << foundEmpty << " to " << i << endl;
				}
			//---
			//Send others to self
			//can only send one message? (Maybe limitation of receiving)
			mapData.clear();
			for (int i = 0; i < WC.clients.size(); i++)//hasn't been added to vector yet so this wont find self!
			{
				if (WC.clients[i] == nullptr || i == foundEmpty)
					continue;
				//successfully sends but breaks.
				
				mapData << (sf::Int32)2 << (sf::Int32)0 << (sf::Int32)i;
				WC.clients[foundEmpty]->send(mapData);
				//cout << "(2) Sending " << i << " to " << foundEmpty << endl;
			}				
			//---
			
			//reset temp client.
			WC.client = new sf::TcpSocket;
			WC.client->setBlocking(false);
		}
		/* - semi fixes problems with not loading other players, but also breaks other stuff.
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && WC.RenderRef.hasFocus())
		{
			for (int ii = 0; ii < WC.clients.size(); ii++)//hasn't been added to vector yet so this wont find self!
			{
				sf::Packet mapData;
				mapData << (sf::Int32)2 << (sf::Int32)1 << (sf::Int32)ii;
				for (int i = 0; i < WC.clients.size(); i++)//hasn't been added to vector yet so this wont find self!
					if (WC.clients[i] != nullptr && i != ii)//only send alive players
					{
						WC.clients[i]->send(mapData);
						cout << "(1) Redelete " << ii << " to " << i << endl;
					}

				mapData.clear();
				mapData << (sf::Int32)2 << (sf::Int32)0 << (sf::Int32)ii;
				for (int i = 0; i < WC.clients.size(); i++)//hasn't been added to vector yet so this wont find self!
					if (WC.clients[i] != nullptr && i != ii)//only send alive players
					{
						WC.clients[i]->send(mapData);
						cout << "(1) ReSending " << ii << " to " << i << endl;
					}
				//---
				//Send others to self
				//can only send one message? (Maybe limitation of receiving)
				for (int i = 0; i < WC.clients.size(); i++)//hasn't been added to vector yet so this wont find self!
				{
					if (WC.clients[i] == nullptr || i == ii)
						continue;

					cout << "(2) Redelete " << i << " to " << ii << endl;
					mapData.clear();
					mapData << (sf::Int32)2 << (sf::Int32)1 << (sf::Int32)i;
					WC.clients[ii]->send(mapData);
				}

				for (int i = 0; i < WC.clients.size(); i++)//hasn't been added to vector yet so this wont find self!
				{
					if (WC.clients[i] == nullptr || i == ii)
						continue;

					cout << "(2) ReSending " << i << " to " << ii << endl;
					mapData.clear();
					mapData << (sf::Int32)2 << (sf::Int32)0 << (sf::Int32)i;
					WC.clients[ii]->send(mapData);
				}
			}
		}*/

		for (int i = 0; i < WC.clients.size(); i++)
		{
			if (WC.clients[i] == nullptr)
				break;
			sf::Packet recievedata;
			sf::Socket::Status Status_ = WC.clients[i]->receive(recievedata);
			if (Status_ == sf::Socket::Done)
			{
				sf::Int32 messageType;
				recievedata >> messageType;
				switch (messageType)
				{
				case 2://PlayerMessage
					sf::Int32 pMessageType;
					recievedata >> pMessageType;
					switch (pMessageType)
					{
					case 0://Jump
						   //Might be able to just send the packet back? although it may get deleted after use...
					{
						sf::Int32 x_, y_, hspeed_, vspeed_;
						recievedata >> x_ >> y_;
						for (int ii = 0; ii < WC.clients.size(); ii++)
						{
							if (ii == i || WC.clients[ii] == nullptr)
								continue;
							sf::Packet sendData;
							sendData << (sf::Int32)2 << (sf::Int32)2 << (sf::Int32)0 << (sf::Int32)i;
							sendData << x_ << y_;
							WC.clients[ii]->send(sendData);
						}
						break;
					}
					case 1://xdir change
						   //Might be able to just send the packet back? although it may get deleted after use...
					{
						sf::Int32 x_, y_, xDIr;
						recievedata >> x_ >> y_ >> xDIr;
						for (int ii = 0; ii < WC.clients.size(); ii++)
						{
							if (ii == i || WC.clients[ii] == nullptr)
								continue;
							sf::Packet sendData;
							sendData << (sf::Int32)2 << (sf::Int32)2 << (sf::Int32)1 << (sf::Int32)i << (sf::Int32)xDIr;
							sendData << x_ << y_;
							WC.clients[ii]->send(sendData);
							continue;//no point
						}
						break;
					}
					}
				}
			}
			//Remove other player.
			if (Status_ == sf::Socket::Disconnected)
			{
				cout << "Client has disconnected!" << endl;
				sf::Packet sendData;
				sendData << (sf::Int32)2 << (sf::Int32)1 << (sf::Int32)i;
				for (int ii = 0; ii < WC.clients.size(); ii++)
				{
					if (ii == i || WC.clients[ii] == nullptr)
						continue;
					WC.clients[ii]->send(sendData);
				}
				delete WC.clients[i];
				WC.clients[i] = nullptr;
			}
		}
		//Poll events, keyboard wont be placed in here.
#else
		if (!WC.online)
		{
			sf::Socket::Status status = WC.socket.connect("10.17.24.161", 53000);
			if (status == sf::Socket::Done)
			{
				WC.online = true;
				cout << "Connection to server successful!" << endl;
			}
		}
		else
		{
			sf::Packet recievedata;
			if(WC.socket.receive(recievedata) == sf::Socket::Done)
			{
				sf::Int32 messageType;
				recievedata >> messageType;
				switch (messageType)
				{
				case 0://Receive map
					sf::Int32 mapSizeX, mapSizeY, Px_, Py_;
					recievedata >> mapSizeX >> mapSizeY >> Px_ >> Py_;
					WC.MapPtr->MapWidth = mapSizeX;
					WC.MapPtr->MapHeight = mapSizeY;
					WC.PlrPtr->x = Px_;
					WC.PlrPtr->y = Py_;
					WC.PlrPtr->PlayerImage.setPosition(WC.PlrPtr->x, WC.PlrPtr->y);
					WC.PlrPtr->xstart = WC.PlrPtr->x;
					WC.PlrPtr->ystart = WC.PlrPtr->y;
					//Error here, when different map size.
					WC.MapPtr->MapMatrix = vector<vector<mapObject>>(WC.MapPtr->MapWidth, vector<mapObject>(WC.MapPtr->MapHeight));
					for (int i = 0; i < mapSizeX; i++)
						for (int ii = 0; ii < mapSizeY; ii++)
						{
							sf::Int32 objectT, tileID, tileSID;
							recievedata >> objectT >> tileID >> tileSID;
							WC.MapPtr->MapMatrix[i][ii].objectType = objectT;
							WC.MapPtr->MapMatrix[i][ii].tileID = tileID;
							WC.MapPtr->MapMatrix[i][ii].tileSetID = tileSID;
						}
					WC.MapPtr->setupBorders();
					cout << "Loaded map from server!" << endl;
					break;
				case 1://Receive block change
					//(int x, int y, int ID, int TID, int TSID)
					sf::Int32 x, y, ID, TID, TSID;
					recievedata >> x >> y >> ID >> TID >> TSID;
					WC.MapPtr->MapMatrix[x][y].objectType = ID;
					WC.MapPtr->MapMatrix[x][y].tileID = TID;
					WC.MapPtr->MapMatrix[x][y].tileSetID = TSID;
					break;
				case 2://Other Player
					sf::Int32 oPlayerMessage;
					recievedata >> oPlayerMessage;
					{
						switch (oPlayerMessage)
						{
						case 0://create new player
						{
							sf::Int32 OPID;
							recievedata >> OPID;
							cout << "Player created: " << OPID << endl;
							if(WC.otherPlayers[OPID] == nullptr)
								WC.otherPlayers[OPID] = new otherPlayer(WC);
							break;
						}
						case 1://remove player
						{
							sf::Int32 OPID;
							recievedata >> OPID;
							if (WC.otherPlayers[OPID] != nullptr)
							{
								delete WC.otherPlayers[OPID];
								WC.otherPlayers[OPID] = nullptr;
							}
							break;
						}
						case 2://movement
						{
							//cout << "Player moved." << endl;
							sf::Int32 PMEssage;
							recievedata >> PMEssage;
							switch (PMEssage)
							{
							case 0:
								//Just get all speeds and positions.
							{
								sf::Int32 OPID, x_, y_;
								recievedata >> OPID;
								//cout << OPID << "_" << WC.otherPlayers.size() << endl;
								if (WC.otherPlayers[OPID] != nullptr)
								{
									WC.otherPlayers[OPID]->vspeed = -10;
									WC.otherPlayers[OPID]->falling = true;
									WC.otherPlayers[OPID]->PlayerImage.setPosition(WC.otherPlayers[OPID]->x, WC.otherPlayers[OPID]->y);
								}
								break;
							}
							case 1:
								//Just get all speeds and positions.
							{
								sf::Int32 OPID, x_, y_, xdirnew;
								recievedata >> OPID;
								//cout << OPID << "_" << WC.otherPlayers.size() << endl;
								if (WC.otherPlayers[OPID] != nullptr)
								{
									recievedata >> xdirnew >> x_ >> y_;
									WC.otherPlayers[OPID]->x = x_;
									WC.otherPlayers[OPID]->y = y_;
									WC.otherPlayers[OPID]->xdir_ = xdirnew;
									WC.otherPlayers[OPID]->PlayerImage.setPosition(WC.otherPlayers[OPID]->x, WC.otherPlayers[OPID]->y);
								}
								break;
							}
							}
						}
						}
					}
					break;
				}
			}
		}
#endif

		while (window.pollEvent(WC.event))
		{
			if (WC.event.type == sf::Event::Closed)
				window.close();
		}
		//Get player controls.
		#ifndef MapMakerMode//Don't poll player events if in mapmaker mode.
			obj_Player.StepPlayer();
			window.setView(obj_Player.PlayerView);

			for (int i = 0; i < WC.otherPlayers.size(); i++)
				if (WC.otherPlayers[i] != nullptr)
					WC.otherPlayers[i]->step();
		#else
			MapMkr.Step();
			window.setView(MapMkr.MapMakrView);
		#endif
		//Clear the window so we can draw new stuff to it.

		window.clear();
		MainMap.DrawMap(obj_Player.PlayerView);
		MainMap.drawBorders();
		obj_Player.DrawPlayer();
#ifdef MapMakerMode//Don't poll player events if in mapmaker mode.
		MapMkr.Draw();
#else
		for (int i = 0; i < WC.otherPlayers.size(); i++)
			if (WC.otherPlayers[i] != nullptr)
				WC.otherPlayers[i]->draw();
#endif
		//Display the window to the client.
		window.display();
	}

	return 0;
}