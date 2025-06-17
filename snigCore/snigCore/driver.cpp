#include "driver.h"

ByyDriver::ByyDriver( ByyRemoteObjectManager& objectManager,const QString &instanceName )
	:myObjectManager(objectManager)
,myName(instanceName)
{
	myStartedFlag=false;
}

ByyDriver::~ByyDriver()
{
	
}

void ByyDriver::start()
{
	if (!myStartedFlag&&onStart())
	{
		myStartedFlag=true;
	}
}

void ByyDriver::stop()
{
	if (myStartedFlag&&onStop())
	{
		myStartedFlag=false;
	}
}

bool ByyDriver::tick()
{
	return onTick();
}

bool ByyDriver::started() const
{
	return myStartedFlag;
}

QString ByyDriver::instanceName() const
{
	return myName;
}

bool ByyDriver::isVRLink() const
{
	return false;
}
