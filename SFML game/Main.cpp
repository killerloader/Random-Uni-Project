#include "Main.h"
/*
TODO: 
	- Give player's IDs that show above their head.
	- Using player IDs, make commands which can be entered into the server to do stuff:
		- Teleport, kick, etc...
	- Clear on client.
	- Colours for players.
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
			//can only send one message below? (Works on my pc right, but not at UNI...)
			
			for (int i = 0; i < WC.clients.size(); i++)//hasn't been added to vector yet so this wont find self!
			{
				if (WC.clients[i] == nullptr || i == foundEmpty)
					continue;
				//successfully sends but breaks.
				mapData.clear();
				mapData << (sf::Int32)2 << (sf::Int32)0 << (sf::Int32)i;
				WC.clients[foundEmpty]->send(mapData);
				//cout << "(2) Sending " << i << " to " << foundEmpty << endl;
			}				
			//---

			//create otherplayer object for server.
			if (WC.otherPlayers[foundEmpty] == nullptr)
				WC.otherPlayers[foundEmpty] = new otherPlayer(WC);
			
			//reset temp client.
			WC.client = new sf::TcpSocket;
			WC.client->setBlocking(false);
		}

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
						//jump otherplayer object
						WC.otherPlayers[i]->vspeed = -10;
						WC.otherPlayers[i]->falling = true;
						WC.otherPlayers[i]->PlayerImage.setPosition(WC.otherPlayers[i]->x, WC.otherPlayers[i]->y);
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
						//Move otherplayer object.
						WC.otherPlayers[i]->x = x_;
						WC.otherPlayers[i]->y = y_;
						WC.otherPlayers[i]->xdir_ = xDIr;
						WC.otherPlayers[i]->PlayerImage.setPosition(WC.otherPlayers[i]->x, WC.otherPlayers[i]->y);
						break;
					}
					}
				}
			}
			//Remove other player.
			if (Status_ == sf::Socket::Disconnected)
			{
				if (WC.otherPlayers[i] != nullptr)
				{
					delete WC.otherPlayers[i];
					WC.otherPlayers[i] = nullptr;
				}
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
			//"10.17.24.161"
			sf::Socket::Status status = WC.socket.connect("127.0.0.1", 53000);
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
							//cout << "Player created: " << OPID << endl;
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
		#else
			MapMkr.Step();
			window.setView(MapMkr.MapMakrView);
		#endif
		for (int i = 0; i < WC.otherPlayers.size(); i++)
			if (WC.otherPlayers[i] != nullptr)
				WC.otherPlayers[i]->step();
		//Clear the window so we can draw new stuff to it.

		window.clear();
		MainMap.DrawMap(obj_Player.PlayerView);
		MainMap.drawBorders();
		obj_Player.DrawPlayer();
#ifdef MapMakerMode//Don't poll player events if in mapmaker mode.
		MapMkr.Draw();
#endif
		for (int i = 0; i < WC.otherPlayers.size(); i++)
			if (WC.otherPlayers[i] != nullptr)
				WC.otherPlayers[i]->draw();
		//Display the window to the client.
		window.display();
	}

	return 0;
}