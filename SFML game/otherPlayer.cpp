#include "main.h"

otherPlayer::otherPlayer(WrapperClass &WCR_) : WCR(WCR_), PlayerAnimation(WCR_)
{
	int Pw_(46), Ph_(35);
	PlayerImage.setTexture(WCR.PlrPtr->PlayerTex);
	//Setup animation:
	//----
	BoundBox.width = Pw_;
	BoundBox.height = Ph_;
	MyCol = sf::Color::White;
	PlayerAnimation.addAnimation(WCR.PlrPtr->PlayerAnimation.getTexture(0), 1, 1, 1, 3, "Left", 46, 35, 5);
	PlayerAnimation.addAnimation(WCR.PlrPtr->PlayerAnimation.getTexture(1), 1, 1, 1, 3, "Right", 46, 35, 5);
	PID = -1;
	vspeedMax = 16;
	myName = new char[5]{ "NULL" };
	NameText.setString(myName);
	NameText.setFont(WCR.MainFont);
	NameText.setCharacterSize(12);
	hspeed = 0;
	haccel = 0.4;
	haccelSlip = 0.2;
	hspeedmax = 5;
	hfric = 0.3;//How fast you slow down if you release all keys
	hfricSlip = 0.05;
	SPD = 5.f;
	falling = true;
	gravity = 0.5;//acceleration on vspeed
	vspeed = 0;
	xdir_ = 0;
	xAct = 0;
	yAct = 0;
	MapEditPermissions = false;
}

otherPlayer::~otherPlayer()
{

}

void otherPlayer::MovePlayer(float Xmove, float Ymove)
{
	x += Xmove;
	y += Ymove;
	//PlayerImage.setPosition(x - xAct, y - yAct);
	NameText.setPosition(round(x+PlayerImage.getLocalBounds().width/2-NameText.getLocalBounds().width/2), y - NameText.getLocalBounds().height - 8);
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

	while (!WCR.MapPtr->CheckCollision(BoundBox, x + Xdir, y + Ydir, 1) && !WCR.MapPtr->CheckCollision(BoundBox, x + Xdir, y + Ydir, 0))//only solid objects
		MovePlayer(Xdir, Ydir);
}

void otherPlayer::step()
{
//vspeed, hspeed, gravity, haccel, hspeedmax;

	AfterImage.emplace_back(PlayerAnimation.getCurrentSprite());

	//AfterImage[AfterImage.size() - 1].setPosition(x, y);
	//AfterImage[AfterImage.size() - 1].setColor(MyCol);
	//AfterImage[AfterImage.size() - 1].setFillColor(sf::Color::White);
	//after image step
	float frixUse = 0, HAUse = 0;
	if (WCR.MapPtr->CheckCollision(BoundBox, x, y + 1, 5) == 5)
	{
		frixUse = hfricSlip;
		HAUse = haccelSlip;
	}
	else
	{
		frixUse = hfric;
		HAUse = haccel;
	}
		
	int AlphaDec = 50;
	for (int i = 0; i < AfterImage.size(); i++)
	{
		if (AfterImage[i].getColor().a - AlphaDec <= 0)
			AfterImage.erase(AfterImage.begin() + i);
		AfterImage[i].setColor(sf::Color(AfterImage[i].getColor().r, AfterImage[i].getColor().g, AfterImage[i].getColor().b, AfterImage[i].getColor().a - AlphaDec));
	}

	if (xdir_ == -1)
	{
		if (PlayerAnimation.getCurrentAnimationID() != 0)//Not going left.
			PlayerAnimation.setAnimation(0);
		if (!PlayerAnimation.getIsPlaying())
			PlayerAnimation.setPlaying(true);
		if (hspeed - HAUse >= -hspeedmax)
			hspeed -= HAUse;
		else
			hspeed = -hspeedmax;
	}
	else if (xdir_ == 1)
	{
		if (PlayerAnimation.getCurrentAnimationID() != 1)//Not going right.
			PlayerAnimation.setAnimation(1);
		if (!PlayerAnimation.getIsPlaying())
			PlayerAnimation.setPlaying(true);
		if (hspeed + HAUse <= hspeedmax)
			hspeed += HAUse;
		else
			hspeed = hspeedmax;
	}
	else if (hspeed != 0)
	{
		if (PlayerAnimation.getIsPlaying())
		{
			PlayerAnimation.setPlaying(false);
			PlayerAnimation.resetAnimation();
		}
		if (hspeed > 0)
		{
			if (hspeed - frixUse >= 0)
				hspeed -= frixUse;
			else
				hspeed = 0;
		}
		else
		{
			if (hspeed + frixUse <= 0)
				hspeed += frixUse;
			else
				hspeed = 0;
		}

	}
	if (hspeed != 0)
	{
		if (WCR.MapPtr->CheckCollision(BoundBox, x + hspeed, y, 1) || WCR.MapPtr->CheckCollision(BoundBox, x + hspeed, y, 0))
		{
			bool fixed = false;
			if (WCR.MapPtr->CheckCollision(BoundBox, x + hspeed, y, 0) != 1)
			{
				for (int i = 1; i <= 17; i++)//17 because there seems to be a rounding bug?
					if (!WCR.MapPtr->CheckCollision(BoundBox, x + hspeed, y - i, 1))
					{
						fixed = true;
						MovePlayer(hspeed, -i);
						break;
					}
			}
			if (!fixed)
			{
				if (hspeed > 0)
					ContractDir(E_right);//Error?
				else if (hspeed < 0)
					ContractDir(E_left);
				hspeed = 0;
			}
		}
		else
			MovePlayer(hspeed, 0);
	}
	if (falling)
	{
		if (vspeed < 0)
		{
			//WCR.MapPtr->CheckCollision(BoundBox, x + hspeed, y)
			if (WCR.MapPtr->CheckCollision(BoundBox, x, y + vspeed, 1) || WCR.MapPtr->CheckCollision(BoundBox, x, y + vspeed, 0) == 1)
			{
				ContractDir(E_up);
				vspeed = 0;
			}
			else
				MovePlayer(0, vspeed);
		}
		else
		{
			if (WCR.MapPtr->CheckCollision(BoundBox, x, y + vspeed, 1) || WCR.MapPtr->CheckCollision(BoundBox, x, y + vspeed, 0) == 1)
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
		if (!WCR.MapPtr->CheckCollision(BoundBox, x, y + 1, 1))//not a solid
			falling = true;
	}
	if (WCR.MapPtr->CheckCollision(BoundBox, x, y, 2) == 2)//Bouncy Block
	{
		vspeed = -15;
		falling = true;
	}

	int STR=20;//How many subdivides of distance.

	if (xAct > 0)//Have to move right
	{
		xAct -= xAct / STR + 1;
		if (xAct < 0)
			xAct = 0;
	}
	if (xAct < 0)//Have to move left
	{
		xAct -= xAct / STR - 1;
		if (xAct > 0)
			xAct = 0;
	}
	if (yAct > 0)//Have to move down
	{
		yAct -= yAct / STR + 1;
		if (yAct < 0)
			yAct = 0;
	}
	if (yAct < 0)//Have to move up
	{
		yAct -= yAct / STR - 1;
		if (yAct > 0)
			yAct = 0;
	}

	for (int i = 0; i < 100; i++)
	{
		if (WCR.MapPtr->CheckCollision(BoundBox, x, y, 1))
			y -= 1;
		else
			break;
	}

	PlayerImage.setPosition(x - xAct, y - yAct);	
	PlayerAnimation.step();
}

void otherPlayer::ChangeName(const char* NewName)
{
#ifdef MapMakerMode
	myName = new char[strlen(NewName) + 1];
	strcpy(myName, NewName);
	NameText.setString(myName);
	stringstream TS;
	TS << "(" << PID << ") " << NewName;
	NameText.setString(TS.str());
#else
	myName = new char[strlen(NewName) + 1];
	strcpy(myName, NewName);
	NameText.setString(myName);
#endif
	
}

void otherPlayer::draw()
{
	for (int i = 0; i < AfterImage.size(); i++)
		WCR.RenderRef.draw(AfterImage[i]);
	//WCR.RenderRef.draw(PlayerImage);
	PlayerAnimation.draw(x - xAct, y - yAct);
	WCR.RenderRef.draw(NameText);
}
