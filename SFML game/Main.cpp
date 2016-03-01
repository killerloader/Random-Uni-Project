#include "Main.h"
/*
TODO: 
	- Add 'OtherPlayer' class to draw players online.
	- Accept more than one connection with a vector of sockets.

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

}

PlayerObject::PlayerObject(WrapperClass &WCR_) : WCR(WCR_) {
	PlayerTex.loadFromFile("Data/Sprites/Player.png");
	PlayerImage.setTexture(PlayerTex);
	PlayerView.setSize(sf::Vector2f(1280, 960));
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
	if(WCR.RenderRef.hasFocus())
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
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		//vspeed, hspeed, gravity, haccel, hspeedmax;
		if (hspeed - haccel >= -hspeedmax)
			hspeed -= haccel;
		else
			hspeed = -hspeedmax;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
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

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		if (WCR.MapPtr->CheckCollision(BoundBox, x, y + 1, 1))
		{
			vspeed = -10;
			falling = true;
		}
}

void PlayerObject::MovePlayer(float Xmove, float Ymove)
{
	x += Xmove;
	y += Ymove;
	PlayerImage.setPosition(x,y);
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
	sf::RenderWindow window(sf::VideoMode(1280, 960), "Unnamed project");
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
	if (MapMkr.LoadMap(WC.curmapID))
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
		cout << "Port bind successful!" << endl;

	// accept a new connection
	
	WC.client.setBlocking(false);
	cout << "Listening for connection..." << endl;
#else
	bool online = false;
	cout << "Attempting to connect to server..." << endl;
	sf::TcpSocket socket; socket.setBlocking(false);
#endif
	

	//Start the game loop.
	while (window.isOpen())
	{
#ifdef MapMakerMode//start server.
		if (!WC.connected)
		{
			if (WC.listener.accept(WC.client) == sf::Socket::Done)
			{
				cout << "Connection found!" << endl;
				WC.connected = true;

				//Send map:
				sf::Packet mapData;
				mapData << (sf::Int32)0;
				mapData << (sf::Int32)WC.MapPtr->MapWidth << (sf::Int32)WC.MapPtr->MapHeight << (sf::Int32)WC.PlrPtr->x << (sf::Int32)WC.PlrPtr->y;

				for (int i = 0; i < WC.MapPtr->MapWidth; i++)
					for (int ii = 0; ii < WC.MapPtr->MapHeight; ii++)
						mapData << (sf::Int32)WC.MapPtr->MapMatrix[i][ii].objectType << (sf::Int32)WC.MapPtr->MapMatrix[i][ii].tileID << (sf::Int32)WC.MapPtr->MapMatrix[i][ii].tileSetID;
				WC.client.send(mapData);
				cout << "Sent map data!" << endl;
			}
		}
		else
		{
			sf::Packet recievedata;
			sf::Socket::Status Status_ = WC.client.receive(recievedata);
			if (Status_ == sf::Socket::Done)
			{
				sf::Int32 int32;
				recievedata >> int32;
				cout << "Received data: " << int32 << endl;
			}
			if (Status_ == sf::Socket::Disconnected)
			{
				cout << "Client has disconnected!" << endl;
				WC.connected = false;
			}
		}
		//Poll events, keyboard wont be placed in here.
#else
		if (!online)
		{
			sf::Socket::Status status = socket.connect("127.0.0.1", 53000);
			if (status == sf::Socket::Done)
			{
				online = true;
				cout << "Connection to server successful!" << endl;
			}
		}
		else
		{
			sf::Packet recievedata;
			if (socket.receive(recievedata) == sf::Socket::Done)
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
					WC.MapPtr->SetObject(x, y, ID, TID, TSID);
					break;
				}
			}
		}
#endif

		while (window.pollEvent(WC.event))
		{
			#ifdef MapMakerMode//Don't poll GUI events if in mapmaker mode.
				MapMkr.PollGUIEvents();
			#endif
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
		//Clear the window so we can draw new stuff to it.
		window.clear();
		MainMap.DrawMap(obj_Player.PlayerView);
		MainMap.drawBorders();
		obj_Player.DrawPlayer();
		#ifdef MapMakerMode//Don't poll player events if in mapmaker mode.
		MapMkr.Draw();
		#endif
		//Display the window to the client.
		window.display();
	}

	return 0;
}