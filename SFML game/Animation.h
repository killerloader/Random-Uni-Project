#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

class TileSet;
class WrapperClass;

class Animation
{
public:
	Animation(WrapperClass& WCR_, sf::Color Col_ = sf::Color::White);
	~Animation();

	WrapperClass& WCR;
	void draw(int x_, int y_);
	void step();
	void addAnimation(const char* FileName, int xOff, int yOff, int xGap, int xCells, int cellSizeX, int cellSizeY, int FrmTm_);
	void addAnimation(sf::Texture* NewTexture , int xOff, int yOff, int xGap, int xCells, const char* TSName, int cellSizeX, int cellSizeY, int FrmTm_);
	void setAnimation(int AnimID);
	void setColor(sf::Color);
	void resetAnimation();
	void setPlaying(bool Play_);
	bool getIsPlaying()
	{
		return Playing;
	}
	int getCurrentAnimationID()
	{
		return CurTileSet;
	}
	sf::Sprite& getCurrentSprite()
	{
		return DrawSprite;
	}
	sf::Texture* getTexture(int TexId);
private:
	void updateRect();

	int CurrentTime;
	vector<TileSet> MyTilesets;
	int CurTileSet;
	int CurTile;
	bool Playing;
	sf::Color MyColor;
	sf::Sprite DrawSprite;
};

