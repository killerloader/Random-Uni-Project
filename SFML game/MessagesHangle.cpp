#include "Main.h"

MessagesHangle::MessagesHangle(WrapperClass &WCR_) : WCR(WCR_)
{
}

MessagesHangle::~MessagesHangle()
{
}

void MessagesHangle::sendData(sf::Packet& Data_, int PID)
{
	//Loop till sent.
	while (true)
	{
		sf::Socket::Status Stat = WCR.clients[PID]->send(Data_);

		if (Stat == sf::Socket::Disconnected)
			break;
		else if (Stat != sf::Socket::Done)
			continue;//keep going, not done.
		else
			break;
	}
	//If we add to queue, we have to make sure it knows when the player logs out (unlikely to cause problems due to the scale of the time)
	//A vector that saves how many packets are in the queue for a certain player?
}

void MessagesHangle::sendData(sf::Packet& Data_, sf::TcpSocket& Sock)
{
	//Loop till sent.

	while (true)
	{
		sf::Socket::Status Stat = Sock.send(Data_);
		if (Stat == sf::Socket::Disconnected)
			break;
		else if (Stat != sf::Socket::Done)
			continue;//keep going, not done.
		else
			break;
	}
	//If we add to queue, we have to make sure it knows when the player logs out (unlikely to cause problems due to the scale of the time)
	//A vector that saves how many packets are in the queue for a certain player?
}

void MessagesHangle::ServerMessagesHandle()
{
	if (WCR.listener.accept(*WCR.client) == sf::Socket::Done)
	{
		cout << "Connection found!" << endl;
		WCR.connected = true;
		int foundEmpty = -1;

		for (int i = 0; i < WCR.clients.size(); i++)
			if (WCR.clients[i] == nullptr)
			{
				foundEmpty = i;
				break;
			}
		if (foundEmpty != -1)
			WCR.clients[foundEmpty] = WCR.client;
		else
			WCR.clients.push_back(WCR.client);

		if (foundEmpty == -1)
			foundEmpty = WCR.clients.size() - 1;


		//Send map:
		sf::Packet mapData;
		mapData << (sf::Int32)0;
		mapData << (sf::Int32)WCR.MapPtr->MapWidth << (sf::Int32)WCR.MapPtr->MapHeight << (sf::Int32)WCR.PlrPtr->x << (sf::Int32)WCR.PlrPtr->y;

		for (int i = 0; i < WCR.MapPtr->MapWidth; i++)
			for (int ii = 0; ii < WCR.MapPtr->MapHeight; ii++)
			{
				mapData << (sf::Int32)WCR.MapPtr->MapMatrix[i][ii].objectType << (sf::Int32)WCR.MapPtr->MapMatrix[i][ii].tileID << (sf::Int32)WCR.MapPtr->MapMatrix[i][ii].tileSetID;
			}

		sendData(mapData, foundEmpty);
		//cout << "Sent map data!" << endl;

		//create otherplayer object for server.
		if (WCR.otherPlayers[foundEmpty] == nullptr)
		{
			WCR.otherPlayers[foundEmpty] = new otherPlayer(WCR);
			WCR.otherPlayers[foundEmpty]->MyCol = sf::Color(rand()%256, rand() % 256, rand() % 256, 255);
			WCR.otherPlayers[foundEmpty]->PlayerImage.setColor(WCR.otherPlayers[foundEmpty]->MyCol);
			WCR.otherPlayers[foundEmpty]->PlayerImage.setPosition(WCR.PlrPtr->x, WCR.PlrPtr->y);
			WCR.otherPlayers[foundEmpty]->PID = foundEmpty;
		}
		sf::Packet ColourData;
		ColourData << (sf::Int32)2 << (sf::Int32)4 << (sf::Int32)WCR.otherPlayers[foundEmpty]->MyCol.r << (sf::Int32)WCR.otherPlayers[foundEmpty]->MyCol.g << (sf::Int32)WCR.otherPlayers[foundEmpty]->MyCol.b;
		sendData(ColourData, foundEmpty);

		//Send to other players
		ColourData.clear();
		mapData.clear();
		mapData << (sf::Int32)2 << (sf::Int32)0 << (sf::Int32)foundEmpty << (sf::Int32)WCR.PlrPtr->x << (sf::Int32)WCR.PlrPtr->y;
		ColourData << (sf::Int32)2 << (sf::Int32)3 << (sf::Int32)foundEmpty << (sf::Int32)WCR.otherPlayers[foundEmpty]->MyCol.r << (sf::Int32)WCR.otherPlayers[foundEmpty]->MyCol.g << (sf::Int32)WCR.otherPlayers[foundEmpty]->MyCol.b;
		for (int i = 0; i < WCR.clients.size(); i++)//hasn't been added to vector yet so this wont find self!
			if (WCR.clients[i] != nullptr && i != foundEmpty)//only send alive players
			{
				sendData(mapData, i);
				sendData(ColourData, i);
				//cout << "Sending " << foundEmpty << " Colour to " << i << endl;
				//cout << "(1) Sending " << foundEmpty << " to " << i << endl;
			}
		//NotReady
		//---
		//Send others to self
		//can only send one message below? (Works on my pc right, but not at UNI...)
		//tempfix by sending all at once.
		for (int i = 0; i < WCR.clients.size(); i++)//hasn't been added to vector yet so this wont find self!
		{
			if (WCR.clients[i] == nullptr || i == foundEmpty)
				continue;
			mapData.clear();
			mapData << (sf::Int32)2 << (sf::Int32)0 << (sf::Int32)i << (sf::Int32)WCR.otherPlayers[i]->x << (sf::Int32)WCR.otherPlayers[i]->y;
			sendData(mapData, foundEmpty);
			//cout << "Sending " << i << " Colour to " << foundEmpty << endl;
			ColourData.clear();
			ColourData << (sf::Int32)2 << (sf::Int32)3 << (sf::Int32)i << (sf::Int32)WCR.otherPlayers[i]->MyCol.r << (sf::Int32)WCR.otherPlayers[i]->MyCol.g << (sf::Int32)WCR.otherPlayers[i]->MyCol.b;
			sendData(ColourData, foundEmpty);
		}
		

		//---


		//reset temp client.
		WCR.client = new sf::TcpSocket;
		WCR.client->setBlocking(false);
	}

	//cout << "_____" << WCR.clients.size() << endl;
	for (int i = 0; i < WCR.clients.size(); i++)
	{
		if (WCR.clients[i] == nullptr)
			continue;//used to be break, this should fix some bugs.
		sf::Packet recievedata;
		sf::Socket::Status Status_ = WCR.clients[i]->receive(recievedata);
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
				{
					sf::Int32 x_, y_, hspeed_, vspeed_;
					recievedata >> x_ >> y_;
					for (int ii = 0; ii < WCR.clients.size(); ii++)
					{
						if (ii == i || WCR.clients[ii] == nullptr)
							continue;
						sf::Packet Data_;
						Data_ << (sf::Int32)2 << (sf::Int32)2 << (sf::Int32)0 << (sf::Int32)i;
						Data_ << x_ << y_;
						sendData(Data_, ii);
					}
					//jump otherplayer object
					WCR.otherPlayers[i]->vspeed = -10;
					WCR.otherPlayers[i]->falling = true;
					WCR.otherPlayers[i]->PlayerImage.setPosition(WCR.otherPlayers[i]->x, WCR.otherPlayers[i]->y);
					break;
				}
				case 1://xdir change
				{
					sf::Int32 x_, y_, xDIr;
					recievedata >> x_ >> y_ >> xDIr;
					for (int ii = 0; ii < WCR.clients.size(); ii++)
					{
						if (ii == i || WCR.clients[ii] == nullptr)
							continue;
						sf::Packet Data_;
						Data_ << (sf::Int32)2 << (sf::Int32)2 << (sf::Int32)1 << (sf::Int32)i << (sf::Int32)xDIr;
						Data_ << x_ << y_;
						sendData(Data_, ii);
						continue;//no point
					}
					//Move otherplayer object.
					WCR.otherPlayers[i]->x = x_;
					WCR.otherPlayers[i]->y = y_;
					WCR.otherPlayers[i]->xdir_ = xDIr;
					WCR.otherPlayers[i]->PlayerImage.setPosition(WCR.otherPlayers[i]->x, WCR.otherPlayers[i]->y);
					break;
				}
				case 2://pos change
				{
					sf::Int32 x_, y_;
					recievedata >> x_ >> y_;
					for (int ii = 0; ii < WCR.clients.size(); ii++)
					{
						if (ii == i || WCR.clients[ii] == nullptr)
							continue;
						sf::Packet Data_;
						Data_ << (sf::Int32)2 << (sf::Int32)2 << (sf::Int32)2 << (sf::Int32)i;
						Data_ << x_ << y_;
						sendData(Data_, ii);
						continue;//no point
					}
					//Move otherplayer object.
					WCR.otherPlayers[i]->x = x_;
					WCR.otherPlayers[i]->y = y_;
					WCR.otherPlayers[i]->PlayerImage.setPosition(WCR.otherPlayers[i]->x, WCR.otherPlayers[i]->y);
					break;
				}
				}
			}
		}
		//Remove other player.
		if (Status_ == sf::Socket::Disconnected)
			deletePlayer(i);
	}
}

void MessagesHangle::deletePlayer(int ID)
{
	if (WCR.otherPlayers[ID] != nullptr)
	{
		delete WCR.otherPlayers[ID];
		WCR.otherPlayers[ID] = nullptr;
	}
	cout << "Client has disconnected!" << endl;
	sf::Packet Data_;
	Data_ << (sf::Int32)2 << (sf::Int32)1 << (sf::Int32)ID;
	for (int ii = 0; ii < WCR.clients.size(); ii++)
	{
		if (ii == ID || WCR.clients[ii] == nullptr)
			continue;
		sendData(Data_, ii);
	}
	delete WCR.clients[ID];
	WCR.clients[ID] = nullptr;
}

void MessagesHangle::ClientMessagesHandle()
{
	sf::Packet recievedata;
	sf::Socket::Status sockstat = WCR.socket.receive(recievedata);
	if (sockstat == sf::Socket::Done)
	{
		sf::Int32 messageType;
		recievedata >> messageType;
		switch (messageType)
		{
		case 0://Receive map
			sf::Int32 mapSizeX, mapSizeY, Px_, Py_;
			recievedata >> mapSizeX >> mapSizeY >> Px_ >> Py_;
			WCR.MapPtr->MapWidth = mapSizeX;
			WCR.MapPtr->MapHeight = mapSizeY;
			WCR.PlrPtr->x = Px_;
			WCR.PlrPtr->y = Py_;
			WCR.PlrPtr->PlayerImage.setPosition(WCR.PlrPtr->x, WCR.PlrPtr->y);
			WCR.PlrPtr->xstart = WCR.PlrPtr->x;
			WCR.PlrPtr->ystart = WCR.PlrPtr->y;
			//Error here, when different map size.
			WCR.MapPtr->MapMatrix = vector<vector<mapObject>>(WCR.MapPtr->MapWidth, vector<mapObject>(WCR.MapPtr->MapHeight));
			for (int i = 0; i < mapSizeX; i++)
				for (int ii = 0; ii < mapSizeY; ii++)
				{
					sf::Int32 objectT, tileID, tileSID;
					recievedata >> objectT >> tileID >> tileSID;
					WCR.MapPtr->MapMatrix[i][ii].objectType = objectT;
					WCR.MapPtr->MapMatrix[i][ii].tileID = tileID;
					WCR.MapPtr->MapMatrix[i][ii].tileSetID = tileSID;
				}
			WCR.MapPtr->setupBorders();
			cout << "Loaded map from server!" << endl;
			break;
		case 1://Receive block change
			   //(int x, int y, int ID, int TID, int TSID)
			sf::Int32 x, y, ID, TID, TSID;
			recievedata >> x >> y >> ID >> TID >> TSID;
			WCR.MapPtr->MapMatrix[x][y].objectType = ID;
			WCR.MapPtr->MapMatrix[x][y].tileID = TID;
			WCR.MapPtr->MapMatrix[x][y].tileSetID = TSID;
			break;
		case 2://Other Player
			sf::Int32 oPlayerMessage;
			recievedata >> oPlayerMessage;
			{
				switch (oPlayerMessage)
				{
				case 0://create new player
				{
					sf::Int32 OPID, OPX, OPY;
					recievedata >> OPID >> OPX >> OPY;
					cout << "Player created: " << OPID << endl;
					if (WCR.otherPlayers[OPID] == nullptr)
					{
						WCR.otherPlayers[OPID] = new otherPlayer(WCR);
						WCR.otherPlayers[OPID]->x = OPX;
						WCR.otherPlayers[OPID]->y = OPY;
					}
					break;
				}
				case 1://remove player
				{
					sf::Int32 OPID;
					recievedata >> OPID;
					if (WCR.otherPlayers[OPID] != nullptr)
					{
						delete WCR.otherPlayers[OPID];
						WCR.otherPlayers[OPID] = nullptr;
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
					case 0://Jump
					{
						sf::Int32 OPID, x_, y_;
						recievedata >> OPID;
						//cout << OPID << "_" << WCR.otherPlayers.size() << endl;
						if (WCR.otherPlayers[OPID] != nullptr)
						{
							WCR.otherPlayers[OPID]->vspeed = -10;
							WCR.otherPlayers[OPID]->falling = true;
							WCR.otherPlayers[OPID]->PlayerImage.setPosition(WCR.otherPlayers[OPID]->x, WCR.otherPlayers[OPID]->y);
						}
						break;
					}
					case 1://Change x dir.
					{
						sf::Int32 OPID, x_, y_, xdirnew;
						recievedata >> OPID;
						//cout << OPID << "_" << WCR.otherPlayers.size() << endl;
						if (WCR.otherPlayers[OPID] != nullptr)
						{
							recievedata >> xdirnew >> x_ >> y_;
							WCR.otherPlayers[OPID]->x = x_;
							WCR.otherPlayers[OPID]->y = y_;
							WCR.otherPlayers[OPID]->xdir_ = xdirnew;
							WCR.otherPlayers[OPID]->PlayerImage.setPosition(WCR.otherPlayers[OPID]->x, WCR.otherPlayers[OPID]->y);
						}
						break;
					}
					case 2://Update x, y
					{
						sf::Int32 OPID, x_, y_;
						recievedata >> OPID;
						//cout << OPID << "_" << WCR.otherPlayers.size() << endl;
						if (WCR.otherPlayers[OPID] != nullptr)
						{
							recievedata >> x_ >> y_;
							WCR.otherPlayers[OPID]->x = x_;
							WCR.otherPlayers[OPID]->y = y_;
							WCR.otherPlayers[OPID]->PlayerImage.setPosition(WCR.otherPlayers[OPID]->x, WCR.otherPlayers[OPID]->y);
						}
						break;
					}
					}
					break;
				}
				case 3://Change player colour
				{
					sf::Int32 OPID, pr_, pg_, pb_;
					recievedata >> OPID >> pr_ >> pg_ >> pb_;
					if (WCR.otherPlayers[OPID] != nullptr)
					{
						WCR.otherPlayers[OPID]->MyCol = sf::Color(pr_, pg_, pb_, 255);
						WCR.otherPlayers[OPID]->PlayerImage.setColor(WCR.otherPlayers[OPID]->MyCol);
					}
					break;
				}
				case 4://Change my colour
				{
					sf::Int32 pr_, pg_, pb_;
					recievedata >> pr_ >> pg_ >> pb_;
					WCR.PlrPtr->MyCol = sf::Color(pr_, pg_, pb_, 255);
					WCR.PlrPtr->PlayerImage.setColor(WCR.PlrPtr->MyCol);
					break;
				}
				}
			}
			break;
		}
	}
	if (sockstat == sf::Socket::Disconnected)
	{
		for (int i = 0; i < WCR.otherPlayers.size(); i++)
		{
			if (WCR.otherPlayers[i] != nullptr)
			{
				delete WCR.otherPlayers[i];
				WCR.otherPlayers[i] = nullptr;
			}
		}
		WCR.online = false;
		WCR.socket.disconnect();
	}
}
