#include "radioCommsManager.h"
#include "remoteObjectManager.h"
#include "ByyCore.h"
#include "entityObject.h"

#include <QMetaType>

DtRadioCommsManager::DtRadioCommsManager(ByyCore& core)
	: myCore(core)
{
	ByyRemoteObjectManager& remoteObjMgr=myCore.remoteObjectManager();

	connect(&remoteObjMgr,SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(removeEntObject(ByyEntityObject*)));
}

DtRadioCommsManager::~DtRadioCommsManager()
{

}

void DtRadioCommsManager::addComm( int senderId,int targetId,bool connected )
{
	Iter it=myCommObjects.find(senderId,targetId);

	if (connected&&it==myCommObjects.end())
	{
		myCommObjects.insert(senderId,targetId);

		emit commAdded(senderId,targetId);
	}
	else if (!connected&&it!=myCommObjects.end())
	{
		myCommObjects.erase(it);
		emit commRemoved(senderId,targetId);
	}

}

void DtRadioCommsManager::addTrack( int senderId,int targetId,bool connected )
{
	Iter it=myTrackObjects.find(senderId,targetId);

	if (connected&&it==myTrackObjects.end())
	{
		myTrackObjects.insert(senderId,targetId);

		emit trackAdded(senderId,targetId);
	}
	else if (!connected&&it!=myTrackObjects.end())
	{
		myTrackObjects.erase(it);
		emit trackRemoved(senderId,targetId);
	}
}

void DtRadioCommsManager::addGuidance( int senderId,int targetId,bool connected )
{
	Iter it=myGuidanceObjects.find(senderId,targetId);

	if (connected&&it==myGuidanceObjects.end())
	{
		myGuidanceObjects.insert(senderId,targetId);

		emit guidanceAdded(senderId,targetId);
	}
	else if (!connected&&it!=myGuidanceObjects.end())
	{
		myGuidanceObjects.erase(it);
		emit guidanceRemoved(senderId,targetId);
	}
}

void DtRadioCommsManager::clear()
{
	while (myCommObjects.size())
	{
		emit commRemoved(myCommObjects.begin().key(),myCommObjects.begin().value());

		myCommObjects.erase(myCommObjects.begin());
	}

	while (myTrackObjects.size())
	{
		emit trackRemoved(myTrackObjects.begin().key(),myTrackObjects.begin().value());

		myTrackObjects.erase(myTrackObjects.begin());
	}

	while (myGuidanceObjects.size())
	{
		emit guidanceRemoved(myGuidanceObjects.begin().key(),myGuidanceObjects.begin().value());

		myGuidanceObjects.erase(myGuidanceObjects.begin());
	}
}

void DtRadioCommsManager::removeEntObject( ByyEntityObject* entObj )
{
	int id=entObj->id();

	Iter it=myCommObjects.begin();

	for (;it!=myCommObjects.end();)
	{
		if (it.key()==id||it.value()==id)
		{
			emit commRemoved(it.key(),it.value());

			it=myCommObjects.erase(it);
		}
		else
			++it;
	}

	it=myTrackObjects.begin();

	for (;it!=myTrackObjects.end();)
	{
		if (it.key()==id||it.value()==id)
		{
			emit trackRemoved(it.key(),it.value());

			it=myTrackObjects.erase(it);
		}
		else
			++it;
	}

	it=myGuidanceObjects.begin();

	for (;it!=myGuidanceObjects.end();)
	{
		if (it.key()==id||it.value()==id)
		{
			emit guidanceRemoved(it.key(),it.value());

			it=myGuidanceObjects.erase(it);
		}
		else
			++it;
	}

	emit routePlanningRemoved(id);
}

void DtRadioCommsManager::addRoutePlanning( int entId,QVector<osg::Vec3d> pnts )
{
	emit routePlanningAdded(entId,pnts);
}
