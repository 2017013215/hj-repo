#include "StdAfx.h"
#include "tacticalGraphicsProcesser.h"
#include "ByyIG.h"
#include "ByyCore.h"
#include "localObjectManager.h"
#include "ByyControlObject.h"
#include "tacticalGraphicsCreator.h"

ByyTacticalGraphicsProcesser::ByyTacticalGraphicsProcesser( ByyIG& ig,QObject *parent/*=0*/ ) 
	: QObject(parent)
	,myIG(ig)
	,myCreator(0)
{
	
}

ByyTacticalGraphicsProcesser::~ByyTacticalGraphicsProcesser()
{
	release();
}

void ByyTacticalGraphicsProcesser::addCreatorCreator( const QString& type,ByyGraphicsCreator func )
{
	myGraphicsCreators.insert(type,func);
}

void ByyTacticalGraphicsProcesser::addEditorCreator( const QString& type,ByyGraphicsEditorCreator func )
{
	myGraphicsEditorCreators.insert(type,func);
}

void ByyTacticalGraphicsProcesser::edit( ByyControlObject* obj )
{
	if (obj)
	{
		createEditor(obj->type(),obj);
	}
}

void ByyTacticalGraphicsProcesser::createCreator( const QString& type )
{
	ByyGraphicsCreatorList::iterator it=myGraphicsCreators.find(type);

	if (it!=myGraphicsCreators.end())
	{
		release();

		myCreator=(*it.value())(myIG);
		connect(myCreator,SIGNAL(finished(int)),this,SLOT(onCreateFinished(int)),Qt::QueuedConnection);
	}
}

void ByyTacticalGraphicsProcesser::createEditor( const QString& type,ByyControlObject* obj )
{
	ByyGraphicsEditorCreatorList::iterator it=myGraphicsEditorCreators.find(type);

	if (it!=myGraphicsEditorCreators.end())
	{
		release();

		myCreator=(*it.value())(myIG,obj);
		connect(myCreator,SIGNAL(finished(int)),this,SLOT(onEditFinished(int)),Qt::QueuedConnection);
	}
}

void ByyTacticalGraphicsProcesser::release()
{
	if (myCreator)
	{
		delete myCreator;
		myCreator=0;
	}
}

void ByyTacticalGraphicsProcesser::onCreateFinished( int status )
{
	if (myCreator&&status==1)
	{
		ByyControlObject* obj=myCreator->accept();

		emit graphicsCreated(obj);
	}

	release();
}

void ByyTacticalGraphicsProcesser::onEditFinished( int status )
{
	if (myCreator&&status==1)
	{
		myCreator->accept();
	}

	release();
}
