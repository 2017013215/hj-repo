#include "remoteObjectManager.h"
#include "entityObject.h"
#include "ByyControlObject.h"
#include "ByySensorSR.h"
#include "ByyDetectObject.h"

ByyRemoteObjectManager::ByyRemoteObjectManager()
	: ByyAbsObjectManager()
	,myShowRole(White)
{
	canSynEntityEvent = false;
	connect(&ByySensorSRSignaler::instance(),SIGNAL(discoverDetect(ByyDetectObject*,ByySensorSR*)),this,SLOT(appendDetectObject(ByyDetectObject*)));
	connect(&ByySensorSRSignaler::instance(),SIGNAL(detectRemoved(ByyDetectObject*,ByySensorSR*)),this,SLOT(removeDetectObject(ByyDetectObject*)));
}

ByyRemoteObjectManager::~ByyRemoteObjectManager()
{
	
}

bool canVisible(ByyRemoteObjectManager::ShowRole role,ByyObject* obj)
{
	return role==ByyRemoteObjectManager::White||
		(role==ByyRemoteObjectManager::Red&&obj->forceType()==ByyForceFriendly)||
		(role==ByyRemoteObjectManager::Blue&&obj->forceType()==ByyForceOpposing);
}

void ByyRemoteObjectManager::appendEntity( ByyEntityObject* entObj )
{
	myRefEntObjList.append(entObj);
	entObj->setVisible(canVisible(myShowRole,entObj));
	if(isMissile(entObj) || isTorpedo(entObj) && canSynEntityEvent)
	{
		emit addMissileEntityObject(entObj->forceType(), entObj->name(), entObj->itemClass());
	}
	emit entityAppend(entObj);
}

void ByyRemoteObjectManager::removeEntity( ByyEntityObject* entObj )
{
	if (!entObj->isLocal()&&myRefEntObjList.contains(entObj))
	{
		if(!isMissile(entObj) && !isTorpedo(entObj) && canSynEntityEvent) 
		{
			emit removeEntityObject(entObj->forceType(), entObj->name(), entObj->itemClass());
		}
		myRefEntObjList.removeOne(entObj);
		emit entityRemoved(entObj);
	}
}

void ByyRemoteObjectManager::appendContrlObject( ByyControlObject* obj )
{
	myRefCtrlObjList.append(obj);
	obj->setVisible(canVisible(myShowRole,obj));
	emit contrlObjectAppend(obj);
}

void ByyRemoteObjectManager::removeContrlObject( ByyControlObject* obj )
{
	myRefCtrlObjList.removeOne(obj);
	emit contrlObjectRemoved(obj);
}

ByyEntityObject* ByyRemoteObjectManager::findEntity( const QString& name )
{
	for (int i=0;i!=myRefEntObjList.size();++i)
	{
		if (myRefEntObjList[i]->name()==name)
		{
			return myRefEntObjList[i];
		}
	}
	return 0;
}

ByyEntityObject* ByyRemoteObjectManager::findEntity( int id )
{
	for (int i=0;i!=myRefEntObjList.size();++i)
	{
		if (myRefEntObjList[i]->id()==id)
		{
			return myRefEntObjList[i];
		}
	}
	return 0;
}

ByyControlObject* ByyRemoteObjectManager::findControlObj( const QString& name )
{
	for (int i=0;i!=myRefCtrlObjList.size();++i)
	{
		if (myRefCtrlObjList[i]->name()==name)
		{
			return myRefCtrlObjList[i];
		}
	}
	return 0;
}

void ByyRemoteObjectManager::setShowRole( ShowRole sr )
{
	if (sr==myShowRole)
		return;

	myShowRole=sr;


	{
		QList<ByyEntityObject*>::iterator it=myRefEntObjList.begin();

		for (;it!=myRefEntObjList.end();++it)
		{
			(*it)->setVisible(canVisible(myShowRole,*it));
		}
	}
	{
		QList<ByyControlObject*>::iterator it=myRefCtrlObjList.begin();

		for (;it!=myRefCtrlObjList.end();++it)
		{
			(*it)->setVisible(canVisible(myShowRole,*it));
		}
	}

	emit showRoleChanged(myShowRole);
}

ByyRemoteObjectManager::ShowRole ByyRemoteObjectManager::getShowRole()
{
	return myShowRole;
}

void ByyRemoteObjectManager::setCanSynEntityEvent(bool isCan)
{
	canSynEntityEvent = isCan;
}

void ByyRemoteObjectManager::appendDetectObject( ByyDetectObject* obj )
{
	myRefDetectObjList.append(obj);

	if ((myShowRole==ByyRemoteObjectManager::Red&&obj->forceType()!=ByyForceFriendly)||
		(myShowRole==ByyRemoteObjectManager::Blue&&obj->forceType()!=ByyForceOpposing))
	{
		emit detectAppend(obj);
	}
}

void ByyRemoteObjectManager::removeDetectObject( ByyDetectObject* obj )
{
	myRefDetectObjList.removeOne(obj);

	emit detectRemoved(obj);
}

QList<ByyDetectObject*>& ByyRemoteObjectManager::detectObjects()
{
	return myRefDetectObjList;
}

QList<ByyEntityObject*>& ByyRemoteObjectManager::entityObjects()
{
	return myRefEntObjList;
}