#include "Main.h"

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
	PlayerImage.setSize(sf::Vector2f(32,32));
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
	AfterImage.emplace_back(sf::Vector2f(32,32));
	AfterImage[AfterImage.size() - 1].setPosition(x, y);
	AfterImage[AfterImage.size() - 1].setFillColor(sf::Color::White);
	//after image step
	int AlphaDec = 15;
	for (int i = 0; i < AfterImage.size(); i++)
	{
		if(AfterImage[i].getFillColor().a- AlphaDec <=0)
			AfterImage.erase(AfterImage.begin() + i);
		AfterImage[i].setFillColor(sf::Color(230, 230, 230,AfterImage[i].getFillColor().a- AlphaDec));
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
	
	PlayerView.setCenter(sf::Vector2f(x+16, y+16));
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

char* test()
{
	char test[] = "test";

	return test;
}

int main()
{
	cout << test() << endl;
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
	#if !MapMakerMode
	if (MapMkr.LoadMap(WC.curmapID))
		cout << "Map load failed!" << endl;
#endif
	WC.MMPtr = &MapMkr;
	//#endif

	//Give certain classes/ structs access to the window context.
	//Start the game loop.
	while (window.isOpen())
	{
		//Poll events, keyboard wont be placed in here.
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		//Get player controls.
		#if !MapMakerMode//Don't poll player events if in mapmaker mode.
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
		#if MapMakerMode//Don't poll player events if in mapmaker mode.
		MapMkr.Draw();
		#endif
		//Display the window to the client.
		window.display();
	}

	return 0;
}