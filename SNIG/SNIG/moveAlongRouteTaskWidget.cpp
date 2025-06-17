#include "StdAfx.h"
#include "moveAlongRouteTaskWidget.h"
#include "moveAlongRouteTask.h"

#include "ByyRoute.h"

#include "comApp.h"
#include "ByyApp.h"
#include "ByyHdose.h"

#include "entityTaskWidget.h"

ByyMoveAlongRouteTaskWidget::ByyMoveAlongRouteTaskWidget( ByyIG& ig,QWidget *parent /*= 0*/ )
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

ByyMoveAlongRouteTaskWidget::~ByyMoveAlongRouteTaskWidget()
{

}

void ByyMoveAlongRouteTaskWidget::init( ByyTask* task )
{
	myTask=dynamic_cast<ByyMoveAlongRouteTask*>(task);

	ui.nameEdit->setText(myTask->name());
	ui.startTimeSpinBox->setValue(myTask->startTime());

	int index=ui.comboBox->findText(myTask->route());
	ui.comboBox->setCurrentIndex(index);
}

ByyTask* ByyMoveAlongRouteTaskWidget::task()
{
	return myTask;
}

void ByyMoveAlongRouteTaskWidget::accept()
{
	myTask->setName(ui.nameEdit->text());
	myTask->setStartTime(ui.startTimeSpinBox->value());
	myTask->setRoute(ui.comboBox->currentText());

	close();
	emit accepted();
}

void ByyMoveAlongRouteTaskWidget::reject()
{
	close();
	emit rejected();
}
