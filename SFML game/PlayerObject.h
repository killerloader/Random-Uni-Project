#pragma once

class MapMaker;
struct Map;
class fileManager;
class mapObject;
class otherPlayer;
class WrapperClass;

class PlayerObject
{
public:
	vector<sf::Sprite> AfterImage;
	WrapperClass &WCR;
	sf::View PlayerView;
	sf::Texture PlayerTex;
	sf::Sprite PlayerImage;
	PlayerObject(WrapperClass &WCR_);
	void PollControls();
	void MovePlayer(float Xmove, float Ymove);
	float vspeed, hspeed, gravity, haccel, hspeedmax, hfric, x, y, xstart, ystart, SPD;
	float xDirOld, pressWOld;
	void StepPlayer();
	void DrawPlayer();
	void ResetMovement();
	void sendXChange();
	void sendJump();
	void sendPos();
	enum Edirection
	{
		E_left, E_right, E_up, E_down
	};
	void ContractDir(Edirection);
	//Positioning of player
	bool falling;
	int vspeedMax;
	sf::Color MyCol = sf::Color();
	sf::Rect<int> BoundBox;
};