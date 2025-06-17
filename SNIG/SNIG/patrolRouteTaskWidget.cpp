#include "StdAfx.h"
#include "patrolRouteTaskWidget.h"
#include "patrolRouteTask.h"

#include "ByyRoute.h"

#include "comApp.h"
#include "ByyApp.h"
#include "ByyHdose.h"

#include "entityTaskWidget.h"

ByyPatrolRouteTaskWidget::ByyPatrolRouteTaskWidget( ByyIG& ig,QWidget *parent /*= 0*/ )
	: ByyTaskEditorWidget(ig,parent)
{
	ui.setupUi(this);

	QObject::connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(reject()));

	QList<ByyRoute*> routes;
	if(comApp::instance()->getApp()->hdose().mode() == ByyHdose::Join)
	{
		routes=ig.core().remoteObjectManager().findControlObjects<ByyRoute*>();
	}
	else
	{
		routes=ig.core().localObjectManager().findControlObjects<ByyRoute*>();
	}

	
	ByyForceType side = dynamic_cast<ByyEntityTaskWidget*>(parent)->myCurrentEntity->forceType();

	for (int i=0;i!=routes.size();++i)
	{
		if(side == routes[i]->forceType())
			ui.comboBox->addItem(routes[i]->name());
	}
}

ByyPatrolRouteTaskWidget::~ByyPatrolRouteTaskWidget()
{

}

void ByyPatrolRouteTaskWidget::init( ByyTask* task )
{
	myTask=dynamic_cast<ByyPatrolRouteTask*>(task);

	ui.nameEdit->setText(myTask->name());
	ui.startTimeSpinBox->setValue(myTask->startTime());
	ui.patrolTimeEdit->setText(QString::number(myTask->patrolTime()));

	int index=ui.comboBox->findText(myTask->route());
	ui.comboBox->setCurrentIndex(index);
}

ByyTask* ByyPatrolRouteTaskWidget::task()
{
	return myTask;
}

void ByyPatrolRouteTaskWidget::accept()
{
	myTask->setName(ui.nameEdit->text());
	myTask->setStartTime(ui.startTimeSpinBox->value());
	myTask->setRoute(ui.comboBox->currentText());
	myTask->setPatrolTime(ui.patrolTimeEdit->text().toInt());

	close();
	emit accepted();
}

void ByyPatrolRouteTaskWidget::reject()
{
	close();
	emit rejected();
}
