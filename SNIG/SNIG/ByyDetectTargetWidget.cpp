#include "StdAfx.h"
#include "ByyDetectTargetWidget.h"

#include "ByyApp.h"
#include "remoteEntity.h"
#include "ByySensorSR.h"
#include "ByyDetectObject.h"

ByyDetectTargetWidget::ByyDetectTargetWidget( ByyApp& app,ByyEntityObject* entity/*=0*/,QWidget* parent/*=0*/,Qt::WindowFlags f /*= 0*/ )
	:ByyEntityObserver(app.core(),entity,parent,f)
	,myApp(app)
	,myCurrentSensorSR(0)
{
	ui.setupUi(this);

	setEntity(entity);

	connect(&ByySensorSRSignaler::instance(),SIGNAL(discoverDetect(ByyDetectObject*,ByySensorSR*)),
		this,SLOT(appendDetectObject(ByyDetectObject*,ByySensorSR*)));
	connect(&ByySensorSRSignaler::instance(),SIGNAL(detectRemoved(ByyDetectObject*,ByySensorSR*)),
		this,SLOT(removeDetectObject(ByyDetectObject*,ByySensorSR*)));
}

ByyDetectTargetWidget::~ByyDetectTargetWidget()
{

}

void ByyDetectTargetWidget::setEntity( ByyEntityObject *entity )
{
	ByyEntityObserver::setEntity(entity);

	ByyRemoteEntity *remoteEnt=dynamic_cast<ByyRemoteEntity*>(myCurrentEntity);

	if (!remoteEnt)
		return;

	myCurrentSensorSR=remoteEnt->getOrCreateSR<ByySensorSR>();

	refresh();
}

void ByyDetectTargetWidget::refresh()
{
	while (ui.tableWidget->rowCount())
	{
		ui.tableWidget->removeRow(0);
	}

	myDetectObjectList.clear();
	 
	ByySensorSR::DeviceStateList* deviceList=myCurrentSensorSR->getDeviceList();

	ByySensorSR::DeviceStateList::iterator it=deviceList->begin();

	for (;it!=deviceList->end();++it)
	{
		ByySensorSR::DetectObjectMap& detectList=it->myDetectObjList;

		for (ByySensorSR::DetectObjectMap::iterator objIt=detectList.begin();objIt!=detectList.end();++objIt)
		{
			append(*objIt);
		}
	}

	ui.tableWidget->resizeColumnsToContents();

}

void ByyDetectTargetWidget::appendDetectObject( ByyDetectObject* obj,ByySensorSR* sensor )
{
	if (sensor!=myCurrentSensorSR)
		return;

	append(obj);
}

void ByyDetectTargetWidget::removeDetectObject( ByyDetectObject* obj,ByySensorSR* sensor )
{
	if (sensor!=myCurrentSensorSR)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index!=-1)
	{
		ui.tableWidget->removeRow(index);
		myDetectObjectList.removeAt(index);
	}
}

void ByyDetectTargetWidget::append( ByyDetectObject* obj )
{
	int row=ui.tableWidget->rowCount();

	ui.tableWidget->insertRow(row);

	myDetectObjectList.append(obj);

	ui.tableWidget->setItem(row,UpdateTime,new QTableWidgetItem(QString::number(obj->updateTime())));
	ui.tableWidget->setItem(row,BatchNo,new QTableWidgetItem(QString::number(obj->batchNo())));
	ui.tableWidget->setItem(row,TargetID,new QTableWidgetItem(QString::number(obj->targetID())));
	ui.tableWidget->setItem(row,TargetName,new QTableWidgetItem(obj->targetName()));
	ui.tableWidget->setItem(row,From,new QTableWidgetItem(obj->deviceName()));
	ui.tableWidget->setItem(row,CID,new QTableWidgetItem(QString::number(obj->getCID())));
	ui.tableWidget->setItem(row,Side,new QTableWidgetItem(QString::number(obj->forceType())));
	ui.tableWidget->setItem(row,EntityType,new QTableWidgetItem(obj->entityType().string()));
	ui.tableWidget->setItem(row,ThreatLevel,new QTableWidgetItem(QString::number(obj->threatLevel())));
	ui.tableWidget->setItem(row,Pos,new QTableWidgetItem(QString::number(obj->position().x())+","+QString::number(obj->position().y())+","+QString::number(obj->position().z())));
	ui.tableWidget->setItem(row,Velocity,new QTableWidgetItem(QString::number(obj->velocity())));
	ui.tableWidget->setItem(row,Course,new QTableWidgetItem(QString::number(obj->course())));
	ui.tableWidget->setItem(row,Distance,new QTableWidgetItem(QString::number(obj->distance())));
	ui.tableWidget->setItem(row,El,new QTableWidgetItem(QString::number(obj->El())));
	ui.tableWidget->setItem(row,Az,new QTableWidgetItem(QString::number(obj->Az())));
	ui.tableWidget->setItem(row,RadiationType,new QTableWidgetItem(QString::number(obj->radiationType())));
	ui.tableWidget->setItem(row,RadiationPt,new QTableWidgetItem(QString::number(obj->radiationPt())));
	ui.tableWidget->setItem(row,RadiationFreq,new QTableWidgetItem(QString::number(obj->radiationFreq())));
	ui.tableWidget->setItem(row,RadiationTu,new QTableWidgetItem(QString::number(obj->radiationTu())));
	ui.tableWidget->setItem(row,RadiationPRF,new QTableWidgetItem(QString::number(obj->radiationPRF())));
	ui.tableWidget->setItem(row,RadiationSignalType,new QTableWidgetItem(QString::number(obj->radiationSignalType())));
	ui.tableWidget->setItem(row,RadiationPulseType,new QTableWidgetItem(QString::number(obj->radiationPulseType())));

	connect(obj,SIGNAL(updateTimeChanged(double)),this,SLOT(updateTimeChanged(double)));
	connect(obj,SIGNAL(batchNoChanged(long)),this,SLOT(batchNoChanged(long)));
	connect(obj,SIGNAL(CIDChanged(long)),this,SLOT(CIDChanged(long)));
	connect(obj,SIGNAL(entityTypeChanged(ByyEntityType)),this,SLOT(entityTypeChanged(ByyEntityType)));
	connect(obj,SIGNAL(threatLevelChanged(long)),this,SLOT(threatLevelChanged(long)));
	connect(obj,SIGNAL(posChanged(osg::Vec3d)),this,SLOT(posChanged(osg::Vec3d)));
	connect(obj,SIGNAL(velocityChanged(double)),this,SLOT(velocityChanged(double)));
	connect(obj,SIGNAL(courseChanged(double)),this,SLOT(courseChanged(double)));
	connect(obj,SIGNAL(distanceChanged(double)),this,SLOT(distanceChanged(double)));
	connect(obj,SIGNAL(elChanged(double)),this,SLOT(elChanged(double)));
	connect(obj,SIGNAL(azChanged(double)),this,SLOT(azChanged(double)));
	connect(obj,SIGNAL(radiationTypeChanged(long)),this,SLOT(radiationTypeChanged(long)));
	connect(obj,SIGNAL(radiationPtChanged(double)),this,SLOT(radiationPtChanged(double)));
	connect(obj,SIGNAL(radiationFreqChanged(double)),this,SLOT(radiationFreqChanged(double)));

	connect(obj,SIGNAL(radiationTuChanged(double)),this,SLOT(radiationTuChanged(double)));
	connect(obj,SIGNAL(radiationPRFChanged(double)),this,SLOT(radiationPRFChanged(double)));
	connect(obj,SIGNAL(radiationSignalTypeChanged(double)),this,SLOT(radiationSignalTypeChanged(double)));
	connect(obj,SIGNAL(radiationPulseTypeChanged(double)),this,SLOT(radiationPulseTypeChanged(double)));

	connect(obj,SIGNAL(forceTypeChanged(ByyForceType)),this,SLOT(forceTypeChanged(ByyForceType)));
	connect(obj,SIGNAL(nameChanged(const QString&)),this,SLOT(nameChanged(const QString&)));
}

void ByyDetectTargetWidget::updateTimeChanged( double val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,UpdateTime)->setText(QString::number(val));
}

void ByyDetectTargetWidget::batchNoChanged( long val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,BatchNo)->setText(QString::number(val));
}

void ByyDetectTargetWidget::CIDChanged( long val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,CID)->setText(QString::number(val));
}

void ByyDetectTargetWidget::entityTypeChanged( ByyEntityType val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,EntityType)->setText(val.string());
}

void ByyDetectTargetWidget::threatLevelChanged( long val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,ThreatLevel)->setText(QString::number(val));
}

void ByyDetectTargetWidget::posChanged( osg::Vec3d val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,Pos)->setText(QString::number(val.x())+","+QString::number(val.y())+","+QString::number(val.z()));
}

void ByyDetectTargetWidget::velocityChanged( double val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,Velocity)->setText(QString::number(val));
}

void ByyDetectTargetWidget::courseChanged( double val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,Course)->setText(QString::number(val));
}

void ByyDetectTargetWidget::distanceChanged( double val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,Distance)->setText(QString::number(val));
}

void ByyDetectTargetWidget::elChanged( double val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,El)->setText(QString::number(val));
}

void ByyDetectTargetWidget::azChanged( double val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,Az)->setText(QString::number(val));
}

void ByyDetectTargetWidget::radiationTypeChanged( long val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,RadiationType)->setText(QString::number(val));
}

void ByyDetectTargetWidget::radiationPtChanged( double val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,RadiationPt)->setText(QString::number(val));
}

void ByyDetectTargetWidget::radiationFreqChanged( double val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,RadiationFreq)->setText(QString::number(val));
}

void ByyDetectTargetWidget::radiationTuChanged( double val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,RadiationTu)->setText(QString::number(val));
}

void ByyDetectTargetWidget::radiationPRFChanged( double val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,RadiationPRF)->setText(QString::number(val));
}

void ByyDetectTargetWidget::radiationSignalTypeChanged( double val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,RadiationSignalType)->setText(QString::number(val));
}

void ByyDetectTargetWidget::radiationPulseTypeChanged( double val )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,RadiationPulseType)->setText(QString::number(val));
}

void ByyDetectTargetWidget::forceTypeChanged( ByyForceType ft )
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,Side)->setText(QString::number(ft));
}

void ByyDetectTargetWidget::nameChanged( const QString& name)
{
	ByyDetectObject *obj=dynamic_cast<ByyDetectObject*>(sender());

	if (!obj)
		return;

	int index=myDetectObjectList.indexOf(obj);

	if (index==-1)
		return;

	ui.tableWidget->item(index,TargetName)->setText(name);
}
