#include "StdAfx.h"
#include "ByyMeasureProcesser.h"
#include "ByyLineMeasure.h"
#include "ByyPolygonMeasure.h"
#include "ByyIntervisibilityLine.h"
#include "ByyIntervisibilityFan.h"

ByyMeasureProcesser::ByyMeasureProcesser( ByyIG& ig,QObject *parent/*=0*/ ) 
	: QObject(parent)
	,myIG(ig)
	,myMeasureItem(0)
{
	addCreator("Line",new ByyLineMeasureCreator);
	addCreator("Polygon",new ByyPolygonMeasureCreator);
	addCreator("IntervisibilityLine",new ByyIntervisibilityLineCreator);
	addCreator("IntervisibilityFan",new ByyIntervisibilityFanCreator);
}

ByyMeasureProcesser::~ByyMeasureProcesser()
{
	qDeleteAll(myCreators);
}

void ByyMeasureProcesser::addCreator( const QString& type,ByyMeasureItemCreator* creator )
{
	myCreators[type]=creator;
}

void ByyMeasureProcesser::measure( const QString& type )
{
	CreatorMap::iterator it=myCreators.find(type);

	if (it!=myCreators.end())
	{
		release();

		myMeasureItem=it.value()->create(myIG);
		connect(myMeasureItem,SIGNAL(finished()),this,SLOT(release()),Qt::QueuedConnection);
	}
}

void ByyMeasureProcesser::release()
{
	if (myMeasureItem)
	{
		delete myMeasureItem;
		myMeasureItem=0;
	}
}
