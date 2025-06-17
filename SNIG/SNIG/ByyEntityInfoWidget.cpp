#include "StdAfx.h"
#include "ByyEntityInfoWidget.h"

ByyEntityInfoWidget::ByyEntityInfoWidget(ByyCore& core,ByyEntityObject* entity,QWidget* parent,Qt::WindowFlags f) 
	:ByyEntityObserver(core,entity,parent,f)
{
	ui.setupUi(this);

	setEntity(entity);

	ui.tableWidget->setAlternatingRowColors(true);

	connect(&myTimer,SIGNAL(timeout()),this,SLOT(updateInfo()));

	myTimer.start(200);
}

ByyEntityInfoWidget::~ByyEntityInfoWidget()
{

}

void ByyEntityInfoWidget::setEntity( ByyEntityObject *entity )
{
	ByyEntityObserver::setEntity(entity);

	if (entity)
	{
		setWindowTitle(entity->name());
	}
}

void ByyEntityInfoWidget::updateInfo()
{
	if (!myCurrentEntity||!isVisible())
		return;

	QTableWidgetItem *item=ui.tableWidget->item(1,1);
	item->setText(QString::number(myCurrentEntity->id()));

	item=ui.tableWidget->item(2,1);
	item->setText(myCurrentEntity->name());

	item=ui.tableWidget->item(3,1);
	item->setText(myCurrentEntity->entityType().string());

	item=ui.tableWidget->item(4,1);
	item->setText(QString::number(myCurrentEntity->forceType()));

	item=ui.tableWidget->item(5,1);
	item->setText(myCurrentEntity->superiorName());

	item=ui.tableWidget->item(6,1);
	osg::Vec3d pos=myCurrentEntity->position();
	item->setText(QString("%1,%2").arg(pos.x(),0,'f',4).arg(pos.y(),0,'f',4));

	item=ui.tableWidget->item(7,1);
	item->setText(QString::number(pos.z(),'f',2));

	item=ui.tableWidget->item(8,1);
	osg::Vec3d ori=myCurrentEntity->orientation();
	item->setText(QString("%1,%2,%3").arg(ori.x(),0,'f',2).arg(ori.y(),0,'f',2).arg(ori.z(),0,'f',2));

	item=ui.tableWidget->item(9,1);
	item->setText(QString::number(myCurrentEntity->speed(),'f',2));
	
	/// xks, 20220506
	item=ui.tableWidget->item(10,1);	
	long hitPoint=100;
	switch(myCurrentEntity->damageState())
	{
	case 0: 
		hitPoint=100;
		break;
	case 1: 
		hitPoint=80;
		break;
	case 2: 
		hitPoint=40;
		break;
	case 3: 
		hitPoint=0;
		break;
	default:
		hitPoint=100;
	}
	/////
	item->setText(QString::number(hitPoint,'f',2));
}
