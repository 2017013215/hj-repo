#include "StdAfx.h"
#include "ByyAction.h"

ByyAction::ByyAction( ByyApp& app,const QString& objectName ,QWidget* parent) 
	:QAction(parent)
	,myApp(app)
{
	setObjectName(objectName);

	connect(this,SIGNAL(triggered()),this,SLOT(on_triggered()));
	connect(this,SIGNAL(triggered()),this,SLOT(checkEnabled()));
}

ByyAction::~ByyAction()
{

}

void ByyAction::on_triggered()
{

}

void ByyAction::checkEnabled()
{

}

ByySeparatorAction::ByySeparatorAction( ByyApp& app,QWidget* parent )
	:ByyAction(app,"Separator",parent)
{
	setSeparator(true);
}

ByySeparatorAction::~ByySeparatorAction()
{

}

ByyCheckableAction::ByyCheckableAction( ByyApp& app,const QString& objectName,QWidget* parent/*=0*/ )
	:ByyAction(app,objectName,parent)
{
	setCheckable(true);
}

ByyCheckableAction::~ByyCheckableAction()
{

}
