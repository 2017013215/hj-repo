#include "entityObjectFrame.h"
#include "entityObject.h"

ByyEntityObjectFrame::ByyEntityObjectFrame( ByyEntityObject* entObj )
{
	myId=entObj->id();
	mySide=entObj->forceType();
	myName=entObj->name();
	mySuperiorName=entObj->superiorName();

	myPosition=entObj->position();   // lat lon high
	myOrientation=entObj->orientation();//  hpr

	mySpeed=entObj->speed();
	myVelocity=entObj->velocity();

	myDamageState=entObj->damageState();

	myEntityType=entObj->entityType();

	myLocalObj=entObj->isLocal();
}

ByyEntityObjectFrame::~ByyEntityObjectFrame()
{

}

void ByyEntityObjectFrame::restore( ByyEntityObject* entObj )
{
	entObj->setId(myId);
	entObj->setForceType(mySide);
	entObj->setName(myName);
	entObj->setSuperiorName(mySuperiorName);
	entObj->setPosition(myPosition);
	entObj->setOrientation(myOrientation);
	entObj->setSpeed(mySpeed);
	entObj->setVelocity(myVelocity);
	entObj->setDamageState(myDamageState);
	entObj->setEntityType(myEntityType);
	entObj->setLocal(myLocalObj);
}
