#include "StdAfx.h"
#include "patrolAreaTaskWidget.h"
#include "patrolAreaTask.h"

#include "ByyPolygon.h"

#include "comApp.h"
#include "ByyApp.h"
#include "ByyHdose.h"

#include "entityTaskWidget.h"

ByyPatrolAreaTaskWidget::ByyPatrolAreaTaskWidget( ByyIG& ig,QWidget *parent /*= 0*/ )
	: ByyTaskEditorWidget(ig,parent)
{
	ui.setupUi(this);

	QObject::connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(reject()));

	QList<ByyPolygon*> polygons;
	if(comApp::instance()->getApp()->hdose().mode() == ByyHdose::Join)
	{
		polygons=ig.core().remoteObjectManager().findControlObjects<ByyPolygon*>();
	}
	else
	{
		polygons=ig.core().localObjectManager().findControlObjects<ByyPolygon*>();
	}
	
	ByyForceType side = dynamic_cast<ByyEntityTaskWidget*>(parent)->myCurrentEntity->forceType();

	for (int i=0;i!=polygons.size();++i)
	{
		if(side == polygons[i]->forceType())
			ui.comboBox->addItem(polygons[i]->name());
	}

}

ByyPatrolAreaTaskWidget::~ByyPatrolAreaTaskWidget()
{

}

void ByyPatrolAreaTaskWidget::init( ByyTask* task )
{
	myTask=dynamic_cast<ByyPatrolAreaTask*>(task);

	ui.nameEdit->setText(myTask->name());
	ui.startTimeSpinBox->setValue(myTask->startTime());
	ui.patrolTimeEdit->setText(QString::number(myTask->patrolTime()));


	int index=ui.comboBox->findText(myTask->area());
	ui.comboBox->setCurrentIndex(index);
}

ByyTask* ByyPatrolAreaTaskWidget::task()
{
	return myTask;
}

void ByyPatrolAreaTaskWidget::accept()
{
	myTask->setName(ui.nameEdit->text());
	myTask->setStartTime(ui.startTimeSpinBox->value());
	myTask->setArea(ui.comboBox->currentText());
	myTask->setPatrolTime(ui.patrolTimeEdit->text().toInt());

	close();
	emit accepted();
}

void ByyPatrolAreaTaskWidget::reject()
{
	close();
	emit rejected();
}
