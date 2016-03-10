#include "mapObject.h"

mapObject::mapObject(int OID, int TID, int TSID, bool PP = false)
{
	objectType = OID;
	tileID = TID;
	tileSetID = TSID;
	pixelPerfect = PP;
}

mapObject::mapObject()
{
	objectType = 0;
	tileID = -1;
	tileSetID = 0;
}

mapObject::~mapObject()
{

}
