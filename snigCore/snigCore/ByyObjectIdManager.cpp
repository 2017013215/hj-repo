#include "ByyObjectIdManager.h"
#include "ByyObject.h"

ByyObjectIdManager::ByyObjectIdManager()
{

}

ByyObjectIdManager::~ByyObjectIdManager()
{

}

ByyObjectIdManager& ByyObjectIdManager::instance()
{
	static ByyObjectIdManager manager;
	return manager;
}

unsigned int ByyObjectIdManager::createNewObjectId()
{
	static unsigned int currentId=1;
	return currentId++;
}

void ByyObjectIdManager::registerObject( ByyObject *obj )
{
	obj->setObjectId(createNewObjectId());
	myProgramObjects.insert(obj->objectId(),obj);
}

void ByyObjectIdManager::unRegisterObject( ByyObject *obj )
{
	QMap<unsigned int,ByyObject*>::iterator it=myProgramObjects.find(obj->objectId());

	if (it!=myProgramObjects.end()&&obj==it.value())
	{
		myProgramObjects.erase(it);
	}
}

ByyObject* ByyObjectIdManager::getObject( unsigned int id )
{
	ByyObject* rel=0;

	QMap<unsigned int,ByyObject*>::iterator it=myProgramObjects.find(id);

	if (it!=myProgramObjects.end())
	{
		rel=it.value();
	}

	return rel;
}
