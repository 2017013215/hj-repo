#include "StdAfx.h"
#include "ByyEntityObserver.h"

ByyEntityObserver::ByyEntityObserver(ByyCore& core,ByyEntityObject* entity,QWidget* parent,Qt::WindowFlags f)
	:QWidget(parent,f)
	,myCore(core) 
,myCurrentEntity(entity)

{
	connect(&myCore.remoteObjectManager(),SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(onEntityRemove(ByyEntityObject*)));
	connect(&myCore.localObjectManager(),SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(onEntityRemove(ByyEntityObject*)));
}

ByyEntityObserver::~ByyEntityObserver()
{
	disconnect(&myCore.remoteObjectManager(),SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(onEntityRemove(ByyEntityObject*)));
	disconnect(&myCore.localObjectManager(),SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(onEntityRemove(ByyEntityObject*)));
}

void ByyEntityObserver::setEntity( ByyEntityObject *entity )
{
	myCurrentEntity=entity;
}

void ByyEntityObserver::onEntityRemove( ByyEntityObject* entObj )
{
	if (entObj==myCurrentEntity)
	{
		myCurrentEntity=0;
		setEntity(0);
	}
}
