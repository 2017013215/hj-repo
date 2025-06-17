#include "ByyControlObject.h"

ByyControlObject::ByyControlObject() 
	:ByyObject()
	,myClampToTerrain(false)
{
	setClampToTerrain(true); // xks,20220926
}

ByyControlObject::~ByyControlObject()
{

}

ByyKeyValueList ByyControlObject::getConfig()
{
	ByyKeyValueList keyValues=__super::getConfig();

	keyValues.add("CtrlObjName",myName);

	return keyValues;
}

void ByyControlObject::fromConfig(ByyKeyValueList& keyValues )
{
	__super::fromConfig(keyValues);

	keyValues.get("CtrlObjName",myName);
}

void ByyControlObject::setClampToTerrain( bool clamp )
{
	if (myClampToTerrain!=clamp)
	{
		myClampToTerrain=clamp;
		emit clampToTerrainChanged(myClampToTerrain);
	}
}

bool ByyControlObject::clampToTerrain()
{
	return myClampToTerrain;
}
