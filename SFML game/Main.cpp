#include "Main.h"
/*
TODO: 
	- Fix headers, do not include .h files in headers (or include one which only has predefinitions)
	- Add connections and listening to header.
	- Function to check player ID to see if it is within acceptable player id limits and that it is not a nullptr.
	- Save blocks to map instead of whole array.
	- Send blocks to players instead of whole array.
	- Clearing should not send all data again, but just ask the client to clear their screen.
	- Colour picker for map when in server.
	- When pressing 'm' to go into the map mode, you currently have to first click on the map editor then on the map to start editing, it must think that the mouse isn't ready to place for some reason.
	- Send map resize.
	- Add map resize to GUI. (Map Settings Window)
	- Ability to add layers of backgrounds with GUI.
	- Backgrounds should draw
	*/

void WrapperClass::LimitVariable(int Min, int Max, int& Var)
{
	if (Var < Min)
		Var = Min;
	if (Var > Max)
		Var = Max;
}

WrapperClass::WrapperClass(sf::RenderWindow &RenderRef_) : RenderRef(RenderRef_), MHandle(*this)
{
	MainFont.loadFromFile("Data\\Fonts\\ARLRDBD.TTF");
	otherPlayers = vector<otherPlayer*>(256, nullptr);
	curmapID = 0;

	
	connected = false;
	mapMakerPermissions = true;
#ifdef MapMakerMode//start server.
	inMapMaker = true;
	gameType= ET_Server;
#else
	inMapMaker = false;
	gameType = ET_Client;
	mapMakerPermissions = false;//Need to receive from server.
#endif
}

int main()
{
	//Setup window and window settings.
	sf::RenderWindow window;	
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
	{
		char tempChar[30];
		cout << "Please enter your username (Max of 30 characters): " << endl;
		cin.getline(tempChar, 30);
		obj_Player.ChangeName(tempChar);
	}
	if (!MapMkr.LoadMap(WC.curmapID))
		cout << "Map load failed!" << endl;
#endif
	WC.MMPtr = &MapMkr;
	//#endif

	window.create(sf::VideoMode(640, 480), "Unnamed project");
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);

#ifdef MapMakerMode//server stuff
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
	WC.ConnectIp = new char[16];
	fstream game; game.open("Connection.txt", ios_base::in);
	strcpy(WC.ConnectIp, "127.0.0.1");

	if (game.is_open())
	{
		game.getline(WC.ConnectIp, 16);
	}

	cout << "Attempting to connect to server with address: " << WC.ConnectIp << endl;
	WC.socket.setBlocking(false);
#endif
	

	//Start the game loop.
	while (window.isOpen())
	{
#ifdef MapMakerMode//start server.
		WC.MHandle.ServerMessagesHandle();
#else
		if (!WC.connected)
		{
			sf::Socket::Status status = WC.socket.connect(WC.ConnectIp, 53000);
			if (status == sf::Socket::Done)
			{
				WC.connected = true;
				cout << "Connection to server successful!" << endl;

				sf::Packet namePacket;
				sf::String nameSf = obj_Player.myName;
				namePacket << (sf::Int32)2 << (sf::Int32)3 << nameSf;
				WC.MHandle.sendData(namePacket, WC.socket);
			}
		}
		else
			WC.MHandle.ClientMessagesHandle();
#endif

		while (window.pollEvent(WC.event))
		{
			if (WC.event.type == sf::Event::Closed)
				window.close();
			if (WC.event.type == sf::Event::Resized)
			{
				MapMkr.MapMakrView.setSize(window.getSize().x, window.getSize().y);
				obj_Player.PlayerView.setSize(window.getSize().x, window.getSize().y);
				window.setSize(sf::Vector2u(floorf(window.getSize().x/2)*2, floorf(window.getSize().y / 2) * 2));
			}
		}
		//Get player controls.
		if (!WC.inMapMaker)//Don't poll player events if in mapmaker mode.
		{
			obj_Player.StepPlayer();
			window.setView(obj_Player.PlayerView);
		}
		else
		{
			MapMkr.Step();
			window.setView(MapMkr.MapMakrView);
		}
		for (int i = 0; i < WC.otherPlayers.size(); i++)
			if (WC.otherPlayers[i] != nullptr)
				WC.otherPlayers[i]->step();
		//Clear the window so we can draw new stuff to it.

		window.clear(sf::Color(98,195,227));
		MainMap.Drawbackground(obj_Player.PlayerView, MainMap.E_vBackground);
		MainMap.DrawMap(obj_Player.PlayerView);
		MainMap.drawBorders();
		if(WC.inMapMaker)
			MapMkr.Draw();
		MainMap.Drawbackground(obj_Player.PlayerView, MainMap.E_vMiddleground);
		for (int i = 0; i < WC.otherPlayers.size(); i++)
			if (WC.otherPlayers[i] != nullptr)
				WC.otherPlayers[i]->draw();
		obj_Player.DrawPlayer();
		MainMap.Drawbackground(obj_Player.PlayerView, MainMap.E_vForeground);
		//Display the window to theclient.
		window.display();
	}

	return 0;
}