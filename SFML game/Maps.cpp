#include "Maps.h"

Map::Map(WrapperClass &WCR_) : WCR(WCR_) {
	//Setup the size of the map.
	CellSize = 32;
	MapWidth = 100;
	MapHeight = 100;
	ViewWidth = 640;
	ViewX = 0;
	ViewHeight = 480;
	ViewY = 0;
	SolidObj = sf::RectangleShape(sf::Vector2f(32, 32));
	SolidObj.setFillColor(sf::Color::Blue);
	SolidObj.setSize(sf::Vector2f(32, 32));
	//Fill map with null object ids.
	resetMap();
	//setupBorders();
}

int Map::addLayer(int Layer_)
{
	BackgroundMatrix.push_back(vector<vector<mapObject>>(MapWidth, vector<mapObject>(MapHeight, mapObject())));
	BackgroundLayers.push_back(Layer_);
	for (int i = 0; i < OrderedBackgroundLayers.size(); i++)
		if (Layer_ > BackgroundLayers[OrderedBackgroundLayers[i]])
		{
			OrderedBackgroundLayers.insert(OrderedBackgroundLayers.begin() + i, BackgroundMatrix.size()-1);
			return i;
		}
	OrderedBackgroundLayers.push_back(BackgroundMatrix.size() - 1);
	return OrderedBackgroundLayers.size()-1;
}

void Map::resetMap()
{
	MapMatrix = vector<vector<mapObject>>(MapWidth, vector<mapObject>(MapHeight));//-1 is a null object id
	BackgroundMatrix = vector<vector<vector<mapObject>>>(1, vector<vector<mapObject>>(MapWidth, vector<mapObject>(MapHeight)));//-1 is a null object id
	OrderedBackgroundLayers = vector<int>(1, 0);
	BackgroundLayers = vector<int>(1, 1);
	setupBorders();

#ifdef MapMakerMode
	if (WCR.connected)
	{
		sf::Packet mapData;
		mapData << (sf::Int32)5;
		for (int i = 0; i < WCR.clients.size(); i++)
			if (WCR.clients[i] != nullptr)
				WCR.MHandle.sendData(mapData, i);
	}
#endif
}

void Map::expandMap(int newSizeX, int newSizeY)
{
	if (newSizeX > 500 || newSizeY > 500)
	{
		cout << "Map size exceeds maximum size of: " << 500 << "x" << 500 << ", clamping map dimensions!" << endl;
	}
	if (newSizeX > 500)
		newSizeX = 500;
	if (newSizeY > 500)
		newSizeY = 500;
	int expandX = newSizeX - MapWidth;
	int expandY = newSizeY - MapHeight;
	MapWidth = newSizeX;
	MapHeight = newSizeY;
	MapMatrix.resize(newSizeY);
	for (int i = 0; i < MapMatrix.size(); i++)
	{
		MapMatrix[i].resize(newSizeX);
	}
	setupBorders();

#ifdef MapMakerMode
	if (WCR.connected)
	{
		sf::Packet mapData;
		mapData << (sf::Int32)6 << (sf::Int16)newSizeX << (sf::Int16)newSizeY;
		for (int i = 0; i < WCR.clients.size(); i++)
			if (WCR.clients[i] != nullptr)
				WCR.MHandle.sendData(mapData, i);
	}
#endif
}

void Map::setupBorders()
{
	MapBorders[0].setSize(sf::Vector2f(MapWidth * 32 + 16, 8));
	MapBorders[0].setPosition(sf::Vector2f(-8,-8));

	MapBorders[1].setSize(sf::Vector2f(MapWidth * 32 + 16, 8));
	MapBorders[1].setPosition(sf::Vector2f(-8, MapHeight * 32));

	MapBorders[2].setSize(sf::Vector2f(8, MapHeight * 32 + 16));
	MapBorders[2].setPosition(sf::Vector2f(0, MapHeight * 32+8));
	MapBorders[2].setRotation(180);

	MapBorders[3].setSize(sf::Vector2f(8, MapHeight * 32 + 16));
	MapBorders[3].setPosition(sf::Vector2f(MapWidth * 32 + 8, MapHeight * 32 + 8));
	MapBorders[3].setRotation(180);
}

void Map::drawBorders()
{
	for (int i = 0; i < 4; i++)
	{
		WCR.RenderRef.draw(MapBorders[i]);
	}
}

bool Map::isObject(int x, int y)
{
	if (x<0 || y<0 || x >MapWidth - 1 || y >MapHeight - 1)
		return true;
	if (MapMatrix[x][y].tileID != -1)//some tile here
		return true;
	return false;
}

bool Map::isBg(int x, int y, int Layer)
{
	if (x<0 || y<0 || x >MapWidth - 1 || y >MapHeight - 1)
		return true;
	if (BackgroundMatrix[Layer][x][y].tileID != -1)//some tile here
		return true;
	return false;
}

void Map::SetObject(int x, int y, int ID, int TID, int TSID, bool PP, int NoSend, bool Player)
{
	//X and Y are grid cells not actual coords.
	WCR.LimitVariable(0, MapWidth - 1, x);
	WCR.LimitVariable(0, MapHeight - 1, y);
	if(!Player)
		if (x< (int)((WCR.MMPtr->MapMakrView.getCenter().x - (WCR.MMPtr->MapMakrView.getSize().x / 2)) / 32)
			|| x >(int)((WCR.MMPtr->MapMakrView.getCenter().x + (WCR.MMPtr->MapMakrView.getSize().x / 2)) / 32)
			|| y < (int)((WCR.MMPtr->MapMakrView.getCenter().y - (WCR.MMPtr->MapMakrView.getSize().y / 2)) / 32)
			|| y > (int)((WCR.MMPtr->MapMakrView.getCenter().y + (WCR.MMPtr->MapMakrView.getSize().y / 2)) / 32))
			return;
	MapMatrix[x][y].objectType = ID;
	MapMatrix[x][y].tileID = TID;
	MapMatrix[x][y].tileSetID = TSID;
	MapMatrix[x][y].pixelPerfect = PP;

#ifdef MapMakerMode
	if (WCR.connected)
	{
		sf::Packet mapData;
		mapData << (sf::Int32)1 << (sf::Int32)x << (sf::Int32)y << (sf::Int32)ID << (sf::Int32)TID << (sf::Int32)TSID << PP;
		for (int i = 0; i < WCR.clients.size(); i++)
			if (WCR.clients[i] != nullptr && i != NoSend)
				WCR.MHandle.sendData(mapData, i);
	}
#else
	if (WCR.connected)
	{
		sf::Packet mapData;
		mapData << (sf::Int32)2 << (sf::Int32)4 << (sf::Int32)x << (sf::Int32)y << (sf::Int32)ID << (sf::Int32)TID << (sf::Int32)TSID << (sf::Int32)PP;
		WCR.MHandle.sendData(mapData, WCR.socket);
	}
#endif

}

void Map::SetBG(int x, int y, int TID, int TSID, int LID, int NoSend, bool Player)
{
	//X and Y are grid cells not actual coords.
	WCR.LimitVariable(0, MapWidth - 1, x);
	WCR.LimitVariable(0, MapHeight - 1, y);
	if(!Player)
		if (x<(int)((WCR.MMPtr->MapMakrView.getCenter().x - (WCR.MMPtr->MapMakrView.getSize().x / 2)) / 32)
			|| x >(int)((WCR.MMPtr->MapMakrView.getCenter().x + (WCR.MMPtr->MapMakrView.getSize().x / 2)) / 32)
			|| y < (int)((WCR.MMPtr->MapMakrView.getCenter().y - (WCR.MMPtr->MapMakrView.getSize().y / 2)) / 32)
			|| y >(int)((WCR.MMPtr->MapMakrView.getCenter().y + (WCR.MMPtr->MapMakrView.getSize().y / 2)) / 32))
			return;
	BackgroundMatrix[LID][x][y].tileID = TID;
	BackgroundMatrix[LID][x][y].tileSetID = TSID;

#ifdef MapMakerMode
	if (WCR.connected)
	{
		sf::Packet mapData;
		mapData << (sf::Int32)3 << (sf::Int16)x << (sf::Int16)y << (sf::Int8)LID << (sf::Int16)TID << (sf::Int8)TSID;
		for (int i = 0; i < WCR.clients.size(); i++)
			if (WCR.clients[i] != nullptr && i != NoSend)
				WCR.MHandle.sendData(mapData, i);
	}
#else
	if (WCR.connected)
	{
		sf::Packet mapData;
		mapData << (sf::Int32)2 << (sf::Int32)5 << (sf::Int16)x << (sf::Int16)y << (sf::Int8)LID << (sf::Int16)TID << (sf::Int8)TSID;
		WCR.MHandle.sendData(mapData, WCR.socket);
	}
#endif
}

void Map::Drawbackground(sf::View& ViewRef, E_Ground Ground)
{
	int MinX, MaxX, MinY, MaxY;
	if (WCR.inMapMaker)
	{
		MinX = (int)((WCR.MMPtr->MapMakrView.getCenter().x - (WCR.MMPtr->MapMakrView.getSize().x / 2)) / CellSize);
		MaxX = (int)((WCR.MMPtr->MapMakrView.getCenter().x + (WCR.MMPtr->MapMakrView.getSize().x / 2)) / CellSize);
		MinY = (int)((WCR.MMPtr->MapMakrView.getCenter().y - (WCR.MMPtr->MapMakrView.getSize().y / 2)) / CellSize);
		MaxY = (int)((WCR.MMPtr->MapMakrView.getCenter().y + (WCR.MMPtr->MapMakrView.getSize().y / 2)) / CellSize);
	}
	else
	{
		MinX = (int)((ViewRef.getCenter().x - ViewRef.getSize().x / 2) / CellSize);
		MaxX = (int)((ViewRef.getCenter().x + ViewRef.getSize().x / 2) / CellSize);
		MinY = (int)((ViewRef.getCenter().y - ViewRef.getSize().y / 2) / CellSize);
		MaxY = (int)((ViewRef.getCenter().y + ViewRef.getSize().y / 2) / CellSize);
	}
	WCR.LimitVariable(0, MapWidth - 1, MinX);
	WCR.LimitVariable(0, MapWidth - 1, MaxX);
	WCR.LimitVariable(0, MapHeight - 1, MinY);
	WCR.LimitVariable(0, MapHeight - 1, MaxY);
	for (int LID = 0; LID < BackgroundMatrix.size(); LID++)
	{
		if (WCR.MMPtr->OnlyShowThisLayer->IsActive() && WCR.inMapMaker && OrderedBackgroundLayers[LID] != WCR.MMPtr->curTileLayer)
			continue;//Skip, not the layer we want.
		int BL_ = BackgroundLayers[OrderedBackgroundLayers[LID]];
		if (Ground == E_vForeground)
		{
			if (BL_ >= 0)
				continue;//Not up to yet
		}
		else if (Ground == E_vMiddleground)
		{
			if (BL_ < 0)
				break;//Gone past
			if (BL_ != 0)
				continue;//Go till found.
		}
		else if (Ground == E_vBackground)
		{
			if (BL_ < 1)
				break;//Gone past
		}
		
		BL_ = OrderedBackgroundLayers[LID];
		for (int i = MinX; i <= MaxX; i++)
			for (int ii = MinY; ii <= MaxY; ii++)
			{
				if (BackgroundMatrix[BL_][i][ii].tileID != -1)//Not empty space.
				{
					WCR.MMPtr->TileSets[BackgroundMatrix[BL_][i][ii].tileSetID].Tiles[BackgroundMatrix[BL_][i][ii].tileID].setPosition(i * CellSize, ii * CellSize);
					WCR.RenderRef.draw(WCR.MMPtr->TileSets[BackgroundMatrix[BL_][i][ii].tileSetID].Tiles[BackgroundMatrix[BL_][i][ii].tileID]);
				}
			}
	}
}

void Map::DrawMap(sf::View& ViewRef)
{
	int MinX, MaxX, MinY, MaxY;
	if (WCR.inMapMaker)
	{
		MinX = (int)((WCR.MMPtr->MapMakrView.getCenter().x - (WCR.MMPtr->MapMakrView.getSize().x / 2)) / CellSize);
		MaxX = (int)((WCR.MMPtr->MapMakrView.getCenter().x + (WCR.MMPtr->MapMakrView.getSize().x / 2)) / CellSize);
		MinY = (int)((WCR.MMPtr->MapMakrView.getCenter().y - (WCR.MMPtr->MapMakrView.getSize().y / 2)) / CellSize);
		MaxY = (int)((WCR.MMPtr->MapMakrView.getCenter().y + (WCR.MMPtr->MapMakrView.getSize().y / 2)) / CellSize);
	}
	else
	{
		MinX = (int)((ViewRef.getCenter().x - ViewRef.getSize().x / 2) / CellSize);
		MaxX = (int)((ViewRef.getCenter().x + ViewRef.getSize().x / 2) / CellSize);
		MinY = (int)((ViewRef.getCenter().y - ViewRef.getSize().y / 2) / CellSize);
		MaxY = (int)((ViewRef.getCenter().y + ViewRef.getSize().y / 2) / CellSize);
	}
	WCR.LimitVariable(0, MapWidth - 1, MinX);
	WCR.LimitVariable(0, MapWidth - 1, MaxX);
	WCR.LimitVariable(0, MapHeight - 1, MinY);
	WCR.LimitVariable(0, MapHeight - 1, MaxY);
	//sf::Drawable* DrawPtr;
	for (int i = MinX; i <= MaxX; i++)
		for (int ii = MinY; ii <= MaxY; ii++)
		{
			if (MapMatrix[i][ii].tileID != -1)//Not empty space.
			{
				if (!(WCR.inMapMaker && WCR.MMPtr->HideObjectSprites->IsActive()))
				{
					WCR.MMPtr->TileSets[MapMatrix[i][ii].tileSetID].Tiles[MapMatrix[i][ii].tileID].setPosition(i * CellSize, ii * CellSize);
					WCR.RenderRef.draw(WCR.MMPtr->TileSets[MapMatrix[i][ii].tileSetID].Tiles[MapMatrix[i][ii].tileID]);
				}
				if (WCR.inMapMaker)
					if (WCR.MMPtr->viewObjectTypes)
					{
						sf::RectangleShape DrawRect;
						DrawRect.setSize(sf::Vector2f(32, 32));						
						switch (MapMatrix[i][ii].objectType)
						{
						case 0:DrawRect.setFillColor(sf::Color::White); break;//Invis
						case 1:DrawRect.setFillColor(sf::Color::Blue); break;//Solid
						case 2:DrawRect.setFillColor(sf::Color::Green); break;//Bouncy
						case 3:DrawRect.setFillColor(sf::Color::Red); break;//Death
						case 4:DrawRect.setFillColor(sf::Color::Yellow); break;//Next Level
						case 5:DrawRect.setFillColor(sf::Color::Cyan); break;//Ice
						}
						DrawRect.setFillColor(sf::Color(DrawRect.getFillColor().r, DrawRect.getFillColor().g, DrawRect.getFillColor().b, 100));
						DrawRect.setPosition(i * CellSize, ii * CellSize);
						WCR.RenderRef.draw(DrawRect);
						if (MapMatrix[i][ii].pixelPerfect)
						{
							sf::CircleShape DrawCirc;
							DrawCirc.setRadius(8);
							DrawCirc.setPosition(i * CellSize + 8, ii * CellSize + 8);
							sf::Color MyCol_ = sf::Color::Black;
							MyCol_.a = 100;
							DrawCirc.setFillColor(MyCol_);

							WCR.RenderRef.draw(DrawCirc);
						}
					}
			}
		}
}

namespace mmath
{
	int max(int v1, int v2)
	{
		return (v1 > v2) ? v1 : v2;
	}

	int min(int v1, int v2)
	{
		return (v1 < v2) ? v1 : v2;
	}
}


int Map::CheckCollision(sf::Rect<int> CheckRect, int X, int Y, int CheckType)
{
	int Xmin = (int)(X / CellSize);
	int Xmax = (int)((X + CheckRect.width) / CellSize);
	int Ymin = (int)(Y / CellSize);
	int Ymax = (int)((Y + CheckRect.height) / CellSize);
	if(CheckType == 0)
		if (X < 0 || Y < 0)
			return 1;//0 returns 1....
	if (CheckType == -1)
		if (X >= (MapWidth - 1) * 32 || Y >= (MapWidth - 1) * 32)//Shitty method of stoping players from leaving the room.
			return -1;//Outside Room
	CheckRect.left = X; CheckRect.top = Y;
	WCR.LimitVariable(0, MapWidth - 1, Xmin);
	WCR.LimitVariable(0, MapWidth - 1, Xmax);
	WCR.LimitVariable(0, MapHeight - 1, Ymin);
	WCR.LimitVariable(0, MapHeight - 1, Ymax);
	for (int i = Xmin; i <= Xmax; i++)
		for (int ii = Ymin; ii <= Ymax; ii++)
		{
			bool IsTrue = false;
			switch (CheckType)//Check for any
			{
			case -2:IsTrue = (MapMatrix[i][ii].objectType != 0); break;
			case 1:IsTrue = (MapMatrix[i][ii].objectType == 1 || MapMatrix[i][ii].objectType == 5); break;//solids
			case 0:break;//Nothing, don't search for empty space. Only search for map edge.
			default:IsTrue = (MapMatrix[i][ii].objectType == CheckType); break;
			}
			if (IsTrue)//Object here
			{
				sf::Rect<int> SolidBoundBox;
				SolidBoundBox.left = i * CellSize;
				SolidBoundBox.top = ii * CellSize;
				SolidBoundBox.width = CellSize; SolidBoundBox.height = CellSize;
				if (CheckRect.intersects(SolidBoundBox))
				{
					if (!MapMatrix[i][ii].pixelPerfect)
						return MapMatrix[i][ii].objectType;
					else
					{
						//get image and player box overlap
						int Left_ = X - i * 32;
						int Top_ = Y - ii * 32;
						int SLeft_ = Left_;
						int STop_ = Top_;
						int Wid_ = CheckRect.width-1;
						int Hei_ = CheckRect.height - 1;

						if (Left_ < 0)
						{
							Wid_ += Left_;
							Left_ = 0;
						}

						if (Top_ < 0)
						{
							Hei_ += Top_;
							Top_ = 0;
						}

						/*if (Top_ > 46)
							Top_ = 46;
						if (Hei_ > 35)
							Hei_ = 35;*/


						if (Wid_ > 31)
							Wid_ = 31;
						if (Hei_ > 31)
							Hei_ = 31;

						for (int f = Wid_; f >= Left_; f--)//<= ??
							for (int ff = Hei_; ff >= Top_; ff--)//<= ??
							{
								if (WCR.MMPtr->TileSets[MapMatrix[i][ii].tileSetID].CollisionMaps[MapMatrix[i][ii].tileID].getPixel(f, ff).a != 0 && WCR.PlrPtr->PlayerMask.getPixel(f - SLeft_, ff - STop_).a != 0)
									return MapMatrix[i][ii].objectType;
							}
					}
					
				}
					
			}
		}
	return false;
}