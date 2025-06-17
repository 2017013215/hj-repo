#include "StdAfx.h"
#include "vrlConnectWidget.h"

#include "ByyCore.h"
#include "driverManager.h"
#include "driver.h"

ByyVRLConnectWidget::ByyVRLConnectWidget( ByyCore& core,QWidget *parent /*= 0*/ ) 
	: QWidget(parent)
	,myCore(core)
{
	ui.setupUi(this);

	QList<ByyDriver*>& drivers=myCore.driverManager().drivers();

	QTreeWidgetItem *item=0;


	for (int i=0;i!=drivers.size();++i)
	{
		if (drivers[i]->isVRLink())
		{
			item=new QTreeWidgetItem;
			item->setText(0,drivers[i]->instanceName());
			updateItem(drivers[i],item);
			item->setData(0,Qt::UserRole,(int)drivers[i]);

			ui.treeWidget->addTopLevelItem(item);
		}
	}

	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(onConnect()));
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(onDisconnect()));

	connect(ui.treeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),this,SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
}

ByyVRLConnectWidget::~ByyVRLConnectWidget()
{

}

void ByyVRLConnectWidget::onCurrentItemChanged( QTreeWidgetItem *current,QTreeWidgetItem *previous )
{
	ByyDriver* driver=current?(ByyDriver*)current->data(0,Qt::UserRole).toInt():0;

	updateButton(driver);
}

void ByyVRLConnectWidget::onConnect()
{
	QTreeWidgetItem *current=ui.treeWidget->currentItem();

	ByyDriver* driver=current?(ByyDriver*)current->data(0,Qt::UserRole).toInt():0;

	if (driver)
	{
		myCore.driverManager().startDriver(driver);
		updateButton(driver);
		updateItem(driver,current);
	}
}

void ByyVRLConnectWidget::onDisconnect()
{
	QTreeWidgetItem *current=ui.treeWidget->currentItem();

	ByyDriver* driver=current?(ByyDriver*)current->data(0,Qt::UserRole).toInt():0;

	if (driver)
	{
		myCore.driverManager().stopDriver(driver);
		updateButton(driver);
		updateItem(driver,current);
	}
}

void ByyVRLConnectWidget::updateButton( ByyDriver* driver )
{
	ui.pushButton->setEnabled(driver&&!driver->started());
	ui.pushButton_2->setEnabled(driver&&driver->started());
}

void ByyVRLConnectWidget::updateItem( ByyDriver* driver,QTreeWidgetItem* item )
{
	item->setText(1,QString::fromLocal8Bit(driver->started()?"ÒÑÁ¬½Ó":""));
}
