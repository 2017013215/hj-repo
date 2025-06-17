#include "StdAfx.h"
#include "ByyApp.h"
#include "ByyControlObjectOperator.h"
#include "ByySelectionManager.h"
#include "ByyObjectIdManager.h"
#include "tacticalGraphicsProcesser.h"
#include "ctrlObjEditWidget.h"
#include "mainWindow.h"

ByyControlObjectOperator::ByyControlObjectOperator(QObject *parent)
	: QObject(parent)
	,myApp(0)
{
	myEditVertexAction=new QAction(QIcon("../data/icons/UtilityDocumentEdit.png"),QString::fromLocal8Bit("±à¼­¶¥µã"),this);
	myEditAttributeAction=new QAction(QIcon("../data/icons/UtilityDocumentEdit.png"),QString::fromLocal8Bit("±à¼­ÊôÐÔ..."),this);
	myRemoveAction=new QAction(QIcon("../data/icons/UtilityClose.png"),QString::fromLocal8Bit("É¾³ý"),this);

	connect(myEditVertexAction,SIGNAL(triggered()),SLOT(onEditVertex()));
	connect(myEditAttributeAction,SIGNAL(triggered()),SLOT(onEditAttribute()));
	connect(myRemoveAction,SIGNAL(triggered()),SLOT(onRemove()));
}

ByyControlObjectOperator::~ByyControlObjectOperator()
{

}

void ByyControlObjectOperator::setApp( ByyApp* app )
{
	myApp=app;
}

ByyControlObjectOperator& ByyControlObjectOperator::instance()
{
	static ByyControlObjectOperator _instance;
	return _instance;
}

void ByyControlObjectOperator::showContextMenu()
{
	ByyControlObject *obj=getSelectionObject();

	if (!obj) return;

	QMenu *menu=new QMenu;

	menu->addAction(myEditVertexAction);
	menu->addAction(myEditAttributeAction);
	menu->addAction(myRemoveAction);

	myEditVertexAction->setEnabled(obj->isLocal());
	myEditAttributeAction->setEnabled(obj->isLocal());

	menu->setAttribute(Qt::WA_DeleteOnClose);

	menu->popup(QCursor::pos());
}

void ByyControlObjectOperator::processDoubleClicked()
{

}

void ByyControlObjectOperator::onEditAttribute()
{
	editAttribute(getSelectionObject());
}

void ByyControlObjectOperator::onEditVertex()
{
	editVertex(getSelectionObject());
}

void ByyControlObjectOperator::onRemove()
{
	remove(getSelectionObject());
}

ByyControlObject* ByyControlObjectOperator::getSelectionObject()
{
	ByyControlObject* rel=0;

	if (ByySelectionManager::instance().selectionType()==ByySelectionManager::ControlObject)
	{
		rel=dynamic_cast<ByyControlObject*>(ByyObjectIdManager::instance().getObject(ByySelectionManager::instance().selectionId()));
	}

	return rel;
}

void ByyControlObjectOperator::editVertex( ByyControlObject* obj )
{
	myApp->ig().tacticalGraphicsProcesser().edit(obj);
}

void ByyControlObjectOperator::editAttribute( ByyControlObject* obj )
{
	ByyCtrlObjEditWidget *ctrlObjEditWidget=new ByyCtrlObjEditWidget(obj,&myApp->mainWindow());
	ctrlObjEditWidget->setWindowFlags(Qt::Window);
	ctrlObjEditWidget->setAttribute(Qt::WA_DeleteOnClose);
	ctrlObjEditWidget->show();
}

void ByyControlObjectOperator::remove( ByyControlObject* obj )
{
	if (obj->isLocal())
	{
		myApp->core().localObjectManager().removeControlObject(obj);
	}
	else if(obj->driverName()=="HDose")
	{
		myApp->hdose().removeControlObject(obj->id(),obj->name());
	}
}
