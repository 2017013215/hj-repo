#include "StdAfx.h"
#include "ByyEntitySensorWidget.h"

#include "entityObject.h"
#include "ByyDescribeEntry.h"
#include "ByyDescribeComponent.h"
#include "remoteEntity.h"
#include "ByySensorSR.h"
#include "ByyApp.h"
#include "ByyHdose.h"
#include "simuController.h"
#include "ByyEntityInfoBoxWidgt.h"

ByyEntitySensorWidget::ByyEntitySensorWidget( ByyApp& app,ByyEntityObject* entity/*=0*/,QWidget* parent/*=0*/,Qt::WindowFlags f /*= 0*/ ) 
	:ByyEntityObserver(app.core(),entity,parent,f)
	,myApp(app)
{
	ui.setupUi(this);

	setEntity(entity);
}

ByyEntitySensorWidget::~ByyEntitySensorWidget()
{

}

void ByyEntitySensorWidget::setEntity( ByyEntityObject *entity )
{
	ByyEntityObserver::setEntity(entity);

	while (ui.tableWidget->rowCount()>1)
	{
		ui.tableWidget->removeRow(1);
	}

	if (!entity)
		return;


	ui.tableWidget->setColumnHidden(1,entity->isLocal());

	ByyDescribeEntry *descEntry=myCurrentEntity->describeEntry();


	if (descEntry)
	{
		const ByyDescribeEntry::ComponentList& components=descEntry->getComponent("sensor");

		ByyRemoteEntity *remoteEnt=0;
		ByySensorSR *sensorSR=0;

		if (!myCurrentEntity->isLocal())
		{
			remoteEnt=dynamic_cast<ByyRemoteEntity*>(myCurrentEntity);

			if (remoteEnt)
			{
				sensorSR=remoteEnt->getOrCreateSR<ByySensorSR>();
			}

		}

		for (int i=0;i!=components.size();++i)
		{
			int row=i+1;

			ui.tableWidget->insertRow(row);

			QTableWidgetItem *item=new QTableWidgetItem;
			item->setText(components[i].getTag());
			item->setTextAlignment(Qt::AlignCenter);

			if (row%2!=0)
			{
				item->setBackground(Qt::SolidPattern);
				item->setBackgroundColor(QColor(249,249,249));
			}

			ui.tableWidget->setItem(row,0,item);

			if (sensorSR)
			{
				QString powerText=QString::fromLocal8Bit(sensorSR->getDevicePower(components[i].getTag())?"关机":"开机");
				SensorPowerButton *btn=new SensorPowerButton(myApp,powerText);

				btn->mySR=sensorSR;
				btn->myDevice=components[i].getTag();
				btn->myEntObj=myCurrentEntity;

				ui.tableWidget->setCellWidget(row,1,btn);
			}
		}
	}


}