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

		if (Stat == sf::Socket::Disconnected || Stat == sf::Socket::Error)
			break;
		else if (Stat != sf::Socket::Done)
			continue;//keep going, not done.
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
		mapData << (sf::Int16)WCR.MapPtr->MapWidth << (sf::Int16)WCR.MapPtr->MapHeight << WCR.PlrPtr->x << WCR.PlrPtr->y;
		int count_ = 0;
		for (int i = 0; i < WCR.MapPtr->MapWidth; i++)
			for (int ii = 0; ii < WCR.MapPtr->MapHeight; ii++)
				if (WCR.MapPtr->MapMatrix[i][ii].tileID != -1)
					count_++;
		mapData << (sf::Int16)count_;
		for (int i = 0; i < WCR.MapPtr->MapWidth; i++)
			for (int ii = 0; ii < WCR.MapPtr->MapHeight; ii++)
				if (WCR.MapPtr->MapMatrix[i][ii].tileID != -1)
					mapData << (sf::Int16)i << (sf::Int16)ii << (sf::Int8)WCR.MapPtr->MapMatrix[i][ii].objectType << (sf::Int16)WCR.MapPtr->MapMatrix[i][ii].tileID << (sf::Int8)WCR.MapPtr->MapMatrix[i][ii].tileSetID << (sf::Int8)WCR.MapPtr->MapMatrix[i][ii].pixelPerfect;

		mapData << (sf::Int8)WCR.MapPtr->BackgroundMatrix.size();

		for (int i = 0; i < WCR.MapPtr->BackgroundMatrix.size(); i++)
		{
			mapData << (sf::Int32)WCR.MapPtr->BackgroundLayers[i];

			count_ = 0;
			for (int ii = 0; ii < WCR.MapPtr->MapWidth; ii++)
				for (int iii = 0; iii < WCR.MapPtr->MapHeight; iii++)
					if (WCR.MapPtr->BackgroundMatrix[i][ii][iii].tileID != -1)
						count_++;
			mapData << (sf::Int16)count_;
			for (int ii = 0; ii < WCR.MapPtr->MapWidth; ii++)
				for (int iii = 0; iii < WCR.MapPtr->MapHeight; iii++)
					if (WCR.MapPtr->BackgroundMatrix[i][ii][iii].tileID != -1)
						mapData << (sf::Int16)ii << (sf::Int16)iii << (sf::Int16)WCR.MapPtr->BackgroundMatrix[i][ii][iii].tileID << (sf::Int8)WCR.MapPtr->BackgroundMatrix[i][ii][iii].tileSetID;
		}

		sendData(mapData, foundEmpty);
		//cout << "Sent map data!" << endl;

		//create otherplayer object for server.
		if (WCR.otherPlayers[foundEmpty] == nullptr)
		{
			WCR.otherPlayers[foundEmpty] = new otherPlayer(WCR);
			WCR.otherPlayers[foundEmpty]->MyCol = sf::Color(rand()%256, 50 + rand() % 196, 50 + rand() % 196, 255);
			WCR.otherPlayers[foundEmpty]->PlayerImage.setColor(WCR.otherPlayers[foundEmpty]->MyCol);
			WCR.otherPlayers[foundEmpty]->PlayerImage.setPosition(WCR.PlrPtr->x, WCR.PlrPtr->y);
			WCR.otherPlayers[foundEmpty]->PID = foundEmpty;
			WCR.otherPlayers[foundEmpty]->x = WCR.PlrPtr->x;
			WCR.otherPlayers[foundEmpty]->y = WCR.PlrPtr->y;
			WCR.otherPlayers[foundEmpty]->PlayerAnimation.setColor(WCR.otherPlayers[foundEmpty]->MyCol);
		}
		sf::Packet ColourData;
		ColourData << (sf::Int32)2 << (sf::Int32)4 << (sf::Int32)WCR.otherPlayers[foundEmpty]->MyCol.r << (sf::Int32)WCR.otherPlayers[foundEmpty]->MyCol.g << (sf::Int32)WCR.otherPlayers[foundEmpty]->MyCol.b;
		sendData(ColourData, foundEmpty);

		if (WCR.MMPtr->GiveAllPermissions->IsActive())//If giveallperms tick is active, give everyone permission to edit map.
		{
			sf::Packet PermSend;
			PermSend << (sf::Int32)2 << (sf::Int32)6 << (sf::Int32)0 << (sf::Int32)1;
			sendData(PermSend, foundEmpty);
			WCR.otherPlayers[foundEmpty]->MapEditPermissions = 1;
		}

		//Send to other players
		ColourData.clear();
		mapData.clear();
		mapData << (sf::Int32)2 << (sf::Int32)0 << (sf::Int32)foundEmpty << WCR.otherPlayers[foundEmpty]->x << WCR.otherPlayers[foundEmpty]->y;
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
			mapData << (sf::Int32)2 << (sf::Int32)0 << (sf::Int32)i << WCR.otherPlayers[i]->x << WCR.otherPlayers[i]->y;
			sendData(mapData, foundEmpty);
			sf::Packet NameData;
			NameData << (sf::Int32)2 << (sf::Int32)5 << (sf::Int32)i << sf::String(WCR.otherPlayers[i]->myName);
			sendData(NameData, foundEmpty);
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
					sf::Int32 hspeed_, vspeed_;
					float x_, y_;
					recievedata >> x_ >> y_;
					for (int ii = 0; ii < WCR.clients.size(); ii++)
					{
						if (ii == i || WCR.clients[ii] == nullptr)
							continue;
						sf::Packet Data_;
						Data_ << (sf::Int32)2 << (sf::Int32)2 << (sf::Int32)0 << (sf::Int32)i << x_ << y_;
						sendData(Data_, ii);
					}
					//jump otherplayer object
					WCR.otherPlayers[i]->xAct += x_ - WCR.otherPlayers[i]->x;
					WCR.otherPlayers[i]->yAct += y_ - WCR.otherPlayers[i]->y;
					WCR.otherPlayers[i]->x = x_;
					WCR.otherPlayers[i]->y = y_;
					WCR.otherPlayers[i]->vspeed = -10;
					WCR.otherPlayers[i]->falling = true;
					//WCR.otherPlayers[i]->PlayerImage.setPosition(WCR.otherPlayers[i]->x, WCR.otherPlayers[i]->y);
					break;
				}
				case 1://xdir change
				{
					sf::Int32 xDIr;
					float x_, y_, hspd_;
					recievedata >> xDIr >> x_ >> y_ >> hspd_;

					for (int ii = 0; ii < WCR.clients.size(); ii++)
					{
						if (ii == i || WCR.clients[ii] == nullptr)
							continue;
						sf::Packet Data_;
						Data_ << (sf::Int32)2 << (sf::Int32)2 << (sf::Int32)1 << (sf::Int32)i << xDIr << x_ << y_ << hspd_;
						sendData(Data_, ii);
					}
					//Move otherplayer object.
					//cout << "Got pos: (" << x_ << ", " << y_ << ")" << endl;
					WCR.otherPlayers[i]->xAct += x_ - WCR.otherPlayers[i]->x;
					WCR.otherPlayers[i]->yAct += y_ - WCR.otherPlayers[i]->y;
					WCR.otherPlayers[i]->x = x_;
					WCR.otherPlayers[i]->y = y_;
					WCR.otherPlayers[i]->hspeed = hspd_;
					WCR.otherPlayers[i]->xdir_ = xDIr;
					//WCR.otherPlayers[i]->PlayerImage.setPosition(WCR.otherPlayers[i]->x, WCR.otherPlayers[i]->y);
					break;
				}
				case 2://pos change
				{
					float x_, y_, hspeed_;
					recievedata >> x_ >> y_ >> hspeed_;
					for (int ii = 0; ii < WCR.clients.size(); ii++)
					{
						if (ii == i || WCR.clients[ii] == nullptr)
							continue;
						sf::Packet Data_;
						Data_ << (sf::Int32)2 << (sf::Int32)2 << (sf::Int32)2 << (sf::Int32)i << x_ << y_ << hspeed_;
						sendData(Data_, ii);
						continue;//no point
					}
					//Move otherplayer object.
					WCR.otherPlayers[i]->x = x_;
					WCR.otherPlayers[i]->y = y_;
					WCR.otherPlayers[i]->xAct = 0;
					WCR.otherPlayers[i]->yAct = 0;
					//WCR.otherPlayers[i]->PlayerImage.setPosition(WCR.otherPlayers[i]->x, WCR.otherPlayers[i]->y);
					break;
				}
				case 3://name change (set)
				{
					sf::String OName_;
					recievedata >> OName_;
					for (int ii = 0; ii < WCR.clients.size(); ii++)
					{
						if (ii == i || WCR.clients[ii] == nullptr)
							continue;
						sf::Packet Data_;
						Data_ << (sf::Int32)2 << (sf::Int32)5 << (sf::Int32)i << OName_;
						sendData(Data_, ii);
						continue;//no point
					}
					//Set Oplayer name.
					WCR.otherPlayers[i]->ChangeName(OName_.toAnsiString().c_str());
					break;
				}
				case 4://Client places block
				{
					if (WCR.otherPlayers[i]->MapEditPermissions)
					{
						sf::Int32 x_, y_, ID_, TID_, TSID_, PP_;
						//mapData << (sf::Int32)2 << (sf::Int32)4 << (sf::Int32)x << (sf::Int32)y << (sf::Int32)ID << (sf::Int32)TID << (sf::Int32)TSID << (sf::Int32)PP;
						recievedata >> x_ >> y_ >> ID_ >> TID_ >> TSID_ >> PP_;
						//cout << x_ << "_" << y_ << "_" << ID_ << "_" << TID_ << "_" << TSID_ << "_" << PP_;
						WCR.MapPtr->SetObject(x_, y_, ID_, TID_, TSID_, PP_, i,true);
					}
					break;
				}
				case 5://Client places background
				{
					if (WCR.otherPlayers[i]->MapEditPermissions)
					{
						sf::Int16 x_, y_, TID_;
						sf::Int8 LID_, TSID_;
						//mapData << (sf::Int32)2 << (sf::Int32)4 << (sf::Int32)x << (sf::Int32)y << (sf::Int32)ID << (sf::Int32)TID << (sf::Int32)TSID << (sf::Int32)PP;
						recievedata >> x_ >> y_ >> LID_ >> TID_ >> TSID_;
						//cout << x_ << "_" << y_ << "_" << ID_ << "_" << TID_ << "_" << TSID_ << "_" << PP_;
						WCR.MapPtr->SetBG(x_, y_, TID_, TSID_, LID_, i, true);
					}
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
	if (ID<0 || ID>=WCR.clients.size())
		return;//ID not in range of sockets.
	//Hopefully there wont be any otherplayers created without sockets otherwise this could break.
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
	if (WCR.clients[ID] != nullptr)
	{
		//Disconnect just in case (If not connected wont do anything!)
		WCR.clients[ID]->disconnect();
		delete WCR.clients[ID];
		WCR.clients[ID] = nullptr;
	}
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
		{
			sf::Int16 mapSizeX, mapSizeY;
			float Px_, Py_;
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
			sf::Int16 Blockss_;
			recievedata >> Blockss_;
			for (int i = 0; i < Blockss_; i++)
			{
				sf::Int16 BlockX, BlockY, tileID;
				sf::Int8 objectT, TPP, tileSID;
				recievedata >> BlockX >> BlockY >> objectT >> tileID >> tileSID >> TPP;
				//bool TPP;
				WCR.MapPtr->MapMatrix[BlockX][BlockY].objectType = objectT;
				WCR.MapPtr->MapMatrix[BlockX][BlockY].tileID = tileID;
				WCR.MapPtr->MapMatrix[BlockX][BlockY].tileSetID = tileSID;
				WCR.MapPtr->MapMatrix[BlockX][BlockY].pixelPerfect = TPP;
			}

			sf::Int8 Layers;
			recievedata >> Layers;
			WCR.MapPtr->BackgroundMatrix = vector<vector<vector<mapObject>>>(Layers, vector<vector<mapObject>>(WCR.MapPtr->MapWidth, vector<mapObject>(WCR.MapPtr->MapHeight)));
			WCR.MapPtr->BackgroundLayers.clear();
			WCR.MapPtr->OrderedBackgroundLayers.clear();
			WCR.MMPtr->LayerList->Clear();

			for (int i = 0; i < Layers; i++)
			{
				sf::Int32 LayerId_;
				recievedata >> LayerId_;

				WCR.MapPtr->BackgroundLayers.push_back(LayerId_);
				stringstream BacktoStr_;
				BacktoStr_ << LayerId_;
				bool foundSpot = false;
				for (int ii = 0; ii < i; ii++)
					if (LayerId_ > WCR.MapPtr->BackgroundLayers[WCR.MapPtr->OrderedBackgroundLayers[ii]])
					{
						WCR.MapPtr->OrderedBackgroundLayers.insert(WCR.MapPtr->OrderedBackgroundLayers.begin() + ii, i);
						foundSpot = true;
						WCR.MMPtr->LayerList->InsertItem(ii, BacktoStr_.str());
						break;
					}
				if (!foundSpot)
				{
					WCR.MMPtr->LayerList->AppendItem(BacktoStr_.str());
					WCR.MapPtr->OrderedBackgroundLayers.push_back(i);
				}

				sf::Int16 Blocks_;
				recievedata >> Blocks_;
				for (int ii = 0; ii < Blocks_; ii++)
				{
					sf::Int16 BlockX, BlockY, TID;
					sf::Int8 TSID;
					recievedata >> BlockX >> BlockY >> TID >> TSID;
					cout << "_" << BlockX << "_" << BlockY << "_" << TID << "_" << TSID << endl;
					WCR.MapPtr->BackgroundMatrix[i][BlockX][BlockY].tileID = TID;
					WCR.MapPtr->BackgroundMatrix[i][BlockX][BlockY].tileSetID = TSID;
				}
			}
			WCR.MMPtr->LayerList->SelectItem(0);
			WCR.MapPtr->setupBorders();
			WCR.MMPtr->curTileLayer = WCR.MapPtr->OrderedBackgroundLayers[0];
			cout << "Loaded map from server!" << endl;
			WCR.PlrPtr->sendXChange();//When loading new map, send new position.
			break;
		}
		case 1://Receive block change
			   //(int x, int y, int ID, int TID, int TSID)
		{
			sf::Int32 x, y, ID, TID, TSID;
			bool TPP;
			recievedata >> x >> y >> ID >> TID >> TSID >> TPP;
			WCR.MapPtr->MapMatrix[x][y].objectType = ID;
			WCR.MapPtr->MapMatrix[x][y].tileID = TID;
			WCR.MapPtr->MapMatrix[x][y].tileSetID = TSID;
			WCR.MapPtr->MapMatrix[x][y].pixelPerfect = TPP;
			break;
		}
		case 2://Players
			sf::Int32 oPlayerMessage;
			recievedata >> oPlayerMessage;
			{
				switch (oPlayerMessage)
				{
				case 0://create new player
				{
					sf::Int32 OPID;
					float x_, y_;
					recievedata >> OPID >> x_ >> y_;
					cout << "Player created: " << OPID << endl;
					if (WCR.otherPlayers[OPID] != nullptr)
					{
						//Delete because already created...
						delete WCR.otherPlayers[OPID];
						WCR.otherPlayers[OPID] = nullptr;
					}
					WCR.otherPlayers[OPID] = new otherPlayer(WCR);
					WCR.otherPlayers[OPID]->xAct = 0;
					WCR.otherPlayers[OPID]->xAct = 0;
					WCR.otherPlayers[OPID]->x = x_;
					WCR.otherPlayers[OPID]->y = y_;
					break;
				}
				case 1://remove player
				{
					sf::Int32 OPID;
					recievedata >> OPID;
					if (WCR.otherPlayers[OPID] == nullptr)
						break;
					delete WCR.otherPlayers[OPID];
					WCR.otherPlayers[OPID] = nullptr;
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
						sf::Int32 OPID;
						float x_, y_;
						recievedata >> OPID >> x_ >> y_;
						if (WCR.otherPlayers[OPID] == nullptr)
							break;
						WCR.otherPlayers[OPID]->vspeed = -10;
						WCR.otherPlayers[OPID]->falling = true;
						WCR.otherPlayers[OPID]->xAct += x_ - WCR.otherPlayers[OPID]->x;
						WCR.otherPlayers[OPID]->yAct += y_ - WCR.otherPlayers[OPID]->y;
						WCR.otherPlayers[OPID]->x = x_;
						WCR.otherPlayers[OPID]->y = y_;
						//WCR.otherPlayers[OPID]->PlayerImage.setPosition(WCR.otherPlayers[OPID]->x, WCR.otherPlayers[OPID]->y);
						break;
					}
					case 1://Change x dir.
					{
						sf::Int32 OPID, xdirnew;
						float x_, y_;
						recievedata >> OPID;
						if (WCR.otherPlayers[OPID] == nullptr)
							break;
						recievedata >> xdirnew >> x_ >> y_;
						WCR.otherPlayers[OPID]->xAct += x_ - WCR.otherPlayers[OPID]->x;
						WCR.otherPlayers[OPID]->yAct += y_ - WCR.otherPlayers[OPID]->y;
						WCR.otherPlayers[OPID]->x = x_;
						WCR.otherPlayers[OPID]->y = y_;
						WCR.otherPlayers[OPID]->xdir_ = xdirnew;
						//WCR.otherPlayers[OPID]->PlayerImage.setPosition(WCR.otherPlayers[OPID]->x, WCR.otherPlayers[OPID]->y);
						break;
					}
					case 2://Update x, y
					{
						sf::Int32 OPID;
						float x_, y_, hspd_;
						recievedata >> OPID;
						if (WCR.otherPlayers[OPID] == nullptr)
							break;
						//cout << x_ << "" << y_ << endl;
						recievedata >> x_ >> y_ >> hspd_;
						WCR.otherPlayers[OPID]->xAct = 0;
						WCR.otherPlayers[OPID]->yAct = 0;
						WCR.otherPlayers[OPID]->x = x_;
						WCR.otherPlayers[OPID]->y = y_;
						WCR.otherPlayers[OPID]->hspeed = hspd_;
						//WCR.otherPlayers[OPID]->PlayerImage.setPosition(WCR.otherPlayers[OPID]->x, WCR.otherPlayers[OPID]->y);
						break;
					}
					}
					break;
				}
				case 3://Change player colour
				{
					sf::Int32 OPID, pr_, pg_, pb_;
					recievedata >> OPID >> pr_ >> pg_ >> pb_;
					if (WCR.otherPlayers[OPID] == nullptr)
						break;
					WCR.otherPlayers[OPID]->MyCol = sf::Color(pr_, pg_, pb_, 255);
					WCR.otherPlayers[OPID]->PlayerImage.setColor(WCR.otherPlayers[OPID]->MyCol);
					WCR.otherPlayers[OPID]->PlayerAnimation.setColor(WCR.otherPlayers[OPID]->MyCol);
					break;
				}
				case 4://Change my colour
				{
					sf::Int32 pr_, pg_, pb_;
					recievedata >> pr_ >> pg_ >> pb_;
					WCR.PlrPtr->MyCol = sf::Color(pr_, pg_, pb_, 255);
					WCR.PlrPtr->PlayerImage.setColor(WCR.PlrPtr->MyCol);
					WCR.PlrPtr->PlayerAnimation.setColor(WCR.PlrPtr->MyCol);
					break;
				}
				case 5://name change (set)
				{
					sf::Int32 ID;
					sf::String OName_;
					recievedata >> ID >> OName_;
					if (WCR.otherPlayers[ID] == nullptr)
						break;
					WCR.otherPlayers[ID]->ChangeName(OName_.toAnsiString().c_str());
					break;
				}
				case 6://Permission Change
				{
					sf::Int32 PID, PVal;

					recievedata >> PID >> PVal;
					switch (PID)
					{
					case 0://Map Editor Permissions
						WCR.mapMakerPermissions = PVal;

						//Check if map editor is open, if it is, force it closed.
						if (WCR.inMapMaker)
						{
							WCR.inMapMaker = false;
							WCR.MMPtr->mmWindow.close();
						}
						break;
					}
					break;
				}
				case 7://Other player attack
				{
					sf::Int32 PID;//Get player ID
					recievedata >> PID;
				}
				}
			}
			break;
			case 3://Background Change
			{
				sf::Int16 x_, y_, TID_;
				sf::Int8 LID_, TSID_;
				recievedata >> x_ >> y_ >> LID_ >> TID_ >> TSID_;
				WCR.MapPtr->BackgroundMatrix[LID_][x_][y_].tileID = TID_;
				WCR.MapPtr->BackgroundMatrix[LID_][x_][y_].tileSetID = TSID_;
				break;
			}
			case 4://Layer stuff
			{
				sf::Int32 LayerMsg;
				recievedata >> LayerMsg;
				switch (LayerMsg)
				{
				case 0://Layer add
				{
					sf::Int32 LID_;
					recievedata >> LID_;
					int Index_ = WCR.MapPtr->addLayer(LID_);
					std::stringstream backtoStr;
					backtoStr << LID_;
					WCR.MMPtr->LayerList->InsertItem(Index_, backtoStr.str());
					break;
					//NewLayerEntry
				}
				case 1://Layer clear
					sf::Int8 LID_;
					recievedata >> LID_;
					WCR.MapPtr->BackgroundMatrix[WCR.MapPtr->OrderedBackgroundLayers[LID_]] = vector<vector<mapObject>>(WCR.MapPtr->MapWidth, vector<mapObject>(WCR.MapPtr->MapHeight)); break;
				}

			}
			case 5://Clear Map
				WCR.MapPtr->resetMap();
				break;
			case 6://Expand map
				sf::Int16 NewW_, NewH_;
				recievedata >> NewW_ >> NewH_;
				WCR.MapPtr->expandMap(NewW_, NewH_);
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
		WCR.connected = false;
		WCR.socket.disconnect();//May be pointless, as already disconnected, hopefully doesn't create double delete error...
		cout << "Disconnected from the server!" << endl;
	}
}
