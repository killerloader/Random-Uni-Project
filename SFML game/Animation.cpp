#include "Main.h"

Animation::Animation(WrapperClass& WCR_, sf::Color col_) : WCR(WCR_)
{
	CurTileSet = -1;
	CurTile = -1;
	CurrentTime = 0;
	Playing = false;
	MyColor = col_;
	DrawSprite.setColor(MyColor);
}

Animation::~Animation()
{

}

void Animation::setColor(sf::Color newCol_)
{
	MyColor = newCol_;
	DrawSprite.setColor(newCol_);
}

void Animation::addAnimation(const char* FileName, int xOff, int yOff, int xGap, int xCells, int cellSizeX, int cellSizeY, int FrmTm_)
{
	MyTilesets.emplace_back(FileName, xOff, yOff, xGap, 0, xCells, 1, cellSizeX, cellSizeY, FrmTm_);//Only 2d sheets, but can have multiple
	if (CurTileSet == -1)
	{
		CurTileSet = 0;
		CurTile = 0;
		updateRect();
	}
}

sf::Texture* Animation::getTexture(int TexId)
{
	//Mainly for sharing textures.
	//Returns pointer to texture of animation tileset.
	return MyTilesets[TexId].TileSheetTex;
}

void Animation::addAnimation(sf::Texture* NewTex , int xOff, int yOff, int xGap, int xCells, const char* TSName, int cellSizeX, int cellSizeY, int FrmTm_)
{
	MyTilesets.emplace_back(NewTex, xOff, yOff, xGap, 0, xCells, 1, TSName, cellSizeX, cellSizeY, FrmTm_);//Only 2d sheets, but can have multiple
	if (CurTileSet == -1)
	{
		CurTileSet = 0;
		CurTile = 0;
		updateRect();
	}
	
}

void Animation::updateRect()
{
	DrawSprite.setTexture(*MyTilesets[CurTileSet].TileSheetTex);
	DrawSprite.setTextureRect(sf::IntRect(MyTilesets[CurTileSet].xOffset + CurTile*(MyTilesets[CurTileSet].GapW + MyTilesets[CurTileSet].CSizeX), MyTilesets[CurTileSet].yOffset, MyTilesets[CurTileSet].CSizeX, MyTilesets[CurTileSet].CSizeY));
}

void Animation::resetAnimation()
{
	CurTile = 0;
	CurrentTime = 0;
	updateRect();
}

void Animation::setPlaying(bool Play_)
{
	Playing = Play_;
}

void Animation::setAnimation(int AnimID)
{
	CurTileSet = AnimID;
	resetAnimation();
}

void Animation::step()
{
	if (CurTileSet == -1 || !Playing)
		return;
	CurrentTime++;
	if (CurrentTime > MyTilesets[CurTileSet].FrameTime)
	{
		CurrentTime = 0;
		CurTile++;
		if (CurTile >= MyTilesets[CurTileSet].CellsX)
			CurTile = 0;
		updateRect();
	}
}

void Animation::draw(int x_, int y_)
{
	if (CurTileSet == -1)
		return;
	DrawSprite.setPosition(x_, y_);
	WCR.RenderRef.draw(DrawSprite);
}