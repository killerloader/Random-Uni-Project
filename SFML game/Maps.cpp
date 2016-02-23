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
	MapMatrix = vector<vector<int>>(MapWidth, vector<int>(MapHeight, 0));//-1 is a null object id
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

void Map::SetObject(int x, int y, int ID)
{
	//X and Y are grid cells not actual coords.
	WCR.LimitVariable(0, MapWidth - 1, x);
	WCR.LimitVariable(0, MapHeight - 1, y);
	if (x<floor((WCR.MMPtr->MapMakrView.getCenter().x - (WCR.MMPtr->MapMakrView.getSize().x / 2)) / 32)
		|| x > ceil((WCR.MMPtr->MapMakrView.getCenter().x + (WCR.MMPtr->MapMakrView.getSize().x / 2)) / 32)
		|| y < floor((WCR.MMPtr->MapMakrView.getCenter().y - (WCR.MMPtr->MapMakrView.getSize().y / 2)) / 32)
		|| y > ceil((WCR.MMPtr->MapMakrView.getCenter().y + (WCR.MMPtr->MapMakrView.getSize().y / 2)) / 32))
		return;
	MapMatrix[x][y] = ID;
}

void Map::DrawMap(sf::View& ViewRef)
{
#if MapMakerMode == true
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

	for (int i = MinX; i <= MaxX; i++)
		for (int ii = MinY; ii <= MaxY; ii++)
		{
			if (MapMatrix[i][ii] != 0)//Solid here
			{
				SolidObj.setPosition(i * CellSize, ii * CellSize);
				switch (MapMatrix[i][ii])
				{
				case 1://Generic Solid
					SolidObj.setFillColor(sf::Color::Blue);
					break;
				case 2://Bouncy object
					SolidObj.setFillColor(sf::Color::Green);
					break;
				case 3://Death object
					SolidObj.setFillColor(sf::Color::Red);
					break;
				case 4://Finish Line
					SolidObj.setFillColor(sf::Color(255,128,64));
					break;
				}
				WCR.RenderRef.draw(SolidObj);
			}
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
			case -2:IsTrue = (MapMatrix[i][ii] != 0); break;
			case 0:break;//Nothing, don't search for empty space. Only search for map edge.
			default:IsTrue = (MapMatrix[i][ii] == CheckType); break;
			}
			if (IsTrue)//Object here
			{
				sf::Rect<int> SolidBoundBox;
				SolidBoundBox.left = i * CellSize;
				SolidBoundBox.top = ii * CellSize;
				SolidBoundBox.width = CellSize; SolidBoundBox.height = CellSize;
				if (CheckRect.intersects(SolidBoundBox))
					return MapMatrix[i][ii];
			}
		}
	return false;
}