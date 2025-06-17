#include "StdAfx.h"
#include "moveToTaskWidget.h"
#include "moveToTask.h"
#include "ByyWaypoint.h"

#include "comApp.h"
#include "ByyApp.h"
#include "ByyHdose.h"

#include "entityTaskWidget.h"

ByyMoveToTaskWidget::ByyMoveToTaskWidget(ByyIG& ig,QWidget *parent)
	: ByyTaskEditorWidget(ig,parent)
	,myTask(NULL)
{
	ui.setupUi(this);
	
	QList<ByyWaypoint*> polygons;
	if(comApp::instance()->getApp()->hdose().mode() == ByyHdose::Join)
	{
		polygons=ig.core().remoteObjectManager().findControlObjects<ByyWaypoint*>();
	}
	else
	{
		polygons=ig.core().localObjectManager().findControlObjects<ByyWaypoint*>();
	}

	ByyForceType side = dynamic_cast<ByyEntityTaskWidget*>(parent)->myCurrentEntity->forceType();

	for (int i=0;i!=polygons.size();++i)
	{
		if(side == polygons[i]->forceType())
			ui.waypointComboBox->addItem(polygons[i]->name());
	}

	connect(ui.pushButton,SIGNAL(clicked()),SLOT(accept()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(reject()));
}

ByyMoveToTaskWidget::~ByyMoveToTaskWidget()
{
	
}

void ByyMoveToTaskWidget::init( ByyTask* task )
{
	myTask=dynamic_cast<ByyMoveToTask*>(task);

	ui.nameEdit->setText(myTask->name());
	ui.startTimeSpinBox->setValue(myTask->startTime());

	int index=ui.waypointComboBox->findText(myTask->waypoint());
	ui.waypointComboBox->setCurrentIndex(index);
}


ByyTask* ByyMoveToTaskWidget::task()
{
	return myTask;
}

void ByyMoveToTaskWidget::accept()
{
	myTask->setWaypoint(ui.waypointComboBox->currentText());

	myTask->setName(ui.nameEdit->text());
	myTask->setStartTime(ui.startTimeSpinBox->value());

	close();
	emit accepted();
}

void ByyMoveToTaskWidget::reject()
{
	close();
	emit rejected();
}