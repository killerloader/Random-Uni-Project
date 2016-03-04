#include "otherPlayer.h"
#include "Maps.h"
#include "mapObject.h"
#include "main.h"


otherPlayer::otherPlayer(WrapperClass &WCR_) : WCR(WCR_)
{
	PlayerImage.setTexture(WCR.PlrPtr->PlayerTex);
	BoundBox.width = 46;
	BoundBox.height = 35;
	MyCol = sf::Color::White;
	PID = -1;
	vspeedMax = 16;
	myName = new char[5]{ "NULL" };
	hspeed = 0;
	haccel = 0.4;
	hspeedmax = 5;
	hfric = 0.3;//How fast you slow down if you release all keys
	SPD = 5.f;
	falling = true;
	gravity = 0.5;//acceleration on vspeed
	vspeed = 0;
	xdir_ = 0;
}

otherPlayer::~otherPlayer()
{

}

void otherPlayer::MovePlayer(float Xmove, float Ymove)
{
	x += Xmove;
	y += Ymove;
	PlayerImage.setPosition(x, y);
}

void otherPlayer::ContractDir(Edirection DIrr)
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

void otherPlayer::step()
{
//vspeed, hspeed, gravity, haccel, hspeedmax;

	AfterImage.emplace_back(PlayerImage);

	//AfterImage[AfterImage.size() - 1].setPosition(x, y);
	//AfterImage[AfterImage.size() - 1].setColor(MyCol);
	//AfterImage[AfterImage.size() - 1].setFillColor(sf::Color::White);
	//after image step
	int AlphaDec = 15;
	for (int i = 0; i < AfterImage.size(); i++)
	{
		if (AfterImage[i].getColor().a - AlphaDec <= 0)
			AfterImage.erase(AfterImage.begin() + i);
		AfterImage[i].setColor(sf::Color(AfterImage[i].getColor().r, AfterImage[i].getColor().g, AfterImage[i].getColor().b, AfterImage[i].getColor().a - AlphaDec));
	}

	if (xdir_ == -1)
	{
		//vspeed, hspeed, gravity, haccel, hspeedmax;
		if (hspeed - haccel >= -hspeedmax)
			hspeed -= haccel;
		else
			hspeed = -hspeedmax;
	}
	else if (xdir_ == 1)
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
}

void otherPlayer::draw()
{
	for (int i = 0; i < AfterImage.size(); i++)
		WCR.RenderRef.draw(AfterImage[i]);
	WCR.RenderRef.draw(PlayerImage);
}