#include "StdAfx.h"
#include "measureActions.h"
#include "ByyApp.h"
#include "ByyMeasureProcesser.h"

ByyLineMeasureAction::ByyLineMeasureAction( ByyApp& app,QWidget* parent ) 
	: ByyAction(app,"ByyLineMeasureAction",parent)
{
	setText( QString::fromLocal8Bit("线段(&L)") );
	setIcon( QIcon("../data/icons/measure.png") );
}

ByyLineMeasureAction::~ByyLineMeasureAction()
{

}

void ByyLineMeasureAction::on_triggered()
{
	myApp.ig().measureProcesser().measure("Line");
}

ByyPolygonMeasureAction::ByyPolygonMeasureAction( ByyApp& app,QWidget* parent ) 
	: ByyAction(app,"ByyPolygonMeasureAction",parent)
{
	setText( QString::fromLocal8Bit("区域(&A)") );
	setIcon( QIcon("../data/icons/measure_polygon.png") );
}

ByyPolygonMeasureAction::~ByyPolygonMeasureAction()
{

}

void ByyPolygonMeasureAction::on_triggered()
{
	myApp.ig().measureProcesser().measure("Polygon");
}

ByyIntervisibilityLineAction::ByyIntervisibilityLineAction( ByyApp& app,QWidget* parent ) 
	: ByyAction(app,"ByyPolygonMeasureAction",parent)
{
	setText( QString::fromLocal8Bit("通视线(&L)") );
	setIcon( QIcon("../data/icons/LOS.png") );
}

ByyIntervisibilityLineAction::~ByyIntervisibilityLineAction()
{

}

void ByyIntervisibilityLineAction::on_triggered()
{
	myApp.ig().measureProcesser().measure("IntervisibilityLine");
}

ByyIntervisibilityFanAction::ByyIntervisibilityFanAction( ByyApp& app,QWidget* parent ) 
	: ByyAction(app,"ByyIntervisibilityFanAction",parent)
{
	setText( QString::fromLocal8Bit("通视圈(&C)") );
	setIcon( QIcon("../data/icons/los_fan.png") );
}

ByyIntervisibilityFanAction::~ByyIntervisibilityFanAction()
{

}

void ByyIntervisibilityFanAction::on_triggered()
{
	myApp.ig().measureProcesser().measure("IntervisibilityFan");
}