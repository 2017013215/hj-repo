#include "ByyCore.h"
#include "coreFactory.h"

#include "remoteObjectManager.h"
#include "driverManager.h"
#include "radioCommsManager.h"
#include "taskFactory.h"
#include "taskRegistration.h"
#include "localObjectManager.h"
ByyCore* byyCoreInstance=0;

ByyCore *byyCore()
{
	return byyCoreInstance;

}
ByyCore::ByyCore()
	:myRemoteObjManager(0)
	,myLocalObjManager(0)
	,myDriverManager(0)
	,myTaskFactory(0)
	,myRadioCommsManager(0)
{
	byyCoreInstance=this;
	connect(&timer,SIGNAL(timeout()),SLOT(tick()));

	myBlock=false;
}

ByyCore::~ByyCore()
{
	timer.stop();

	delete myDriverManager;
	delete myRemoteObjManager;
	delete myTaskFactory;
	delete myRadioCommsManager;
}

void ByyCore::initialize()
{
	myLocalObjManager=new ByyLocalObjectManager;
	myRemoteObjManager=new ByyRemoteObjectManager;
	myDriverManager=new ByyDriverManager(myRemoteObjManager);
	myRadioCommsManager=new DtRadioCommsManager(*this);
	
	myTaskFactory=new ByyTaskFactory;
	registerTask(*myTaskFactory);

	//timer.start(50);
	timer.start(100);
}

void ByyCore::tick()
{
	if (myBlock)
		return;

	myDriverManager->tick(); 
	emit postTick();
}

ByyLocalObjectManager& ByyCore::localObjectManager()
{
	return * myLocalObjManager;
}

ByyRemoteObjectManager& ByyCore::remoteObjectManager()
{
	return *myRemoteObjManager;
}

ByyTaskFactory& ByyCore::taskFactory()
{
	return *myTaskFactory;
}

ByyDriverManager& ByyCore::driverManager()
{
	return *myDriverManager;
}

DtRadioCommsManager& ByyCore::radioCommsManager()
{
	return *myRadioCommsManager;
}

void ByyCore::block( bool bl )
{
	myBlock=bl;
}
