#include "Main.h"
/*
TODO: 
	- Clear on client.
	- Fix headers, do not include .h files in headers (or include one which only has predefinitions)
	- Add connections and listening to header.
	- Commands on server
	- Function to check player ID to see if it is within acceptable player id limits and that it is not a nullptr.
	- Animation for players
	- Allow other players to have permissions to edit the world, by command, or just give everyone permission with a tickbox (sends permission when they login or on command)
	- Add precise option for server to make pixel perfect blocks
	- Save pixel perfect blocks to map
	- Save blocks to map instead of whole array.
	- Send blocks to players instead of whole array.
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
		if (!WC.online)
		{
			sf::Socket::Status status = WC.socket.connect(WC.ConnectIp, 53000);
			if (status == sf::Socket::Done)
			{
				WC.online = true;
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

		window.clear(sf::Color(98,195,227));
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