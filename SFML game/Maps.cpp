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
	MapMatrix = vector<vector<mapObject>>(MapWidth, vector<mapObject>(MapHeight));//-1 is a null object id
	setupBorders();
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

void Map::SetObject(int x, int y, int ID, int TID, int TSID, bool PP)
{
	//X and Y are grid cells not actual coords.
	WCR.LimitVariable(0, MapWidth - 1, x);
	WCR.LimitVariable(0, MapHeight - 1, y);
	if (x<floor((WCR.MMPtr->MapMakrView.getCenter().x - (WCR.MMPtr->MapMakrView.getSize().x / 2)) / 32)
		|| x > ceil((WCR.MMPtr->MapMakrView.getCenter().x + (WCR.MMPtr->MapMakrView.getSize().x / 2)) / 32)
		|| y < floor((WCR.MMPtr->MapMakrView.getCenter().y - (WCR.MMPtr->MapMakrView.getSize().y / 2)) / 32)
		|| y > ceil((WCR.MMPtr->MapMakrView.getCenter().y + (WCR.MMPtr->MapMakrView.getSize().y / 2)) / 32))
		return;
	MapMatrix[x][y].objectType = ID;
	MapMatrix[x][y].tileID = TID;
	MapMatrix[x][y].tileSetID = TSID;
	MapMatrix[x][y].pixelPerfect = PP;

	if (WCR.connected)
	{
		sf::Packet mapData;
		mapData << (sf::Int32)1 << (sf::Int32)x << (sf::Int32)y << (sf::Int32)ID << (sf::Int32)TID << (sf::Int32)TSID << PP;
		for (int i = 0; i < WCR.clients.size(); i++)
			if (WCR.clients[i] != nullptr)
				WCR.clients[i]->send(mapData);
	}
}

void Map::DrawMap(sf::View& ViewRef)
{
#ifdef MapMakerMode
	int MinX = floor((WCR.MMPtr->MapMakrView.getCenter().x - (WCR.MMPtr->MapMakrView.getSize().x / 2)) / CellSize);
	int MaxX = ceil((WCR.MMPtr->MapMakrView.getCenter().x + (WCR.MMPtr->MapMakrView.getSize().x / 2)) / CellSize);
	int MinY = floor((WCR.MMPtr->MapMakrView.getCenter().y - (WCR.MMPtr->MapMakrView.getSize().y / 2)) / CellSize);
	int MaxY = ceil((WCR.MMPtr->MapMakrView.getCenter().y + (WCR.MMPtr->MapMakrView.getSize().y / 2)) / CellSize);
#else
	int MinX = floor((ViewRef.getCenter().x - ViewRef.getSize().x / 2) / CellSize);
	int MaxX = ceil((ViewRef.getCenter().x + ViewRef.getSize().x / 2) / CellSize);
	int MinY = floor((ViewRef.getCenter().y - ViewRef.getSize().y / 2) / CellSize);
	int MaxY = ceil((ViewRef.getCenter().y + ViewRef.getSize().y / 2) / CellSize);
#endif
	WCR.LimitVariable(0, MapWidth - 1, MinX);
	WCR.LimitVariable(0, MapWidth - 1, MaxX);
	WCR.LimitVariable(0, MapHeight - 1, MinY);
	WCR.LimitVariable(0, MapHeight - 1, MaxY);
	sf::Drawable* DrawPtr;
	for (int i = MinX; i <= MaxX; i++)
		for (int ii = MinY; ii <= MaxY; ii++)
		{
			if (MapMatrix[i][ii].tileID != -1)//Not empty space.
			{
				WCR.MMPtr->TileSets[MapMatrix[i][ii].tileSetID].Tiles[MapMatrix[i][ii].tileID].setPosition(i * CellSize, ii * CellSize);
				WCR.RenderRef.draw(WCR.MMPtr->TileSets[MapMatrix[i][ii].tileSetID].Tiles[MapMatrix[i][ii].tileID]);
#ifdef MapMakerMode
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
				}
#endif
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
	
	int Xmin = floor(X / CellSize);
	int Xmax = floor((X + CheckRect.width) / CellSize);
	int Ymin = floor(Y / CellSize);
	int Ymax = floor((Y + CheckRect.height) / CellSize);
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
					{
						return MapMatrix[i][ii].objectType;
					}
					else
					{
						//get image and player box overlap
						int Left_ = X - i * 32;
						int Top_ = Y - ii * 32;
						int Wid_ = CheckRect.width;
						int Hei_ = CheckRect.height;

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

						if (Wid_ > 31)
							Wid_ = 31;
						if (Hei_ > 31)
							Hei_ = 31;

						for (int f = Wid_; f >= Left_; f--)//<= ??
							for (int ff = Hei_; ff >= Top_; ff--)//<= ??
								if (WCR.MMPtr->TileSets[MapMatrix[i][ii].tileSetID].CollisionMaps[MapMatrix[i][ii].tileID].getPixel(f, ff).a != 0)
									return true;
					}
					
				}
					
			}
		}
	return false;
}