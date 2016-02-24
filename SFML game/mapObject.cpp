#include "mapObject.h"

mapObject::mapObject(int OID, int TID)
{
	objectType = OID;
	tileID = TID;
}

mapObject::mapObject()
{
	objectType = 0;
	tileID = 0;
}

mapObject::~mapObject()
{
}
