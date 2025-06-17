#include "StdAfx.h"
#include "ByyEntityInfoBoxWidgt.h"
#include "ByyEntityInfoWidget.h"
#include "ByyEntitySensorWidget.h"
#include "ByyDescribeWidget.h"
#include "ByyObjectIdManager.h"
#include "sensorVisualizer.h"
#include "sceneEntity.h"
#include "remoteEntity.h"
#include "ByySensorSR.h"
#include "ByyApp.h"
#include "ByyHdose.h"
#include "simuController.h"
#include "ByyRangeOfVisibilityVisualizer.h"

ByyEntityInfoBoxWidgt::ByyEntityInfoBoxWidgt( ByyApp& app,ByyEntityObject* entity/*=0*/,QWidget* parent/*=0*/,Qt::WindowFlags f /*= 0*/ ) 
	: ByyEntityObserver(app.core(),entity,parent,f)
	,myApp(app)
	,myLastUpdateTime(0)
{
	ui.setupUi(this);

	connect(ui.sensorTableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(onItemClicked(QTableWidgetItem*)));

	connect(&ByySelectionManager::instance(),SIGNAL(currentSelectionChanged(ByySelectionManager::SelectionType,unsigned int)),this,
		SLOT(onCurrentSelectionChanged(ByySelectionManager::SelectionType,unsigned int)));

	connect(&myCore,SIGNAL(postTick()),this,SLOT(updateParam()));
}

ByyEntityInfoBoxWidgt::~ByyEntityInfoBoxWidgt()
{

}

void ByyEntityInfoBoxWidgt::onCurrentSelectionChanged( ByySelectionManager::SelectionType type,unsigned int id )
{
	if (type==ByySelectionManager::Entity)
	{
		ByyEntityObject *entity=dynamic_cast<ByyEntityObject*>(ByyObjectIdManager::instance().getObject(id));

		if (entity)
		{
			setEntity(entity);
		}
	}
}

void ByyEntityInfoBoxWidgt::setEntity( ByyEntityObject *entity )
{
	ByyEntityObserver::setEntity(entity);

	ui.nameEdit->setText(QString::null);
	ui.forceEdit->setText(QString::null);
	ui.superiorEdit->setText(QString::null);
	ui.positionEdit->setText(QString::null);
	ui.altitudeEdit->setText(QString::null);
	ui.headingEdit->setText(QString::null);
	ui.speedEdit->setText(QString::null);

	updateSensor();
}

void ByyEntityInfoBoxWidgt::updateParam()
{
	if (!myCurrentEntity||!isVisible()||osg::Timer::instance()->time_s()-myLastUpdateTime<0.2)
		return;

	myLastUpdateTime=osg::Timer::instance()->time_s();

	ui.nameEdit->setText(myCurrentEntity->name());

	ui.forceEdit->setText(forceTypeString(myCurrentEntity->forceType()));

	ui.superiorEdit->setText(myCurrentEntity->superiorName());

	osg::Vec3d pos=myCurrentEntity->position();
	ui.positionEdit->setText(QString("%1,%2").arg(pos.x(),0,'f',4).arg(pos.y(),0,'f',4));

	ui.altitudeEdit->setText(QString::number(pos.z(),'f',2));

	osg::Vec3d ori=myCurrentEntity->orientation();
	ui.headingEdit->setText(QString::number(ori.x(),'f',2));

	ui.speedEdit->setText(QString::number(myCurrentEntity->speed(),'f',2));
}

void ByyEntityInfoBoxWidgt::updateSensor()
{
	while(ui.sensorTableWidget->rowCount())
	{
		ui.sensorTableWidget->removeRow(0);
	}

	if (!myCurrentEntity)
		return;
	
	ui.sensorTableWidget->setColumnHidden(1,myCurrentEntity->isLocal());

	ByyDescribeEntry *descEntry=myCurrentEntity->describeEntry();


	if (!descEntry)
		return;
	
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
		int row=i;

		ui.sensorTableWidget->insertRow(row);

		QTableWidgetItem *item=new QTableWidgetItem;
		item->setText(components[i].getTag());
		item->setTextAlignment(Qt::AlignCenter);

		if (row%2!=0)
		{
			item->setBackground(Qt::SolidPattern);
			item->setBackgroundColor(QColor(249,249,249));
		}

		ui.sensorTableWidget->setItem(row,0,item);

		if (sensorSR)
		{
			QString powerText=QString::fromLocal8Bit(sensorSR->getDevicePower(components[i].getTag())?"关机":"开机");
			SensorPowerButton *btn=new SensorPowerButton(myApp,powerText);

			btn->mySR=sensorSR;
			btn->myDevice=components[i].getTag();
			btn->myEntObj=myCurrentEntity;

			ui.sensorTableWidget->setCellWidget(row,1,btn);
		}

		item=new QTableWidgetItem;
		item->setTextAlignment(Qt::AlignCenter);
		item->setCheckState(Qt::Checked);

		ui.sensorTableWidget->setItem(row,2,item);
	}

	ui.sensorTableWidget->resizeColumnsToContents();
	ui.sensorTableWidget->horizontalHeader()->setStretchLastSection(true);
}

QSize ByyEntityInfoBoxWidgt::sizeHint() const
{
	return QSize(150,200);
}

void ByyEntityInfoBoxWidgt::onItemClicked( QTableWidgetItem *item )
{
	if (item->column()!=2)
		return;

	QString tag=ui.sensorTableWidget->item(item->row(),0)->text();

	if(!myCurrentEntity)
		return;

	ByySceneEntity* sceneObj=myApp.ig().sceneObjectManager()->findEntity(myCurrentEntity);

	if (!sceneObj)
		return;

	QList<ByyStateVisualizer*>& stateVisualizers=sceneObj->getStateVisualizerList();


	for (int i=0;i!=stateVisualizers.size();++i)
	{
		ByySensorVisualizer* sensorVisualizer=dynamic_cast<ByySensorVisualizer*>(stateVisualizers[i]);

		if (!sensorVisualizer)
			continue;

		if (sensorVisualizer->getSensorTag()==tag)
		{
			sensorVisualizer->setVisible(item->checkState()==Qt::Checked);
			break;
		}
	}

	for (int i=0;i!=stateVisualizers.size();++i)
	{
		ByyRangeOfVisibilityVisualizer* rangeVisualizer=dynamic_cast<ByyRangeOfVisibilityVisualizer*>(stateVisualizers[i]);

		if (!rangeVisualizer)
			continue;

		if (rangeVisualizer->getSensorTag()==tag)
		{
			rangeVisualizer->setVisible(item->checkState()==Qt::Checked);
			break;
		}
	}
}

void SensorPowerButton::switchPower()
{
	bool newState=mySR->switchPower(myDevice);

	setText(QString::fromLocal8Bit(newState?"关机":"开机"));

	myApp.hdose().simuController().sendSenorOper(myEntObj->id(),myDevice,newState);
}
