#include "StdAfx.h"
#include "entityLevelDisplayManager.h"
#include "ByyIG.h"
#include "ByyCore.h"
#include "remoteObjectManager.h"
#include "entityObject.h"
#include "subjectionVisualizer.h"

ByyEntityLevelDisplayManager::ByyEntityLevelDisplayManager( ByyIG& ig ) 
	: myIG(ig)
{
	ByyRemoteObjectManager& remoteObjMgr=ig.core().remoteObjectManager();

	connect(&remoteObjMgr,SIGNAL(entityAppend(ByyEntityObject*)),this,SLOT(appendEntObject(ByyEntityObject*)));
	connect(&remoteObjMgr,SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(removeEntObject(ByyEntityObject*)));

	//QTimer* t=new QTimer;



	connect(&myIG,SIGNAL(preTick()),this,SLOT(tick()));

	//t->start(100);
}

ByyEntityLevelDisplayManager::~ByyEntityLevelDisplayManager()
{

}

void ByyEntityLevelDisplayManager::appendEntObject( ByyEntityObject* entObj )
{
	if (entObj->level()>1)
	{
		myUnMatchingEntityObjList.insert(entObj,entObj->id());
	}
}

void ByyEntityLevelDisplayManager::removeEntObject( ByyEntityObject* entObj )
{
	QMap<ByyEntityObject*,int>::iterator it=myUnMatchingEntityObjList.find(entObj);

	if (it!=myUnMatchingEntityObjList.end())
	{
		myUnMatchingEntityObjList.erase(it);
	}
	else
	{
		Iter it=mySubjectionVisualizers.begin();

		for (;it!=mySubjectionVisualizers.end();)
		{
			if (it.key().first==entObj||it.key().second==entObj)
			{
				delete it.value();

				it=mySubjectionVisualizers.erase(it);
			}
			else
				++it;
		}
	}
}

void ByyEntityLevelDisplayManager::tick()
{
	QMap<ByyEntityObject*,int>::iterator it=myUnMatchingEntityObjList.begin();

	for (;it!=myUnMatchingEntityObjList.end();)
	{
		ByyEntityObject* parentEnt=myIG.core().remoteObjectManager().findEntity(it.key()->superiorName());

		if (!parentEnt)
		{
			++it;
			continue;
		}

		ByySubjectionVisualizer* visualizer=new ByySubjectionVisualizer(parentEnt,it.key(),myIG);

		mySubjectionVisualizers.insert(qMakePair(parentEnt,it.key()),visualizer);

		it=myUnMatchingEntityObjList.erase(it);

	}
}
