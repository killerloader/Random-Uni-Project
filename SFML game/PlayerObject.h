#pragma once

class MapMaker;
struct Map;
class fileManager;
class mapObject;
class otherPlayer;
class WrapperClass;
class Animation;

class PlayerObject
{
public:
	vector<sf::Sprite> AfterImage;
	WrapperClass &WCR;
	sf::View PlayerView;
	sf::Texture PlayerTex;
	sf::Sprite PlayerImage;
	Animation PlayerAnimation;
	sf::Image PlayerMask;
	PlayerObject(WrapperClass &WCR_);
	~PlayerObject()
	{
		if(myName!=nullptr)
		delete[] myName;
	}
	void PollControls();
	void MovePlayer(float Xmove, float Ymove);
	float vspeed, hspeed, gravity, haccel, haccelSlip, hspeedmax, hfric, x, y, xstart, ystart, SPD, hfricSlip;
	float xDirOld, pressWOld;
	void StepPlayer();
	void DrawPlayer();
	void ResetMovement();
	void sendXChange();
	void sendJump();
	void sendPos();//Positioning of player
	enum Edirection
	{
		E_left, E_right, E_up, E_down
	};
	void ContractDir(Edirection);
	void ChangeName(const char* NewName);

	char* myName;
	sf::Text NameText;


	bool falling;
	int vspeedMax;
	sf::Color MyCol = sf::Color();
	sf::Rect<int> BoundBox;
};