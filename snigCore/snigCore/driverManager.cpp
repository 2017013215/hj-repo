#include "driverManager.h"
#include "driver.h"
#include "remoteObjectManager.h"

ByyDriverManager::ByyDriverManager( ByyRemoteObjectManager* objectManager )
	:myObjectManager(objectManager)
{

}

ByyDriverManager::~ByyDriverManager()
{
	for(int i=0;i!=myDriverList.size();++i)
	{
		myDriverList[i]->stop();

		delete myDriverList[i];
	}
}

void ByyDriverManager::addDriver( ByyDriver* driver )
{
	myDriverList.push_back(driver);
}

void ByyDriverManager::destroyDriver( ByyDriver* driver )
{
	myDriverList.removeOne(driver);
	delete driver;
}

void ByyDriverManager::startDriver( ByyDriver* driver )
{
	driver->start();
}

void ByyDriverManager::stopDriver( ByyDriver* driver )
{
	driver->stop();
}

void ByyDriverManager::tick()
{
	for(int i=0;i!=myDriverList.size();++i)
	{
		if (myDriverList[i]->started())myDriverList[i]->tick();
	}
}

ByyDriver* ByyDriverManager::findDriver( const QString& dirverName )
{
	for (int i=0;i!=myDriverList.size();++i)
	{
		if (myDriverList[i]->instanceName()==dirverName)
		{
			return myDriverList[i];
		}
	}

	return NULL;
}

QList<ByyDriver*>& ByyDriverManager::drivers()
{
	return myDriverList;
}
