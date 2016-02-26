#include "mapObject.h"

mapObject::mapObject(int OID, int TID, int TSID)
{
	objectType = OID;
	tileID = TID;
	tileSetID = TSID;
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
