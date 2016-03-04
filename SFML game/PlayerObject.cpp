#include "Main.h"

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
	BoundBox.width = 46;
	BoundBox.height = 35;
	vspeedMax = 16;
	SPD = 5.f;
	falling = true;
	MyCol = sf::Color::Blue;
	gravity = 0.5;//acceleration on vspeed
	vspeed = 0;
	xDirOld = 0;
	pressWOld = false;
	//PlayerImage.setSize(sf::Vector2f(32,32));
	PlayerImage.setPosition(x, y);
}

void PlayerObject::ContractDir(Edirection DIrr)
{
	int Xdir = 0;
	int Ydir = 0;
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
	AfterImage.emplace_back(PlayerImage);
	//AfterImage[AfterImage.size() - 1].setPosition(x, y);
	//after image step
	int AlphaDec = 15;
	for (int i = 0; i < AfterImage.size(); i++)
	{
		if (AfterImage[i].getColor().a - AlphaDec <= 0)
			AfterImage.erase(AfterImage.begin() + i);
		AfterImage[i].setColor(sf::Color(MyCol.r, MyCol.g, MyCol.b, AfterImage[i].getColor().a - AlphaDec));
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
			if (WCR.MapPtr->CheckCollision(BoundBox, x, y + vspeed, 1) == 1 || WCR.MapPtr->CheckCollision(BoundBox, x, y + vspeed, 0) == 1)
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
		if (vspeed + gravity > vspeedMax)
			vspeed = vspeedMax;
		else
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
		sendPos();
		PlayerImage.setPosition(x, y);
	}
	if (WCR.MapPtr->CheckCollision(BoundBox, x, y, 4) == 4)//Next Level Block
	{
		WCR.curmapID++;//next map
		ResetMovement();
		if (!WCR.MMPtr->LoadMap(WCR.curmapID))
			cout << "Map load failed!" << endl;
	}

	PlayerView.setCenter(sf::Vector2f((int)x + 16, (int)y + 16));
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
	PlayerImage.setPosition(x, y);
}

void PlayerObject::sendJump()
{
	if (!WCR.online)
		return;
	sf::Packet sendData;
	sendData << (sf::Int32)2 << (sf::Int32)0 << (sf::Int32)x << (sf::Int32)y;
	WCR.MHandle.sendData(sendData, WCR.socket);
}

void PlayerObject::sendPos()
{
	if (!WCR.online)
		return;
	sf::Packet sendData;
	sendData << (sf::Int32)2 << (sf::Int32)2 << (sf::Int32)x << (sf::Int32)y;
	WCR.MHandle.sendData(sendData, WCR.socket);
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
		WCR.MHandle.sendData(sendData, WCR.socket);
	}
}

void PlayerObject::DrawPlayer() {

	for (int i = 0; i < AfterImage.size(); i++)
		WCR.RenderRef.draw(AfterImage[i]);
	WCR.RenderRef.draw(PlayerImage);
}