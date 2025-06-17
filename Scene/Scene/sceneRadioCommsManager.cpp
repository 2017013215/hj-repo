#include "StdAfx.h"
#include "sceneRadioCommsManager.h"
#include "ByyIG.h"
#include "entityObject.h"
#include "radioCommsManager.h"
#include "radioCommsVisualizer.h"
#include "ByyCore.h"
#include "remoteObjectManager.h"
#include "trackVisualizer.h"
#include "guidanceVisualizer.h"
#include "routePlanVisualizer.h"

ByySceneRadioCommsManager::ByySceneRadioCommsManager( ByyIG& ig )
	:myIG(ig)
{
	connect(&myIG.core().radioCommsManager(),SIGNAL(commAdded(int,int)),this,SLOT(commAdded(int,int)));
	connect(&myIG.core().radioCommsManager(),SIGNAL(commRemoved(int,int)),this,SLOT(commRemoved(int,int)));

	connect(&myIG.core().radioCommsManager(),SIGNAL(trackAdded(int,int)),this,SLOT(trackAdded(int,int)));
	connect(&myIG.core().radioCommsManager(),SIGNAL(trackRemoved(int,int)),this,SLOT(trackRemoved(int,int)));

	connect(&myIG.core().radioCommsManager(),SIGNAL(guidanceAdded(int,int)),this,SLOT(guidanceAdded(int,int)));
	connect(&myIG.core().radioCommsManager(),SIGNAL(guidanceRemoved(int,int)),this,SLOT(guidanceRemoved(int,int)));

	qRegisterMetaType<QVector<osg::Vec3d>>("QVector<osg::Vec3d>");

	connect(&myIG.core().radioCommsManager(),SIGNAL(routePlanningAdded(int,const QVector<osg::Vec3d>&)),
		this,SLOT(routePlanningAdded(int,const QVector<osg::Vec3d>&)));
	connect(&myIG.core().radioCommsManager(),SIGNAL(routePlanningRemoved(int)),this,SLOT(routePlanningRemoved(int)));
}

ByySceneRadioCommsManager::~ByySceneRadioCommsManager()
{
	qDeleteAll(myCommVisualizers);
	qDeleteAll(myTrackVisualizers);
	qDeleteAll(myGuidanceVisualizers);
	qDeleteAll(myRoutePlanVisualizers);
}

void ByySceneRadioCommsManager::commAdded( int senderId,int targetId )
{
	Iter it=myCommVisualizers.find(qMakePair(senderId,targetId));

	if (it==myCommVisualizers.end())
	{
		ByyEntityObject* sender=myIG.core().remoteObjectManager().findEntity(senderId);
		ByyEntityObject* target=myIG.core().remoteObjectManager().findEntity(targetId);

		if (sender&&target)
		{
			ByyRadioCommsVisualizer* visualizer=new ByyRadioCommsVisualizer(sender,target,myIG);

			myCommVisualizers[qMakePair(senderId,targetId)]=visualizer;
		}
	}
}

void ByySceneRadioCommsManager::commRemoved( int senderId,int targetId )
{
	Iter it=myCommVisualizers.find(qMakePair(senderId,targetId));

	if (it!=myCommVisualizers.end())
	{
		delete it.value();

		myCommVisualizers.erase(it);
	}
}

void ByySceneRadioCommsManager::trackAdded( int senderId,int targetId )
{
	QMap<CommObjects,ByyTrackVisualizer*>::iterator it=myTrackVisualizers.find(qMakePair(senderId,targetId));

	if (it==myTrackVisualizers.end())
	{
		ByyEntityObject* sender=myIG.core().remoteObjectManager().findEntity(senderId);
		ByyEntityObject* target=myIG.core().remoteObjectManager().findEntity(targetId);

		if (sender&&target)
		{
			ByyTrackVisualizer* visualizer=new ByyTrackVisualizer(sender,target,myIG);

			myTrackVisualizers[qMakePair(senderId,targetId)]=visualizer;
		}
	}
}

void ByySceneRadioCommsManager::trackRemoved( int senderId,int targetId )
{
	QMap<CommObjects,ByyTrackVisualizer*>::iterator it=myTrackVisualizers.find(qMakePair(senderId,targetId));

	if (it!=myTrackVisualizers.end())
	{
		delete it.value();

		myTrackVisualizers.erase(it);
	}
}

void ByySceneRadioCommsManager::guidanceAdded( int senderId,int targetId )
{
	QMap<CommObjects,ByyGuidanceVisualizer*>::iterator it=myGuidanceVisualizers.find(qMakePair(senderId,targetId));

	if (it==myGuidanceVisualizers.end())
	{
		ByyEntityObject* sender=myIG.core().remoteObjectManager().findEntity(senderId);
		ByyEntityObject* target=myIG.core().remoteObjectManager().findEntity(targetId);

		if (sender&&target)
		{
			ByyGuidanceVisualizer* visualizer=new ByyGuidanceVisualizer(sender,target,myIG);

			myGuidanceVisualizers[qMakePair(senderId,targetId)]=visualizer;
		}
	}
}

void ByySceneRadioCommsManager::guidanceRemoved( int senderId,int targetId )
{
	QMap<CommObjects,ByyGuidanceVisualizer*>::iterator it=myGuidanceVisualizers.find(qMakePair(senderId,targetId));

	if (it!=myGuidanceVisualizers.end())
	{
		delete it.value();

		myGuidanceVisualizers.erase(it);
	}
}

void ByySceneRadioCommsManager::routePlanningAdded( int entId,const QVector<osg::Vec3d>& pnts )
{
	ByyEntityObject* sender=myIG.core().remoteObjectManager().findEntity(entId);

	if (sender)
	{
		ByyRoutePlanVisualizer *v=new ByyRoutePlanVisualizer(sender,pnts,myIG);

		myRoutePlanVisualizers.insert(entId,v);
	}
}

void ByySceneRadioCommsManager::routePlanningRemoved( int entId )
{
	QMultiMap<int,ByyRoutePlanVisualizer*>::const_iterator i = myRoutePlanVisualizers.find(entId);
	while (i != myRoutePlanVisualizers.end() && i.key() == entId)
	{
		delete i.value();

		++i;
	}

	myRoutePlanVisualizers.remove(entId);
}
